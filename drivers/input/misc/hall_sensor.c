#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/gpio.h>
#include <asm/io.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include "../init-input.h"
#include <linux/input.h>
#include <linux/pinctrl/consumer.h>
#include <linux/sunxi-gpio.h>


#ifdef CONFIG_HAS_EARLYSUSPEND
#include <linux/earlysuspend.h>
#endif


#define SUSPEND_STATUS (1)
#define RESUME_STATUS (0)

#define EARLY_SUSPEND_STATUS (1)
#define EARLY_RESUME_STATUS (0)



#define DBG_EN 0
#if (DBG_EN == 1)
	#define __dbg(x, arg...) printk("[HAL_SWI_DBG]"x, ##arg)
	#define __inf(x, arg...) printk(KERN_INFO"[HAL_SWI_INF]"x, ##arg)
#else
	#define __dbg(x, arg...)
	#define __inf(x, arg...)
#endif

#define __err(x, arg...) printk(KERN_ERR"[HAL_SWI_ERR]"x, ##arg)

char phys[32];
u32 int_number;
struct gpio_config irq_gpio;
struct input_dev *input_dev;
struct work_struct  work;
static struct workqueue_struct *hal_wq;
/*static int trigger_mode;*/
static int sys_status;
static int early_status;
static spinlock_t irq_lock;
static char irq_pin_name[8];
static const char *device_name;
/*static int resule_flag;*/
unsigned int old_level_status;

struct hal_data {
#ifdef CONFIG_HAS_EARLYSUSPEND
	struct early_suspend early_suspend;
#endif
};

#ifdef CONFIG_PM
static struct dev_pm_domain hal_pm_domain;
#endif

struct hal_data *hal_switch_data;
static unsigned int level_status; /*0:low level 1:high level*/
static unsigned int is_first = 1;

static void hall_work_func(struct work_struct *work)
{
	unsigned int config;
	unsigned long irqflags = 0;
	unsigned int irq_num;
	unsigned int filter_status;

	/* set gpio input func */
	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_FUNC, 0);
	pin_config_set(SUNXI_PINCTRL, irq_pin_name, config);

	level_status = gpio_get_value(int_number);

	__dbg("gpio value = %d\n", level_status);

    __dbg(" ********hall_work_func level_status = %d, old_level_status = %d, is_first =%d, sys_status = %d, early_status = %d\n",
			level_status, old_level_status, is_first, sys_status, early_status);

	msleep(10);
	filter_status = gpio_get_value(int_number);
	if (level_status != filter_status) {
		__dbg("maybe this is noise, ignore it");
		goto end;
	}

	if ((level_status != old_level_status) || (is_first == 1)) {
		old_level_status = level_status;
		if (level_status == 1) { /*leave off*/
			__dbg("******leave off hall sensor, status is suspend, input KEY_WAKEUP******\n");
			input_report_key(input_dev, KEY_WAKEUP, 1);
			input_sync(input_dev);
			msleep(100);
			input_report_key(input_dev, KEY_WAKEUP, 0);
			input_sync(input_dev);
		//} else if ((level_status == 0) && (sys_status == RESUME_STATUS) && (early_status == EARLY_RESUME_STATUS)) {/*close*/
		} else if (level_status == 0) {/*close*/
			__dbg("******close to hall sensor, status is resume, input KEY_SLEEP******\n");
			input_report_key(input_dev, KEY_SLEEP, 1);
			input_sync(input_dev);
			msleep(100);
			input_report_key(input_dev, KEY_SLEEP, 0);
			input_sync(input_dev);
		}

		is_first = 0;
	 }

end:
	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_FUNC, 6);
	pin_config_set(SUNXI_PINCTRL, irq_pin_name, config);

	spin_lock_irqsave(&irq_lock, irqflags);
	irq_num = gpio_to_irq(int_number);
	enable_irq(irq_num);
	spin_unlock_irqrestore(&irq_lock, irqflags);
}

