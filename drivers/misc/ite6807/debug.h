///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <debug.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/30
//   @fileversion: iTE6807_MCUSRC_1.07
//******************************************/

#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/kernel.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/notifier.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/of.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/sunxi-gpio.h>
#include <linux/of_gpio.h>
#include <linux/pm_runtime.h>
#include <linux/regulator/consumer.h>
#include <linux/compat.h>
#include <linux/sched.h>   //wake_up_process()
#include <linux/kthread.h> //kthread_create()、kthread_run()
#include <linux/err.h>




#ifndef Debug_message
#define Debug_message 0
#endif

#if Debug_message
	#define MHLRX_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define EQ_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define VIDEOTIMNG_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define iTE6805_DEBUG_INT_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define HDMIRX_VIDEO_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define HDMIRX_AUDIO_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define HDMIRX_DEBUG_PRINT(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define CEC_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define EDID_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define RCP_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define MHL3D_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define MHL_MSC_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define HDCP_DEBUG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define HDCP_DEBUG_PRINTF1(fmt, ...) printk(fmt, ##__VA_ARGS__);
    #define HDCP_DEBUG_PRINTF2(fmt, ...) printk(fmt, ##__VA_ARGS__);
	#define REG_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
	#define REG_MHL_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
	#define SPI_PRINTF(fmt, ...) printk(fmt, ##__VA_ARGS__);
	#define printinfo(fmt, ...)  printk("%s-<%d>:" fmt,  __func__, __LINE__, ##__VA_ARGS__);
	#define printinfo_s(fmt, ...)  printk(fmt, ##__VA_ARGS__);
#else
    #define MHLRX_DEBUG_PRINTF(fmt, ...)
    #define EQ_DEBUG_PRINTF(fmt, ...)
    #define VIDEOTIMNG_DEBUG_PRINTF(fmt, ...)
    #define iTE6805_DEBUG_INT_PRINTF(fmt, ...)
    #define HDMIRX_VIDEO_PRINTF(fmt, ...)
    #define HDMIRX_AUDIO_PRINTF(fmt, ...)
    #define HDMIRX_DEBUG_PRINT(fmt, ...)
    #define CEC_DEBUG_PRINTF(fmt, ...)
    #define EDID_DEBUG_PRINTF(fmt, ...)
    #define IT680X_DEBUG_PRINTF(fmt, ...)
    #define RCP_DEBUG_PRINTF(fmt, ...)
    #define MHL3D_DEBUG_PRINTF(fmt, ...)
	#define MHL_MSC_DEBUG_PRINTF(fmt, ...)
	#define REG_PRINTF(fmt, ...)
	#define SPI_PRINTF(fmt, ...)
	#define printinfo(fmt, ...)
	#define printinfo_s(fmt, ...)
#endif




#endif
