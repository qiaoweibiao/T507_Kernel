#ifndef _IT6807_H_
#define _IT6807_H_
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
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif
#include "typedef.h"
#include "config.h"
//#include "iTE6805_I2C_RDWR.h"
#include "iTE6805_Global.h"
#include "version.h"
#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
#include "iTE6805_MHL_SYS.h"
#endif


#define _ALLWINNER_EVB_

iTE_u1 i2c_write_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device);
iTE_u1 i2c_read_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device);

#endif
