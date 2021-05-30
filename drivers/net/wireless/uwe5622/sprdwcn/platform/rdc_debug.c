#include <linux/file.h>
#include <linux/fs.h>
#include <linux/kthread.h>
#include <misc/marlin_platform.h>
#include <linux/vmalloc.h>

#include "mdbg_type.h"
#include "../include/wcn_dbg.h"
#include "rdc_debug.h"
#include "wcn_txrx.h"

#define WCN_DEBUG_RETRY_TIMES	3
#define MAX_PATH_LEN	110
#define MAX_PATH_NUM	2
static unsigned int wcn_cp2_log_limit_size = 1024 * 1024 * 20;
static unsigned int wcn_cp2_file_max_num = 2;
static char *wcn_cp2_config_path[MAX_PATH_NUM] = {
	"/data/unisoc_cp2log_config.txt",
	"/vendor/etc/wifi/unisoc_cp2log_config.txt"
};

/* cover_old: when reached wcn_cp2_file_max_num will write from 0 file
  again, otherwise log file will not be limited by wcn_cp2_file_max_num. */
static unsigned int wcn_cp2_log_cover_old = 1;

#define UNISOC_LOG_PATH "/data/unisoc_dbg"

static char wcn_cp2_log_num;
static char wcn_cp2_mem_num;
static loff_t wcn_cp2_log_pos = 0;
static loff_t wcn_cp2_mem_pos = 0;
static char wcn_cp2_log_path[MAX_PATH_LEN];
static char wcn_cp2_mem_path[MAX_PATH_LEN] = UNISOC_LOG_PATH"/unisoc_cp2mem_0.mem";
static char wcn_cp2_log_path_tmp[MAX_PATH_LEN] = UNISOC_LOG_PATH"/unisoc_cp2log_%d.txt";
static char wcn_cp2_mem_path_tmp[MAX_PATH_LEN] = UNISOC_LOG_PATH"/unisoc_cp2mem_%d.mem";
static char debug_inited = 0;
static char debug_user_inited = 0;
static char config_inited = 0;

static int wcn_mkdir(char *path)
{
	int result = 0;
	char cmd_path[] = "/system/bin/mkdir";
	char *cmd_argv[] = {cmd_path, "-p", path, NULL};
	char *cmd_envp[] = {"HOME=/", "PATH=/sbin:/bin:/system/bin", NULL};
	struct file *fp;

	result = call_usermodehelper(cmd_path, cmd_argv, cmd_envp,
		UMH_WAIT_PROC);

	/* check if the new dir is created.*/
	fp = filp_open(path, O_DIRECTORY, 0644);
	if (IS_ERR(fp)) {
		WCN_INFO("open %s error.\n", path);
		return -1;
	} else {
		WCN_INFO("open %s success.\n", path);
		filp_close(fp, NULL);
		return 0;
	}
}

static int wcn_find_cp2_file_num(char *path, loff_t *pos)
{
	int i;
	struct kstat config_stat;
	mm_segment_t fs_old;
	int ret = 0;
	/*first file whose size less than wcn_cp2_log_limit_size*/
	int first_small_file = 0;
	char first_file_set = 0;
	int first_file_size = 0;
	char wcn_cp2_file_path[MAX_PATH_LEN];
	int config_size = 0;
	int num = 0;
	int exist_file_num = 0;

	fs_old = get_fs();
	set_fs(KERNEL_DS);

	if (wcn_cp2_log_cover_old) {
		for (i = 0; i < wcn_cp2_file_max_num; i++) {
			sprintf(wcn_cp2_file_path, path, i);
			ret = vfs_stat(wcn_cp2_file_path, &config_stat);
			if (ret) {
				break;
			} else {
				exist_file_num++;
				config_size = (int)config_stat.size;
				if ((config_size < wcn_cp2_log_limit_size)
						&& (first_file_set == 0)) {
					first_small_file = i;
					first_file_set = 1;
					first_file_size = config_size;
				}
			}
		}
		/* file number reaches max num*/
		if (i == wcn_cp2_file_max_num) {
			num = first_small_file;
			*pos = first_file_size;
			/*if all the exist files reached wcn_cp2_log_limit_size
			 empty the 0 file.*/
			if (first_file_set == 0) {
				struct file *filp = NULL;
				sprintf(wcn_cp2_file_path, path, 0);
				WCN_INFO("%s: empty:%s\n", __func__,
					wcn_cp2_file_path);
				filp = filp_open(wcn_cp2_file_path,
					O_CREAT | O_RDWR | O_TRUNC, 0644);
				if (IS_ERR(filp))
					WCN_INFO("%s: can not empty:%s\n",
						__func__, wcn_cp2_file_path);
				else
					filp_close(filp, NULL);
			}
		} else {
			/*in case all exist files reached wcn_cp2_log_limit_size
			 ,the file number still not reach wcn_cp2_file_max_num*/
			if ((first_file_set == 0) && (exist_file_num != 0)) {
				/*use a new file*/
				num = i;
				*pos = 0;
			} else {
				num = first_small_file;
				*pos = first_file_size;
			}
		}
	} else {
		struct file *fp = NULL;
		num = 0;
		*pos = 0;
		sprintf(wcn_cp2_file_path, path, 0);
		fp = filp_open(wcn_cp2_file_path,
			O_CREAT | O_RDWR | O_TRUNC, 0644);
		if (IS_ERR(fp)) {
			WCN_INFO("%s :%s file is not exit\n",
				__func__, wcn_cp2_file_path);
		} else
			filp_close(fp, NULL);
	}
	set_fs(fs_old);
	return num;
}

