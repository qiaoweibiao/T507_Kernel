/*
 * Copyright (C) 2015 Spreadtrum Communications Inc.
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */
#include <misc/wcn_bus.h>

#include "bufring.h"
#ifdef CONFIG_WCN_PCIE
#include "edma_engine.h"
#endif
#include "loopcheck.h"
#include "wcn_glb.h"
#include "wcn_procfs.h"
#include "mdbg_type.h"
#include "../include/wcn_dbg.h"
#include "../include/sc2355_glb.h"

#ifdef CONFIG_WCN_PCIE
int mdbg_log_cb(int channel, struct mbuf_t *head, struct mbuf_t *tail, int num);
int mdbg_log_push(int chn, struct mbuf_t **head,
		  struct mbuf_t **tail, int *num);
#else
int mdbg_log_read(int channel, struct mbuf_t *head,
		  struct mbuf_t *tail, int num);
#endif

static struct ring_device *ring_dev;
static unsigned long long rx_count;
static unsigned long long rx_count_last;

#ifdef CONFIG_WCN_PCIE
static struct mchn_ops_t mdbg_ringc_ops = {
	.channel = WCN_RING_RX,
	.inout = WCNBUS_RX,
	.hif_type = 1,
	/* bt cp log buffer size: 360*4, so setting as 1500 */
	.buf_size = 1500,
	/* bt log buf num + pld buf num = 4 + 8 */
	.pool_size = 16,
	.cb_in_irq = 0,
	.pop_link = mdbg_log_cb,
	.push_link = mdbg_log_push,
};
#else
static struct mchn_ops_t mdbg_ringc_ops = {
	.channel = WCN_RING_RX,
	.inout = WCNBUS_RX,
	.pool_size = 1,
	.pop_link = mdbg_log_read,
};
#endif

#ifdef CONFIG_WCN_PCIE
int mdbg_log_push(int chn, struct mbuf_t **head, struct mbuf_t **tail, int *num)
{
	WCN_INFO("%s enter num=%d, chn=%d,mbuf used done", __func__, *num, chn);
#ifdef CONFIG_WCN_PCIE
	edma_dump_glb_reg();
	edma_dump_chn_reg(chn);
#endif

	return 0;
}
#endif

bool mdbg_rx_count_change(void)
{
	rx_count = sprdwcn_bus_get_rx_total_cnt();

	WCN_INFO("rx_count:0x%llx rx_count_last:0x%llx\n",
		rx_count, rx_count_last);

	if ((rx_count == 0) && (rx_count_last == 0)) {
		return true;
	} else if (rx_count != rx_count_last) {
		rx_count_last = rx_count;
		return true;
	} else {
		return false;
	}
}

int mdbg_read_release(unsigned int fifo_id)
{
	return 0;
}

long mdbg_content_len(void)
{
	if (unlikely(!ring_dev))
		return 0;

	return mdbg_ring_readable_len(ring_dev->ring);
}

static long int mdbg_comm_write(char *buf,
				long int len, unsigned int subtype)
{
	unsigned char *send_buf = NULL;
	char *str = NULL;
	struct mbuf_t *head, *tail;
	int num = 1;

	if (unlikely(marlin_get_module_status() != true)) {
		WCN_ERR("WCN module have not open\n");
		return -EIO;
	}

	send_buf = kzalloc(len + PUB_HEAD_RSV + 1, GFP_KERNEL);
	if (!send_buf)
		return -ENOMEM;
	memcpy(send_buf + PUB_HEAD_RSV, buf, len);

	str = strstr(send_buf + PUB_HEAD_RSV, SMP_HEAD_STR);
	if (!str)
		str = strstr(send_buf + PUB_HEAD_RSV + ARMLOG_HEAD,
			     SMP_HEAD_STR);

	if (str) {
		int ret;

		/* for arm log to pc */
		WCN_INFO("smp len:%ld,str:%s\n", len, str);
		str[sizeof(SMP_HEAD_STR)] = 0;
		ret = kstrtol(&str[sizeof(SMP_HEAD_STR) - 1], 10,
							&ring_dev->flag_smp);
		WCN_INFO("smp ret:%d, flag_smp:%ld\n", ret,ring_dev->flag_smp);
		kfree(send_buf);
	} else {
		if (!sprdwcn_bus_list_alloc(
				mdbg_proc_ops[MDBG_AT_TX_OPS].channel,
				&head, &tail, &num)) {
			head->buf = send_buf;
			head->len = len;
			head->next = NULL;
			sprdwcn_bus_push_list(
				mdbg_proc_ops[MDBG_AT_TX_OPS].channel,
				head, tail, num);
		}
	}

	return len;
}

