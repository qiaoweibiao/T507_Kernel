#include "it6807.h"

#define IT6807_NAME        ("it6807")
#define IT6807_MATCH_DTS_EN 1

struct i2c_client *this_client;
IT6807_CONFIG_INFO it6807_config_info;
extern struct i2c_client *mhl_client;
extern struct i2c_client *edid_client;
extern struct i2c_client *cec_client;

extern struct i2c_adapter *edid_adap;
extern struct i2c_adapter *mhl_adap;
extern struct i2c_adapter *cec_adap;

_iTE6805_DATA			iTE6805_DATA;
_iTE6805_VTiming		iTE6805_CurVTiming;
_iTE6805_PARSE3D_STR	iTE6805_EDID_Parse3D;

iTE_u1 i2c_write_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device)
{

	iTE_u8 i;
	int ret;
	struct i2c_adapter *adap = NULL;//this_client->adapter;
	struct i2c_msg msg;
	unsigned char data[256];


	if ((address>>1) == this_client->addr) {
		adap = this_client->adapter;
	} else if ((mhl_client != NULL) && ((address>>1) == mhl_client->addr)) {
		adap = mhl_client->adapter;
	} else if ((edid_client != NULL) && ((address>>1) == edid_client->addr)) {
		adap = edid_client->adapter;
	} else if ((cec_client != NULL) && ((address>>1) == cec_client->addr)) {
		adap = cec_client->adapter;
	} else {
		if (mhl_client != NULL) {

		} else {
			printinfo("mhl_client is null !!!\n");
		}

		if (edid_client != NULL) {

		} else {
			printinfo("edid_client is null !!!\n");
		}

		if (cec_client != NULL) {
			printinfo("cec_client is not null,,,cec_client->addr=0x%x\n", cec_client->addr);
		} else {
			printinfo("cec_client is null !!!\n");
		}
		adap = this_client->adapter;
	}
    if (adap == NULL) {
		printinfo("adap is null !");
		return 0;
    }
	data[0] = offset;
	if (byteno > 1) {
		for (i = 0; i < byteno - 1; i++) {
			data[i+1] = p_data[i];
		}
	} else {
		data[1] = p_data[0];
	}

	msg.addr = address>>1;
	msg.flags = 0;
	msg.len = byteno + 1;
	msg.buf = data;

	ret = i2c_transfer(adap, &msg, 1);

	if (ret >= 0) {
		return 1;
	} else {
		printinfo("error! slave = 0x%x, addr = 0x%2x\n ", address>>1, offset);
	}

	return 0;
}

iTE_u1 i2c_read_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data, iTE_u8 device)
{
	iTE_u8 i;
	int ret;
	struct i2c_adapter *adap = NULL;
	struct i2c_msg msg[2];
	unsigned char data[256];

	if ((address>>1) == this_client->addr) {
		adap = this_client->adapter;
	} else if ((mhl_client != NULL) && ((address>>1) == mhl_client->addr)) {
		adap = mhl_client->adapter;
	} else if ((edid_client != NULL) && ((address>>1) == edid_client->addr)) {
		adap = edid_client->adapter;
	} else if ((cec_client != NULL) && ((address>>1) == cec_client->addr)) {
		adap = cec_client->adapter;
	} else {
		if (mhl_client != NULL) {
			printinfo("mhl_client is not null,,,mhl_client->addr=0x%x\n", mhl_client->addr);
		} else {
			printinfo("mhl_client is null !!!\n");
		}

		if (edid_client != NULL) {
			printinfo("edid_client is not null,,,edid_client->addr=0x%x\n", edid_client->addr);
		} else {
			printinfo("edid_client is null !!!\n");
		}

		if (cec_client != NULL) {
			printinfo("cec_client is not null,,,cec_client->addr=0x%x\n", cec_client->addr);
		} else {
			printinfo("cec_client is null !!!\n");
		}
		adap = this_client->adapter;
	}
	if (adap == NULL) {
		printinfo("adap is null !");
		return 0;
    }
	data[0] = offset;

	/*
	* Send out the register address...
	*/
	msg[0].addr = address>>1;
	msg[0].flags = 0;
	msg[0].len = 1;
	msg[0].buf = &data[0];
	/*
	* ...then read back the result.
	*/
	msg[1].addr = address>>1;
	msg[1].flags = I2C_M_RD;
	msg[1].len = byteno;
	msg[1].buf = &data[1];

	ret = i2c_transfer(adap, msg, 2);
	if (ret >= 0) {
		if (byteno > 1) {
			for (i = 0; i < byteno - 1; i++) {
				p_data[i] = data[i+1];
			}
		} else {
			p_data[0] = data[1];
		}
		return 1;
	} else {
		printinfo("error! slave = 0x%x, addr = 0x%2x\n ", address>>1, offset);
	}
	return 0;
}

