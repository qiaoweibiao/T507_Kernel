#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mm.h>

#include "dbg_ini_util.h"
#include "sprdwl.h"

#define LOAD_BUF_SIZE 1024
#define DBG_INI_FILE_PATH "/data/misc/wifi/wifi_dbg.ini"

static int dbg_load_ini_resource(char *path, char *buf, int size)
{
	int ret;
	mm_segment_t oldfs;
	struct file *filp = (struct file *)-ENOENT;

	filp = filp_open(path, O_RDONLY, S_IRUSR);
	if (IS_ERR(filp) || !filp->f_op) {
		return -ENOENT;
	}

	oldfs = get_fs();
	set_fs(KERNEL_DS);

	ret = filp->f_op->read(filp, buf, size, &filp->f_pos);
	set_fs(oldfs);

	filp_close(filp, NULL);

	return ret;
}

static int get_valid_line(char *buf, int buf_size, char *line, int line_size)
{
	int i = 0;
	int rem = 0;
	char *p = buf;

	while (1) {
		if (p - buf >= buf_size)
			break;

		if (i >= line_size)
			break;

		if (*p == '#' || *p == ';')
			rem = 1;

		switch (*p) {
		case '\0':
		case '\r':
		case '\n':
			if (i != 0) {
				line[i] = '\0';
				return p - buf + 1;
			} else {
				rem = 0;
			}

			break;

		case ' ':
			break;

		default:
			if (rem == 0)
				line[i++] = *p;
			break;
		}
		p++;
	}

	return -1;
}

static void dbg_ini_parse(struct dbg_ini_cfg *cfg, char *buf, int size)
{
	int ret;
	int sec = 0;
	int index = 0;
	int left = size;
	char *pos = buf;
	char line[256];
	int status[MAX_SEC_NUM] = {0};
	unsigned long value;

	while (1) {
		ret = get_valid_line(pos, left, line, sizeof(line));
		if (ret < 0 || left < ret)
			break;

		left -= ret;
		pos += ret;

		if (line[0] == '[') {
			if (strcmp(line, "[SOFTAP]") == 0)
				sec = SEC_SOFTAP;
			else if (strcmp(line, "[DEBUG]") == 0)
				sec = SEC_DEBUG;
			else
				sec = SEC_INVALID;

			status[sec]++;
			if (status[sec] != 1) {
				pr_info("invalid section %s\n", line);
				sec = SEC_INVALID;
			}
		} else {
			while (line[index] != '=' && line[index] != '\0')
				index++;

			if (line[index] != '=')
				continue;

			line[index++] = '\0';

			switch (sec) {
			case SEC_SOFTAP:
				if (strcmp(line, "channel") == 0) {
					if (!kstrtoul(&line[index], 0, &value))
						cfg->softap_channel = value;
				}

				break;

			case SEC_DEBUG:
				if (strcmp(line, "log_level") == 0) {
					if (!kstrtoul(&line[index], 0, &value))
						if (value >= L_NONE)
							sprdwl_debug_level = value;
				}

				break;

			default:
				pr_info("drop: %s\n", line);
				break;
			}
		}
	}
}

int dbg_util_init(struct dbg_ini_cfg *cfg)
{
	int ret;
	char *buf;

	buf = kmalloc(LOAD_BUF_SIZE, GFP_KERNEL);
	if (buf == NULL)
		return -ENOMEM;

	ret = dbg_load_ini_resource(DBG_INI_FILE_PATH, buf, LOAD_BUF_SIZE);
	if (ret <= 0) {
		kfree(buf);
		return -EINVAL;
	}

	cfg->softap_channel = -1;
	dbg_ini_parse(cfg, buf, ret);

	kfree(buf);
	return 0;
}