int log_rx_callback(void *addr, unsigned int len)
{
	ssize_t ret;
	loff_t file_size = 0;
	struct file *filp;
	static int retry = 0;

	//WCN_INFO("log_rx_callback\n");
	if ((debug_inited == 0) && (debug_user_inited == 0))
		return 0;

	if (retry > WCN_DEBUG_RETRY_TIMES)
		return 0;

retry:
	filp = filp_open(wcn_cp2_log_path, O_CREAT | O_RDWR | O_APPEND, 0644);
	if (IS_ERR(filp)) {
		WCN_ERR("%s open %s error no.%ld retry:%d\n", __func__,
			wcn_cp2_log_path, PTR_ERR(filp), retry);

		/*in case the path no longer exist, creat path or change path.*/
		if ((PTR_ERR(filp) == -ENOENT) && (retry == 0)) {
			retry = 1;
			WCN_ERR("%s:%s is no longer exist!\n",
				__func__, wcn_cp2_log_path);
			if (wcn_mkdir("/data/unisoc_dbg") == 0) {
				wcn_set_log_file_path("/data/unisoc_dbg",
					strlen("/data/unisoc_dbg"));
				goto retry;
			} else {
				WCN_ERR("%s: mkdir failed,can only use /data\n",
					__func__);
				wcn_set_log_file_path("/data",
					strlen("/data"));
				goto  retry;
			}
		} else {
			retry++;
			if (PTR_ERR(filp) == -EACCES)
				WCN_ERR("%s: Permission denied.\n", __func__);
			else if (PTR_ERR(filp) == -ENOMEM)
				WCN_ERR("%s: no memory in system,"
				"please delete old log file.\n",
					__func__);
			return PTR_ERR(filp);
		}
	}

	file_size = wcn_cp2_log_pos;

	if (file_size > wcn_cp2_log_limit_size) {
		filp_close(filp, NULL);
		wcn_cp2_log_pos = 0;

		if (wcn_cp2_log_cover_old) {
			if ((wcn_cp2_log_num + 1) < wcn_cp2_file_max_num) {
				wcn_cp2_log_num++;
				sprintf(wcn_cp2_log_path, wcn_cp2_log_path_tmp,
					wcn_cp2_log_num);
			} else if ((wcn_cp2_log_num + 1) == wcn_cp2_file_max_num) {
				wcn_cp2_log_num = 0;
				sprintf(wcn_cp2_log_path, wcn_cp2_log_path_tmp,
					wcn_cp2_log_num);
			} else {
				WCN_INFO("%s error log num:%d\n",
					__func__, wcn_cp2_log_num);
				wcn_cp2_log_num = 0;
				sprintf(wcn_cp2_log_path, wcn_cp2_log_path_tmp,
					wcn_cp2_log_num);
			}
		} else {
			wcn_cp2_log_num++;
			sprintf(wcn_cp2_log_path, wcn_cp2_log_path_tmp,
				wcn_cp2_log_num);
		}

		WCN_INFO("%s cp2 log file is %s\n", __func__, wcn_cp2_log_path);
		filp = filp_open(wcn_cp2_log_path,
				 O_CREAT | O_RDWR | O_TRUNC, 0644);
		if (IS_ERR(filp)) {
			WCN_ERR("%s open wcn log file error no. %d\n",
				__func__, (int)IS_ERR(filp));
			return PTR_ERR(filp);
		}
	}

	ret = kernel_write(filp, addr, len, wcn_cp2_log_pos);
	if (ret != len) {
		WCN_ERR("wcn log write to file failed: %zd\n", ret);
		filp_close(filp, NULL);
		return ret < 0 ? ret : -ENODEV;
	}
	wcn_cp2_log_pos += ret;
	filp_close(filp, NULL);

	return 0;
}