static const struct i2c_device_id it6807_id[] = {
	{"it6807_0", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, it6807_id);

static struct of_device_id it6807_dt_ids[] = {
	{.compatible = "allwinner,it6807_0",},
	{},
};
MODULE_DEVICE_TABLE(of, it6807_dt_ids);

static struct task_struct *it6807_fsm_task;

static int it6807_fsm_thread(void *data)
{
	while (1) {
//		printinfo_s("it6807_fsm_thread start ..\n");
		iTE6805_FSM();
		msleep(50);
//		printinfo_s("it6807_fsm_thread end ..\n");
		if (kthread_should_stop()) {
			break;
		}

	}
	return 0;
}

/*******************************************************
Function:
    I2c probe.
Input:
    client: i2c device struct.
    id: device id.
Output:
    Executive outcomes.
	0: succeed.
*******************************************************/
static int it6807_probe(struct i2c_client *client, const struct i2c_device_id *id)
{

	int err;
	int ret = 0;
	struct regulator *hdmi_3v3;
    printinfo("entered probe!");
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		return err;
	}

	this_client = client;
	printinfo("probe ok====>0x%x\n", client->addr);

	hdmi_3v3 = regulator_get(&client->dev, "it6807_hdmi_vcc_3v3");
	if (IS_ERR(hdmi_3v3)) {
		pr_err("[%s]: get it6807_hdmi_vcc_3v3 failed\n", __func__);
	} else {
		ret = regulator_set_voltage(hdmi_3v3, 3300000, 3300000);

		if (ret) {
			pr_err("[%s]: it6807_hdmi_vcc set vol failed\n", __func__);
//			return -EFAULT;
		}

		ret = regulator_enable(hdmi_3v3);
		if (ret != 0) {
			pr_err("[%s]: it6807_hdmi_vcc enable failed!\n", __func__);
			regulator_disable(hdmi_3v3);
			regulator_put(hdmi_3v3);
		}
	}

	it6807_fsm_task = kthread_create(it6807_fsm_thread, NULL, "it6807_fsm_thread");
	if (IS_ERR(it6807_fsm_task)) {
		printinfo_s("Unable to start kernel thread. ");
		ret = PTR_ERR(it6807_fsm_task);
		it6807_fsm_task = NULL;
		return ret;
	}
	//kthread_bind(it6807_fsm_task,2);    //这里是绑定在某个cpu上
	wake_up_process(it6807_fsm_task);
	return 0;
}

#if !IT6807_MATCH_DTS_EN
static int  it6807_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	struct i2c_adapter *adapter = client->adapter;
	if (client == NULL || client->adapter == NULL) {
		printinfo("it6807 detect client or client->adapter is NULL\n");
		return -1;
	}
	printinfo("enter it6807 detect==>the adapter number is %d,,,,client->addr=%x\n", adapter->nr, client->addr);
	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE_DATA)) {
		printinfo("it6807 i2c_check_functionality is fail\n");
		return -ENODEV;
	}

	if ((1 == adapter->nr) && (client->addr == 0x48)) {
		printinfo("Detected chip i2c1_it6807 at adapter %d, address 0x%02x\n", i2c_adapter_id(adapter), client->addr);
		//this_client = client;
		strlcpy(info->type, "it6807_0", I2C_NAME_SIZE);
	} else {
		printinfo("Detected chip i2c1_it6807 at adapter 1, address 0x48 fail\n");
		return -ENODEV;
	}
	return 0;
}
#endif

static int  it6807_remove(struct i2c_client *client)
{
	printk("it6807_remove:**********************\n");
	i2c_set_clientdata(client, NULL);
	return 0;
}

static int it6807_pm_suspend(struct device *dev)
{
	return 0;
}

static int it6807_pm_resume(struct device *dev)
{
	return 0;
}

static const unsigned short normal_i2c[] = {0x48, I2C_CLIENT_END};

static struct dev_pm_ops it6807_pm_ops = {
	.suspend = it6807_pm_suspend,
	.resume  = it6807_pm_resume,
};