static void mdbg_ring_rx_task(struct work_struct *work)
{
	struct ring_rx_data *rx = NULL;
	struct mdbg_ring_t *ring = NULL;
	struct mbuf_t *mbuf_node;
	int i;
#ifndef CONFIG_WCN_PCIE
	struct bus_puh_t *puh = NULL;
#endif

	if (unlikely(!ring_dev)) {
		WCN_ERR("ring_dev is NULL\n");
		return;
	}

	spin_lock_bh(&ring_dev->rw_lock);
	rx = list_first_entry_or_null(&ring_dev->rx_head,
				      struct ring_rx_data, entry);
	if (rx) {
		list_del(&rx->entry);
	} else {
		WCN_ERR("tasklet something err\n");
		spin_unlock_bh(&ring_dev->rw_lock);
		return;
	}
	if (!list_empty(&ring_dev->rx_head))
		schedule_work(&ring_dev->mdbg_ring_wq);
	ring = ring_dev->ring;
	spin_unlock_bh(&ring_dev->rw_lock);

	for (i = 0, mbuf_node = rx->head; i < rx->num; i++,
		mbuf_node = mbuf_node->next) {
#ifndef CONFIG_WCN_PCIE
		rx->addr = mbuf_node->buf + PUB_HEAD_RSV;
		puh = (struct bus_puh_t *)mbuf_node->buf;
		//mdbg_ring_write(ring, rx->addr, puh->len);
		log_rx_callback(rx->addr, puh->len);
#else
		mdbg_ring_write(ring, mbuf_node->buf, mbuf_node->len);
#endif
	}
	sprdwcn_bus_push_list(mdbg_ringc_ops.channel,
			      rx->head, rx->tail, rx->num);
	wake_up_log_wait();
	kfree(rx);
}

int mdbg_log_read(int channel, struct mbuf_t *head,
		  struct mbuf_t *tail, int num)
{
	struct ring_rx_data *rx;

	if (ring_dev) {
		mutex_lock(&ring_dev->mdbg_read_mutex);
		rx = kmalloc(sizeof(*rx), GFP_KERNEL);
		if (!rx) {
			WCN_ERR("mdbg ring low memory\n");
			mutex_unlock(&ring_dev->mdbg_read_mutex);
			sprdwcn_bus_push_list(channel, head, tail, num);
			return 0;
		}
		mutex_unlock(&ring_dev->mdbg_read_mutex);
		spin_lock_bh(&ring_dev->rw_lock);
		rx->channel = channel;
		rx->head = head;
		rx->tail = tail;
		rx->num = num;
		list_add_tail(&rx->entry, &ring_dev->rx_head);
		spin_unlock_bh(&ring_dev->rw_lock);
		schedule_work(&ring_dev->mdbg_ring_wq);

	}

	return 0;
}

/*
 * PCIE's log callback in Interrupt context, can not use interface(kmalloc,
 * mutex) that may cause sleep. So handle it apart.
 * SDIO's log callback not in interrupt context, it in Kthread
 */