int dumpmem_rx_callback(void *addr, unsigned int len)
{
	ssize_t ret;
	struct file *filp;
	static int first_time_open = 1;
	static int retry = 0;

	WCN_INFO("dumpmem_rx_callback\n");

	if (retry > WCN_DEBUG_RETRY_TIMES)
		return 0;

retry:
	if (first_time_open)
		filp = filp_open(wcn_cp2_mem_path,
			O_CREAT | O_RDWR | O_TRUNC, 0644);
	else
		filp = filp_open(wcn_cp2_mem_path,
			O_CREAT | O_RDWR | O_APPEND, 0644);
	if (IS_ERR(filp)) {
		WCN_ERR("%s open %s error no.%ld retry:%d\n", __func__,
			wcn_cp2_mem_path, PTR_ERR(filp), retry);

		/*in case the path no longer exist, creat path or change path.*/
		if ((PTR_ERR(filp) == -ENOENT) && (retry == 0)) {
			retry = 1;
			WCN_ERR("%s:%s is no longer exist!\n",
				__func__, wcn_cp2_mem_path);
			if (wcn_mkdir("/data/unisoc_dbg") == 0) {
				wcn_set_log_file_path("/data/unisoc_dbg",
					strlen("/data/unisoc_dbg"));
				goto retry;
			} else {
				WCN_ERR("%s: mkdir failed, can only use /data\n",
					__func__);
				wcn_set_log_file_path("/data",
					strlen("/data"));
				goto  retry;
			}
		} else {
			retry++;
			return PTR_ERR(filp);
		}
	}

	if (first_time_open)
		first_time_open = 0;

	ret = kernel_write(filp, addr, len, wcn_cp2_mem_pos);
	if (ret != len) {
		WCN_ERR("wcn mem write to file failed: %zd\n", ret);
		filp_close(filp, NULL);
		return ret < 0 ? ret : -ENODEV;
	}

	wcn_cp2_mem_pos += ret;
	filp_close(filp, NULL);

	return 0;
}

/* unit of log_file_limit_size is MByte. */
int wcn_set_log_file_limit_size(unsigned int log_file_limit_size)
{
	wcn_cp2_log_limit_size = log_file_limit_size * 1024 * 1024;
	WCN_INFO("%s = %d bytes\n", __func__, wcn_cp2_log_limit_size);
	return 0;
}

int wcn_set_log_file_max_num(unsigned int log_file_max_num)
{
	wcn_cp2_file_max_num = log_file_max_num;
	WCN_INFO("%s = %d\n", __func__, wcn_cp2_file_max_num);
	return 0;
}

int wcn_set_log_file_cover_old(unsigned int is_cover_old)
{
	if (is_cover_old == 1) {
		wcn_cp2_log_cover_old = is_cover_old;
		WCN_INFO("%s will cover old files!\n", __func__);
		return 0;
	} else if (is_cover_old == 0) {
		wcn_cp2_log_cover_old = is_cover_old;
		WCN_INFO("%s NOT cover old files!\n", __func__);
		return 0;
	} else {
		WCN_ERR("%s param is invalid!\n", __func__);
		return -1;
	}
}

