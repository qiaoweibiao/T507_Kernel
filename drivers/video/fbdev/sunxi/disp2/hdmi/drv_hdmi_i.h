/*
 * Allwinner SoCs hdmi driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef _DRV_HDMI_I_H_
#define  _DRV_HDMI_I_H_
#include <linux/module.h>
#include <linux/uaccess.h>
#include <asm/memory.h>
#include <asm/unistd.h>
#include "asm-generic/int-ll64.h"
#include "linux/kernel.h"
#include "linux/mm.h"
#include "linux/semaphore.h"
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>
#include <linux/sched.h>   /* wake_up_process() */
#include <linux/kthread.h> /* kthread_create()??kthread_run() */
#include <linux/err.h> /* IS_ERR()??PTR_ERR() */
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/cdev.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/of_irq.h>
#include <linux/of_address.h>
#include <linux/of_iommu.h>
#include <linux/of_device.h>
#include <linux/of_platform.h>

#include <video/sunxi_display2.h>
#include <video/drv_hdmi.h>

#include "hdmi_bsp.h"

extern u32 hdmi_print;
extern u32 rgb_only;
extern u32 hdmi_hpd_mask;/* 0x10: force unplug; 0x11: force plug */

#define OSAL_PRINTF(msg...)\
	do {\
		pr_warn("[HDMI] ");\
		pr_warn(msg);\
	} while (0)
#define hdmi_inf(msg...)\
	do {\
		if (hdmi_print) {\
		pr_warn("[HDMI] ");\
		pr_warn(msg);\
	} \
	} while (0)
#define hdmi__msg(msg...)\
	do {\
		if (hdmi_print) {\
			pr_warn("[HDMI] file:%s,line:%d:",\
					__FILE__, __LINE__);\
			pr_warn(msg);\
		} \
	} while (0)
#define hdmi_wrn(msg...)\
	do {\
		pr_warn("[HDMI WRN] file:%s,line:%d:    ",\
				__FILE__, __LINE__);\
		pr_warn(msg);\
	} while (0)
#define hdmi_here \
	do {\
		if (hdmi_print) {\
			pr_warn("[HDMI] file:%s,line:%d\n",\
					__FILE__, __LINE__);\
		} \
	} while (0)


s32 hdmi_init(struct platform_device *pdev);
s32 hdmi_exit(void);
extern s32 Fb_Init(u32 from);
s32 hdmi_hpd_state(u32 state);
s32 hdmi_hpd_event(void);

typedef struct {
	struct device           *dev;
	bool                    bopen;
	enum disp_tv_mode            mode;/* vic */
	u32                   base_hdmi;
	struct work_struct      hpd_work;
} hdmi_info_t;

#ifdef CONFIG_HDMI_CEC_STANDARD
/* cec_msg tx/rx_status field */
#define CEC_TX_STATUS_OK            (1 << 0)
#define CEC_TX_STATUS_NACK          (1 << 1)
#define CEC_TX_STATUS_ERROR         (1 << 2)
#define CEC_TX_STATUS_MAX_RETRIES   (1 << 3)

#define CEC_RX_STATUS_OK            (1 << 0)
#define CEC_RX_STATUS_TIMEOUT       (1 << 1)
#define CEC_RX_STATUS_FEATURE_ABORT (1 << 2)


/* CEC config ioctl */
#define CEC_S_PHYS_ADDR             _IOW('c', 1, __u16)
#define CEC_G_PHYS_ADDR             _IOR('c', 2, __u16)
#define CEC_S_LOG_ADDR              _IOW('c', 3, __u8)
#define CEC_G_LOG_ADDR              _IOR('c', 4, __u8)

/* CEC transmit/receive ioctl */
#define CEC_TRANSMIT                _IOWR('c', 5, struct cec_msg)
#define CEC_RECEIVE                 _IOWR('c', 6, struct cec_msg)

#define CEC_RXMSG_MAX		    20

struct hdmi_cec_drv {
	dev_t cec_devid;
	struct cdev *cec_cdev;
	struct class *cec_class;
	struct device *cec_dev;
};

struct hdmi_cec {
	struct cec_msg msg;
	struct list_head list;
};
#endif

enum hdcp_status {
	HDCP_DISABLE,
	HDCP_SUCCESS,
	HDCP_FAILED,
	HDCP_KSV_LIST_READY,
	HDCP_ERR_KSV_LIST_NOT_VALID,
};

extern hdmi_info_t ghdmi;
extern struct disp_video_timings video_timing[];

extern s32 hdmi_i2c_add_driver(void);
extern s32 hdmi_i2c_del_driver(void);

extern int disp_sys_script_get_item(char *main_name, char *sub_name, int value[], int type);

struct disp_hdmi_mode {
	enum disp_tv_mode mode;
	int hdmi_mode;/* vic */
};

#endif