irqreturn_t switch_handle(int irq, void *dev_id)
{
	unsigned long irqflags = 0;
	unsigned int irq_num;

	__dbg(" switch_handle enter\n");

	spin_lock_irqsave(&irq_lock, irqflags);
	irq_num = gpio_to_irq(int_number);
	disable_irq_nosync(irq_num);
	spin_unlock_irqrestore(&irq_lock, irqflags);

	queue_work(hal_wq, &work);

	return IRQ_HANDLED;
}

#ifdef CONFIG_PM
static int sunxi_hal_suspend(struct device *dev)
{
    unsigned long config;
	sys_status = SUSPEND_STATUS;
	/*old_level_status = 0;*/
	__dbg("%s,old_level_status = %d \n", __FUNCTION__, old_level_status);
	/*resule_flag = 0;*/

	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_DAT, 0xFFFF);
	pin_config_get(SUNXI_PINCTRL, irq_pin_name, &config);

/*	level_status = SUNXI_PINCFG_UNPACK_VALUE(config);*/
	level_status = gpio_get_value(int_number);
	old_level_status = level_status;
	__dbg("%s sys_status = %d level_status = %d\n", __FUNCTION__, sys_status, level_status);

	flush_workqueue(hal_wq);
	enable_irq_wake(gpio_to_irq(int_number));
	return 0;
}

static int sunxi_hal_resume(struct device *dev)
{
	unsigned long config;

	sys_status = RESUME_STATUS;

	__dbg("%s sys_status = %d \n", __FUNCTION__, sys_status);
	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_FUNC, 0);
	pin_config_set(SUNXI_PINCTRL, irq_pin_name, config);

	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_DAT, 0xFFFF);
	pin_config_get(SUNXI_PINCTRL, irq_pin_name, &config);

	level_status = gpio_get_value(int_number);

	__dbg(" %s level_status = %d, old_level_status = %d , is_first = %d\n", __FUNCTION__, level_status, old_level_status, is_first);
	if ((level_status == 1) && (old_level_status != level_status) && (is_first == 0)) {
		old_level_status  = level_status;
		__dbg("**********2***************\n");
		input_report_key(input_dev, KEY_WAKEUP, 1);
		input_sync(input_dev);
		udelay(20);
		input_report_key(input_dev, KEY_WAKEUP, 0);
		input_sync(input_dev);
	}
	if ((old_level_status != level_status) && level_status == 0) {
		old_level_status  = level_status;
		__dbg("%s 00000000000 old_level_status = %d,\n", __FUNCTION__, old_level_status);
	}

	config = SUNXI_PINCFG_PACK(SUNXI_PINCFG_TYPE_FUNC, 6);
	pin_config_set(SUNXI_PINCTRL, irq_pin_name, config);
	disable_irq_wake(gpio_to_irq(int_number));
	return 0;
}
#endif