int wcn_set_log_file_path(char *path, unsigned int path_len)
{
	char wcn_cp2_log_path_user_tmp[MAX_PATH_LEN] = {0};
	char wcn_cp2_mem_path_user_tmp[MAX_PATH_LEN] = {0};
	char wcn_cp2_log_path_user[MAX_PATH_LEN] = {0};
	char wcn_cp2_mem_path_user[MAX_PATH_LEN] = {0};
	char log_name[] = "/unisoc_cp2log_%%d.txt";
	char mem_name[] = "/unisoc_cp2mem_%%d.mem";
	char wcn_cp2_log_num_user;
	char wcn_cp2_mem_num_user;
	loff_t wcn_cp2_log_pos_user = 0;
	struct file *filp;

	/* 10 is the len of 0xFFFFFFFF to decimal num*/
	if (path_len > (MAX_PATH_LEN-10)) {
		WCN_ERR("%s: log path is too long:%d", __func__, path_len);
		return -1;
	}

	sprintf(wcn_cp2_log_path_user_tmp, path);
	sprintf(wcn_cp2_log_path_user_tmp + path_len, log_name);
	sprintf(wcn_cp2_mem_path_user_tmp, path);
	sprintf(wcn_cp2_mem_path_user_tmp + path_len, mem_name);

	wcn_cp2_log_num_user =
		wcn_find_cp2_file_num(wcn_cp2_log_path_user_tmp,
			&wcn_cp2_log_pos_user);
	sprintf(wcn_cp2_log_path_user, wcn_cp2_log_path_user_tmp,
		wcn_cp2_log_num_user);

	wcn_cp2_mem_num_user = 0; //only one mem_dump file
	sprintf(wcn_cp2_mem_path_user,
		wcn_cp2_mem_path_user_tmp, wcn_cp2_mem_num_user);

	//check if the new path is valid.
	filp = filp_open(wcn_cp2_log_path_user,
		O_CREAT | O_RDWR | O_APPEND, 0644);
	if (IS_ERR(filp)) {
		WCN_ERR("new path [%s] is invalid %d\n", wcn_cp2_log_path_user,
			(int)IS_ERR(filp));
		return PTR_ERR(filp);
	}
	filp_close(filp, NULL);

	debug_inited = 0;
	debug_user_inited = 0;

	strcpy(wcn_cp2_log_path, wcn_cp2_log_path_user);
	strcpy(wcn_cp2_mem_path, wcn_cp2_mem_path_user);
	strcpy(wcn_cp2_log_path_tmp, wcn_cp2_log_path_user_tmp);
	strcpy(wcn_cp2_mem_path_tmp, wcn_cp2_mem_path_user_tmp);
	wcn_cp2_log_pos = wcn_cp2_log_pos_user;
	wcn_cp2_mem_pos = 0;
	wcn_cp2_log_num = wcn_cp2_log_num_user;
	wcn_cp2_mem_num = wcn_cp2_mem_num_user;

	WCN_INFO("%s cp2 log file is %s\n", __func__, wcn_cp2_log_path);
	WCN_INFO("%s cp2 mem file is %s\n", __func__, wcn_cp2_mem_path);
	WCN_INFO("%s wcn_cp2_log_pos is %d\n", __func__, (int)wcn_cp2_log_pos);

	debug_user_inited = 1;

	return 0;
}

