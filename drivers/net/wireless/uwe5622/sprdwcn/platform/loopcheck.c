#include <linux/completion.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <linux/timer.h>

#include "../include/wcn_dbg.h"
#include "loopcheck.h"
#include "wcn_glb.h"
#include "wcn_procfs.h"

#define LOOPCHECK_TIMER_INTERVAL      5
#define USERDEBUG	1
#define WCN_LOOPCHECK_INIT	1
#define WCN_LOOPCHECK_OPEN	2

#ifdef CONFIG_WCN_LOOPCHECK
struct wcn_loopcheck {
	unsigned long status;
	struct completion completion;
	struct delayed_work work;
	struct workqueue_struct *workqueue;
};

static struct wcn_loopcheck loopcheck;
unsigned int (*cp_assert_cb_matrix[2])(unsigned int type) = {0};
#endif
static struct completion atcmd_completion;
static struct mutex atcmd_lock;

int at_cmd_send(char *buf, unsigned int len)
{
	unsigned char *send_buf = NULL;
	struct mbuf_t *head, *tail;
	int num = 1;

	WCN_DEBUG("%s len=%d\n", __func__, len);
	if (unlikely(marlin_get_module_status() != true)) {
		WCN_ERR("WCN module have not open\n");
		return -EIO;
	}

	send_buf = kzalloc(len + PUB_HEAD_RSV + 1, GFP_KERNEL);
	if (!send_buf)
		return -ENOMEM;
	memcpy(send_buf + PUB_HEAD_RSV, buf, len);

	if (!sprdwcn_bus_list_alloc(mdbg_proc_ops[MDBG_AT_TX_OPS].channel,
				    &head, &tail, &num)) {
		head->buf = send_buf;
		head->len = len;
		head->next = NULL;
		sprdwcn_bus_push_list(mdbg_proc_ops[MDBG_AT_TX_OPS].channel,
				      head, tail, num);
	}
	return 0;
}

#ifdef CONFIG_WCN_LOOPCHECK
static void reset_cp(void)
{
	marlin_chip_en(0, 0);
	if (cp_assert_cb_matrix[0])
		cp_assert_cb_matrix[0](0);
	if (cp_assert_cb_matrix[1])
		cp_assert_cb_matrix[1](1);

}

static void loopcheck_work_queue(struct work_struct *work)
{
	int ret;
	unsigned long timeleft;
	char a[] = "at+loopcheck\r\n";

#ifndef CONFIG_WCN_LOOPCHECK
	return;
#endif

	if (!test_bit(WCN_LOOPCHECK_OPEN, &loopcheck.status))
		return;
	mutex_lock(&atcmd_lock);
	at_cmd_send(a, sizeof(a));

	timeleft = wait_for_completion_timeout(&loopcheck.completion, (3 * HZ));
	mutex_unlock(&atcmd_lock);
	if (!test_bit(WCN_LOOPCHECK_OPEN, &loopcheck.status))
		return;
	if (!timeleft) {
		WCN_ERR("didn't get loopcheck ack\n");
		WCN_INFO("start dump CP2 mem\n");
		if (USERDEBUG)
			mdbg_dump_mem();
		else
			reset_cp();
		return;
	}

	ret = queue_delayed_work(loopcheck.workqueue, &loopcheck.work,
				 LOOPCHECK_TIMER_INTERVAL * HZ);
}
#endif

void switch_cp2_log(bool flag)
{
	unsigned long timeleft;
	char a[32];
	unsigned char ret;

	WCN_INFO("%s - %s entry!\n", __func__, (flag ? "open" : "close"));
	mutex_lock(&atcmd_lock);
	sprintf(a, "at+armlog=%d\r\n", (flag ? 1 : 0));
	ret = at_cmd_send(a, sizeof(a));
	if (ret) {
		mutex_unlock(&atcmd_lock);
		WCN_ERR("%s fail!\n", __func__);
		return;
	}
	timeleft = wait_for_completion_timeout(&atcmd_completion, (3 * HZ));
	mutex_unlock(&atcmd_lock);
	if (!timeleft)
		WCN_ERR("didn't get %s ack\n", __func__);
}

void get_cp2_version(void)
{
	unsigned long timeleft;
	char a[] = "at+spatgetcp2info\r\n";
	unsigned char ret;

	WCN_INFO("%s entry!\n", __func__);
	mutex_lock(&atcmd_lock);
	ret = at_cmd_send(a, sizeof(a));
	if (ret) {
		mutex_unlock(&atcmd_lock);
		WCN_ERR("%s fail!\n", __func__);
		return;
	}
	timeleft = wait_for_completion_timeout(&atcmd_completion, (3 * HZ));
	mutex_unlock(&atcmd_lock);
	if (!timeleft)
		WCN_ERR("didn't get CP2 version\n");
}

#ifdef CONFIG_WCN_LOOPCHECK
void start_loopcheck(void)
{
	if (!test_bit(WCN_LOOPCHECK_INIT, &loopcheck.status) ||
	    test_and_set_bit(WCN_LOOPCHECK_OPEN, &loopcheck.status))
		return;
	WCN_INFO("%s\n", __func__);
	reinit_completion(&loopcheck.completion);
	queue_delayed_work(loopcheck.workqueue, &loopcheck.work, HZ);
}

void stop_loopcheck(void)
{
	if (!test_bit(WCN_LOOPCHECK_INIT, &loopcheck.status) ||
	    !test_and_clear_bit(WCN_LOOPCHECK_OPEN, &loopcheck.status))
		return;
	WCN_INFO("%s\n", __func__);
	complete_all(&loopcheck.completion);
	cancel_delayed_work_sync(&loopcheck.work);
}

void complete_kernel_loopcheck(void)
{
	complete(&loopcheck.completion);
}
#endif

void complete_kernel_atcmd(void)
{
	complete(&atcmd_completion);
}

int loopcheck_init(void)
{
#ifdef CONFIG_WCN_LOOPCHECK
	WCN_INFO("loopcheck_init\n");
	loopcheck.status = 0;
	init_completion(&loopcheck.completion);
	loopcheck.workqueue =
			create_singlethread_workqueue("WCN_LOOPCHECK_QUEUE");
	if (!loopcheck.workqueue) {
		WCN_ERR("WCN_LOOPCHECK_QUEUE create failed");
		return -ENOMEM;
	}
	set_bit(WCN_LOOPCHECK_INIT, &loopcheck.status);
	INIT_DELAYED_WORK(&loopcheck.work, loopcheck_work_queue);
#endif
	init_completion(&atcmd_completion);
	mutex_init(&atcmd_lock);

	return 0;
}

int loopcheck_deinit(void)
{
#ifdef CONFIG_WCN_LOOPCHECK
	stop_loopcheck();
	destroy_workqueue(loopcheck.workqueue);
	loopcheck.status = 0;
#endif
	mutex_destroy(&atcmd_lock);

	return 0;
}