static int switch_request_irq(void)
{
	int irq_number = 0;
	int ret = 0;

	input_dev = input_allocate_device();
	if (input_dev == NULL) {
		ret = -ENOMEM;
		__err("%s:Failed to allocate input device\n", __func__);
		return -1;
	}

	input_dev->evbit[0] = BIT_MASK(EV_SYN) | BIT_MASK(EV_KEY) | BIT_MASK(EV_ABS);
	sprintf(phys, "input/switch");

	if (device_name != NULL)
		input_dev->name = device_name;
	else
		input_dev->name = "switch";
	input_dev->phys = phys;
	input_dev->id.bustype = BUS_HOST;
	input_dev->id.vendor = 0x0002;
	input_dev->id.product = 0x0012;
	input_dev->id.version = 0x0102;
	set_bit(EV_KEY, input_dev->evbit);
	set_bit(EV_REL, input_dev->evbit);
	set_bit(KEY_SLEEP, input_dev->keybit);
	set_bit(KEY_WAKEUP, input_dev->keybit);

#ifdef CONFIG_PM
	hal_pm_domain.ops.suspend = sunxi_hal_suspend;
	hal_pm_domain.ops.resume = sunxi_hal_resume;
	input_dev->dev.pm_domain = &hal_pm_domain;
#endif
	ret = input_register_device(input_dev);
	if (ret) {
		__err("Unable to register %s input device\n", input_dev->name);
		return -1;
	}

#ifdef CONFIG_HAS_EARLYSUSPEND
	hal_switch_data = kzalloc(sizeof(*hal_switch_data), GFP_KERNEL);
	if (hal_switch_data == NULL) {
		__err("Alloc GFP_KERNEL memory failed.");
		ret = -ENOMEM;
		goto switch_err;
	}

	hal_switch_data->early_suspend.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN + 1;
	hal_switch_data->early_suspend.suspend = hal_switch_early_suspend;
	hal_switch_data->early_suspend.resume = hal_switch_late_resume;
	register_early_suspend(&hal_switch_data->early_suspend);
#endif
	/*ע���ж�*/
	irq_number = gpio_to_irq(int_number);
    __dbg("%s,irq_number = %d\n", __FUNCTION__, irq_number);
	if (!gpio_is_valid(irq_number)) {
		__err("map gpio [%d] to virq failed, errno = %d\n",
				int_number, irq_number);
		ret = -EINVAL;
		goto switch_err;
	}

	ret = devm_request_irq(&(input_dev->dev), irq_number, switch_handle,
			       IRQF_TRIGGER_RISING|IRQF_TRIGGER_FALLING|IRQF_NO_SUSPEND, "HALL_EINT", NULL);
	if (ret) {
		__err("request virq %d failed, errno = %d\n", irq_number, ret);
		ret = -EINVAL;
		goto switch_err;
	}
    spin_lock_init(&irq_lock);
	return 0;

switch_err:
	input_unregister_device(input_dev);
	return ret;
}

static int __init switch_init(void)
{

	int ret = -1;
	struct device_node *np = NULL;

	np = of_find_node_by_name(NULL, "hall_para");
	if (!np) {
		pr_err("ERROR! get hall_para failed, func:%s, line:%d\n", __FUNCTION__, __LINE__);
		goto devicetree_get_item_err;
	}
	if (!of_device_is_available(np)) {
		pr_err("%s: hall is not used\n", __func__);
		goto devicetree_get_item_err;
	}
	int_number = of_get_named_gpio_flags(np, "hall_int_port", 0, (enum of_gpio_flags *)&irq_gpio);
	if (!gpio_is_valid(int_number)) {
		pr_err("%s: hall_int_port is invalid. \n", __func__);
		goto devicetree_get_item_err;
	}
	of_property_read_string(np, "hall_name", &device_name);

	__dbg("%s int_number = %d \n", __FUNCTION__, int_number);

	sunxi_gpio_to_name(int_number, irq_pin_name);
	__dbg("%s,irq_pin_name = %s\n", __FUNCTION__, irq_pin_name);

	INIT_WORK(&work, hall_work_func);
	hal_wq = create_singlethread_workqueue("hall_wq");
	if (!hal_wq) {
		__err("%s:Creat hal_wq workqueue failed.\n", __func__);
		return 0;
	}
	flush_workqueue(hal_wq);
	ret = switch_request_irq();
	if (!ret) {
		__dbg("*********Register IRQ OK!*********\n");
	}
	return 0;

	/*add by clc*/
devicetree_get_item_err:
	__dbg("=========script_get_item_err============\n");
	return ret;
}

static void __exit switch_exit(void)
{
	unsigned int irq_num;
#ifdef CONFIG_HAS_EARLYSUSPEND
	unregister_early_suspend(&hal_switch_data->early_suspend);
	kfree(hal_switch_data);
#endif
    if (hal_wq) {
		destroy_workqueue(hal_wq);
	}

	irq_num = gpio_to_irq(int_number);
	devm_free_irq(&(input_dev->dev), irq_num, NULL);
	input_unregister_device(input_dev);
	return;
}

late_initcall(switch_init);
module_exit(switch_exit);

MODULE_DESCRIPTION("hallswitch Driver");
MODULE_LICENSE("GPL v2");