static void wcn_config_log_file(void)
{
	struct file *filp;
	loff_t offset = 0;
	struct kstat config_stat;
	int config_size = 0;
	int read_len = 0;
	mm_segment_t fs_old;
	int ret;
	char *buf;
	char *buf_end;
	char *limit_size = "wcn_cp2_log_limit_size=";
	char *max_num = "wcn_cp2_file_max_num=";
	char *cover_old = "wcn_cp2_file_cover_old=";
	char *log_path = "wcn_cp2_log_path=";
	char *cc = NULL;
	int config_limit_size = 0;
	int config_max_num = 0;
	int index = 0;

	fs_old = get_fs();
	set_fs(KERNEL_DS);
	for (index = 0; index < MAX_PATH_NUM; index++) {
		ret = vfs_stat(wcn_cp2_config_path[index], &config_stat);
		if (!ret) {
			config_size = (int)config_stat.size;
			WCN_INFO("%s: find config file:%s size:%d\n",
				 __func__, wcn_cp2_config_path[index],
				 config_size);
			break;
		}
	}
	set_fs(fs_old);
	if (index == MAX_PATH_NUM) {
		WCN_INFO("%s: there is no unisoc_cp2log_config.txt\n",
			 __func__);
		return;
	}

	buf = kzalloc(config_size+1, GFP_KERNEL);
	if(!buf) {
		WCN_ERR("%s:no more space[%d]!\n", __func__, config_size+1);
		return;
	}
	buf_end = buf + config_size;

	filp = filp_open(wcn_cp2_config_path[index], O_RDONLY, 0);
	if (IS_ERR(filp)) {
		WCN_ERR("%s: can not open log config file:%s\n",
			__func__, wcn_cp2_config_path[index]);
		goto out;
	}

	read_len = kernel_read(filp, offset, buf, config_size);
	if (read_len <= 0) {
		WCN_ERR("%s: can not read config file read_len:%d\n",
			__func__, read_len);
		goto out1;
	}

	buf[config_size+1] = '\0';
	//WCN_INFO("config_file:%s\n", buf);

	/* config wcn_cp2_log_limit_size */
	cc = strstr(buf, limit_size);
	if (cc == NULL || cc >= buf_end) {
		WCN_INFO("can not find limit_size in config file!\n");
		goto config_max_num;
	} else {
		config_limit_size =
			simple_strtol(cc + strlen(limit_size), &cc, 10);
		if (config_limit_size == 0) {
			WCN_ERR("config_limit_size invalid!\n");
			goto config_max_num;
		}
	}
	if ((cc[0] == 'M') || (cc[0] == 'm')) {
		config_limit_size = config_limit_size*1024*1024;
	} else if ((cc[0] == 'K') || (cc[0] == 'k')) {
		config_limit_size = config_limit_size*1024;
	}

	wcn_cp2_log_limit_size = config_limit_size;

config_max_num:
	/* config wcn_cp2_file_max_num */
	cc = strstr(buf, max_num);
	if (cc == NULL || cc >= buf_end) {
		WCN_INFO("can not find max_num in config file!\n");
		goto config_cover_old;
	} else {
		config_max_num = simple_strtol(cc + strlen(max_num), &cc, 10);
		if (config_max_num == 0) {
			WCN_ERR("config_max_num invalid!\n");
			goto config_cover_old;
		}
	}
	wcn_cp2_file_max_num = config_max_num;

config_cover_old:
	/* config wcn_cp2_log_cover_old */
	cc = strstr(buf, cover_old);
	if (cc == NULL || cc >= buf_end) {
		WCN_INFO("can not find cover_old in config file!\n");
		goto config_new_path;
	} else {
		if (strncmp(cc + strlen(cover_old), "true", 4) == 0)
			wcn_cp2_log_cover_old = 1;
		else if (strncmp(cc + strlen(cover_old), "TRUE", 4) == 0)
			wcn_cp2_log_cover_old = 1;
		else if (strncmp(cc + strlen(cover_old), "false", 5) == 0)
			wcn_cp2_log_cover_old = 0;
		else if (strncmp(cc + strlen(cover_old), "FALSE", 5) == 0)
			wcn_cp2_log_cover_old = 0;
		else
			WCN_ERR("%s param is invalid!\n", cover_old);
	}

config_new_path:
	/* config wcn_cp2_log_path */
	cc = strstr(buf, log_path);
	if (cc == NULL || cc >= buf_end) {
		WCN_INFO("can not find log_path in config file!\n");
	} else {
		char *path_start = cc + strlen(log_path) + 1;
		char *path_end = strstr(path_start, "\"");
		if (path_end == NULL || path_end >= buf_end) {
			WCN_ERR("can not find log_path_end in config file!\n");
		} else {
			path_end[0] = '\0';
			wcn_set_log_file_path(path_start, path_end-path_start);
		}
	}

out1:
	filp_close(filp, NULL);
out:
	kfree(buf);
}

int wcn_debug_init(void)
{
	WCN_INFO("%s entry\n", __func__);

	/* config cp2 log if there is a config file.*/
	if (config_inited == 0) {
		wcn_config_log_file();
		config_inited = 1;
		WCN_INFO("%s:wcn_cp2_log_limit_size:%d Byte\n",
			__func__, wcn_cp2_log_limit_size);
		WCN_INFO("%s:wcn_cp2_file_max_num:%d\n",
			__func__, wcn_cp2_file_max_num);
		WCN_INFO("%s:wcn_cp2_log_cover_old is %d\n",
			__func__, wcn_cp2_log_cover_old);
	}

	if (debug_inited || debug_user_inited) {
		/*WCN_INFO("%s log path already initialized wcn_cp2_log_path:%s\n",
			__func__, wcn_cp2_log_path);*/
		return 0;
	}

	if (wcn_mkdir("/data/unisoc_dbg")) {
		wcn_set_log_file_path("/data", strlen("/data"));
		return 0;
	}

	wcn_cp2_log_num = wcn_find_cp2_file_num(wcn_cp2_log_path_tmp,
		&wcn_cp2_log_pos);
	sprintf(wcn_cp2_log_path, wcn_cp2_log_path_tmp, wcn_cp2_log_num);
	WCN_INFO("%s cp2 log file is %s\n", __func__, wcn_cp2_log_path);
	WCN_INFO("%s cp2 mem file is %s\n", __func__, wcn_cp2_mem_path);
	WCN_INFO("%s wcn_cp2_log_pos is %d\n", __func__, (int)wcn_cp2_log_pos);

	debug_inited = 1;

	return 0;
}