#ifdef CONFIG_WCN_PCIE
int mdbg_log_cb(int channel, struct mbuf_t *head, struct mbuf_t *tail, int num)
{
	struct mbuf_t *mbuf_node;
	int i;

	WCN_INFO("%s:seq=0x%x, num=%d\n", __func__,
		 *((u32 *)(head->buf + 12)), num);

	for (i = 0, mbuf_node = head; i < num; i++, mbuf_node = mbuf_node->next)
		mdbg_ring_write(ring_dev->ring, mbuf_node->buf, mbuf_node->len);

	sprdwcn_bus_push_list(channel, head, tail, num);
	wake_up_log_wait();

	return 0;
}
#endif

long int mdbg_send(char *buf, long int len, unsigned int subtype)
{
	long int sent_size = 0;

	WCN_DEBUG("BYTE MODE");
	__pm_stay_awake(&ring_dev->rw_wake_lock);
	sent_size = mdbg_comm_write(buf, len, subtype);
	__pm_relax(&ring_dev->rw_wake_lock);

	return sent_size;
}
EXPORT_SYMBOL_GPL(mdbg_send);

long int mdbg_receive(void *buf, long int len)
{
	return mdbg_ring_read(ring_dev->ring, buf, len);
}

int mdbg_tx_cb(int channel, struct mbuf_t *head,
	       struct mbuf_t *tail, int num)
{
#ifndef CONFIG_WCN_PCIE
	struct mbuf_t *mbuf_node;
	int i;

	mbuf_node = head;
	for (i = 0; i < num; i++, mbuf_node = mbuf_node->next) {
		kfree(mbuf_node->buf);
		mbuf_node->buf = NULL;
	}
#endif
	/* PCIe buf is witebuf[], not kmalloc, no need to free */
	sprdwcn_bus_list_free(channel, head, tail, num);

	return 0;
}

int mdbg_tx_power_notify(int chn, int flag)
{
	if (flag) {
		WCN_DEBUG("%s resume\n", __func__);
#ifdef CONFIG_WCN_LOOPCHECK
		start_loopcheck();
#endif
	} else {
		WCN_DEBUG("%s suspend\n", __func__);
#ifdef CONFIG_WCN_LOOPCHECK
		stop_loopcheck();
#endif
	}
	return 0;
}

static void mdbg_pt_ring_reg(void)
{
	sprdwcn_bus_chn_init(&mdbg_ringc_ops);
#ifdef CONFIG_WCN_PCIE
	prepare_free_buf(15, 1500, 16);
#endif
}

static void mdbg_pt_ring_unreg(void)
{
	sprdwcn_bus_chn_deinit(&mdbg_ringc_ops);
}

int mdbg_ring_init(void)
{
	int err = 0;

	ring_dev = kmalloc(sizeof(struct ring_device), GFP_KERNEL);
	if (!ring_dev)
		return -ENOMEM;

	ring_dev->ring = mdbg_ring_alloc(MDBG_RX_RING_SIZE);
	if (!(ring_dev->ring)) {
		WCN_ERR("Ring malloc error.");
		return -MDBG_ERR_MALLOC_FAIL;
	}

	wakeup_source_init(&ring_dev->rw_wake_lock, "mdbg_wake_lock");
	spin_lock_init(&ring_dev->rw_lock);
	mutex_init(&ring_dev->mdbg_read_mutex);
	INIT_LIST_HEAD(&ring_dev->rx_head);
	INIT_WORK(&ring_dev->mdbg_ring_wq, mdbg_ring_rx_task);
	ring_dev->flag_smp = 0;
	mdbg_pt_ring_reg();
	WCN_INFO("success!");

	mdbg_dev->ring_dev = ring_dev;

	return err;
}

void mdbg_ring_remove(void)
{
	struct ring_rx_data *pos, *next;

	MDBG_FUNC_ENTERY;
	mdbg_pt_ring_unreg();
	wakeup_source_destroy(&ring_dev->rw_wake_lock);
	mdbg_ring_destroy(ring_dev->ring);
	cancel_work_sync(&ring_dev->mdbg_ring_wq);
	list_for_each_entry_safe(pos, next, &ring_dev->rx_head, entry) {
		list_del(&pos->entry);
		kfree(pos);
	}
	kfree(ring_dev);
	ring_dev = NULL;
}