static struct i2c_driver it6807_driver = {
	.class      = I2C_CLASS_HWMON,
	.driver = {
		.name   = IT6807_NAME,
		.owner    = THIS_MODULE,
#if IT6807_MATCH_DTS_EN
		.of_match_table = it6807_dt_ids,
#endif
#ifndef CONFIG_HAS_EARLYSUSPEND
#if defined(CONFIG_PM)
		.pm		  = &it6807_pm_ops,
#endif
#endif
    },
	.probe      = it6807_probe,
	.remove     = it6807_remove,
	.id_table   = it6807_id,
#if !IT6807_MATCH_DTS_EN
	.detect = it6807_detect,
	.address_list	= normal_i2c,
#endif
};

static int script_it6807_gpio_init(void)
{

	struct device_node *np = NULL;
	struct gpio_config config;

	np = of_find_node_by_name(NULL, "it6807");
	if (!np) {
		 pr_err ("ERROR! get it6807_para failed, func:%s, line:%d\n", __FUNCTION__, __LINE__);
		 return -1;
	}

	if (!of_device_is_available(np)) {
		pr_err ("%s: it6807 is not used\n", __func__);
		return -1;
	} else {
		it6807_config_info.it6807_used = 1;
	}

/*	ret = of_property_read_u32(np, "twi_addr",&it6807_config_info.address);
	if (ret) {
		pr_err("get it6807_twi_address is fail, %d\n", ret);
		return -1;
	}
*/
	it6807_config_info.reset_gpio = of_get_named_gpio_flags(np, "it6807_reset", 0, (enum of_gpio_flags *)&config);
	if (!gpio_is_valid(it6807_config_info.reset_gpio)) {
		pr_err ("%s: it6807_reset_gpio is invalid.\n", __func__);
		return -1;
	} else {
		pr_err ("%s: it6807_reset_gpio success. \n", __func__);
		if (0 != gpio_request(it6807_config_info.reset_gpio, NULL)) {
			printinfo("reset_gpio_request is failed\n");
			return -1;
		}
		if (0 != gpio_direction_output(it6807_config_info.reset_gpio, 1)) {
			printinfo("it6807_reset_gpio set err!\n");
			return -1;
		}
	}

	it6807_config_info.backlight_gpio = of_get_named_gpio_flags(np, "it6807_backlight", 0, (enum of_gpio_flags *)&config);
	if (!gpio_is_valid(it6807_config_info.backlight_gpio)) {
		pr_err("%s: backlight_gpio is invalid.\n", __func__);
		return -1;
	} else {
		pr_err ("%s: backlight_gpio success. \n", __func__);
		if (0 != gpio_request(it6807_config_info.backlight_gpio, NULL)) {
			printinfo("backlight_gpio_request is failed\n");
			return -1;
		}
		if (0 != gpio_direction_output(it6807_config_info.backlight_gpio, 0)) {
			printinfo("backlight_gpio set err!\n");
			return -1;
		}
	}

	return 1;
}


/*******************************************************
Function:
    Driver Install function.
Input:
    None.
Output:
    Executive Outcomes. 0---succeed.
********************************************************/
static int __init it6807_init(void)
{
    int ret = 0;
	printinfo("it6807 init:*************************\n");

    if (script_it6807_gpio_init() > 0) {
		gpio_set_value(it6807_config_info.reset_gpio, 0);
		mdelay(40);
		gpio_set_value(it6807_config_info.reset_gpio, 1);

		ret = i2c_add_driver(&it6807_driver);
		if (ret != 0)
			pr_err("Failed to register it6807 i2c driver : %d \n", ret);

	}


    return ret;
}

/*******************************************************
Function:
    Driver uninstall function.
Input:
    None.
Output:
    Executive Outcomes. 0---succeed.
********************************************************/
static void __exit it6807_exit(void)
{
	printk("it6807 exit:*************************\n");
	if (it6807_fsm_task) {
		printk("it6807_fsm_task is not exit,now need exit\n");
		kthread_stop(it6807_fsm_task);
		it6807_fsm_task = NULL;
		printk("kthread_stop:*************************\n");
	}

	i2c_put_adapter(edid_adap);
	i2c_put_adapter(cec_adap);
	i2c_put_adapter(mhl_adap);

	i2c_unregister_device(edid_client);
	i2c_unregister_device(cec_client);
	i2c_unregister_device(mhl_client);
	i2c_unregister_device(this_client);

	i2c_del_adapter(edid_adap);
	i2c_del_adapter(cec_adap);
	i2c_del_adapter(mhl_adap);

	i2c_del_driver(&it6807_driver);
    printk("it6807 exit:*************************done!\n");
}

module_init(it6807_init);
module_exit(it6807_exit);

MODULE_AUTHOR("<wanpeng@allwinnertech.com>");
MODULE_DESCRIPTION("IT6807 Driver");
MODULE_LICENSE("GPL");
