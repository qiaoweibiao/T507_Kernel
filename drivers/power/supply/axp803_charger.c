#define pr_fmt(x) KBUILD_MODNAME ": " x "\n"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/param.h>
#include <linux/jiffies.h>
#include <linux/platform_device.h>
#include <linux/power_supply.h>
#include <linux/fs.h>
#include <linux/ktime.h>
#include <linux/of.h>
#include <linux/timekeeping.h>
#include <linux/types.h>
#include <linux/string.h>
#include <asm/irq.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/pm_runtime.h>
#include <linux/gpio/consumer.h>
#include <linux/kthread.h>
#include <linux/freezer.h>
#include <linux/err.h>
#include <linux/mfd/axp2101.h>
#include "axp803_charger.h"

/* reg AXP803_STATUS */
#define AXP803_STATUS_AC_PRESENT	BIT(7)
#define AXP803_STATUS_AC_USED		BIT(6)
#define AXP803_STATUS_VBUS_PRESENT	BIT(5)
#define AXP803_STATUS_VBUS_USED		BIT(4)
#define AXP803_STATUS_BAT_CUR_DIRCT	BIT(2)

/* reg AXP803_MODE_CHGSTATUS */
#define AXP803_CHGSTATUS_BAT_CHARGING	BIT(6)
#define AXP803_CHGSTATUS_BAT_PRESENT	BIT(5)
#define AXP803_CHGSTATUS_BAT_PST_VALID	BIT(4)

/* reg AXP803_MODE_CHGSTATUS */
#define AXP803_FAULT_LOG_COLD		BIT(0)
#define AXP803_FAULT_LOG_CHA_CUR_LOW	BIT(2)
#define AXP803_FAULT_LOG_BATINACT	BIT(3)
#define AXP803_FAULT_LOG_OVER_TEMP	BIT(7)

/* AXP803_ADC_EN */
#define AXP803_ADC_BATVOL_ENABLE	BIT(7)
#define AXP803_ADC_BATCUR_ENABLE	BIT(6)
#define AXP803_ADC_TSVOL_ENABLE		BIT(0)

static struct axp803_charger_ps *axp803_charger;
static bool charger_debug;

static int axp803_get_bat_health(struct axp803_charger_ps *di)
{
	unsigned int reg_value;
	int ret = 0;

	ret = regmap_read(di->regmap, AXP803_MODE_CHGSTATUS, &reg_value);
	if (reg_value & AXP803_FAULT_LOG_BATINACT)
		return POWER_SUPPLY_HEALTH_DEAD;
	else if (reg_value & AXP803_FAULT_LOG_OVER_TEMP)
		return POWER_SUPPLY_HEALTH_OVERHEAT;
	else if (reg_value & AXP803_FAULT_LOG_COLD)
		return POWER_SUPPLY_HEALTH_COLD;
	else
		return POWER_SUPPLY_HEALTH_GOOD;
}

static inline int axp803_vbat_to_mV(u32 reg)
{
	return ((int)(((reg >> 8) << 4) | (reg & 0x000F))) * 1100 / 1000;
}

static int axp803_get_vbat(struct axp803_charger_ps *di)
{
	unsigned char temp_val[2];
	unsigned int res;
	int ret = 0;

	ret = regmap_bulk_read(di->regmap, AXP803_VBATH_RES, temp_val, 2);
	if (ret < 0)
		return ret;

	res = (temp_val[0] << 8) | temp_val[1];

	return axp803_vbat_to_mV(res);
}

static int axp803_set_chg_cur(struct axp803_charger_ps *di, int cur)
{
	uint8_t tmp = 0;
	struct regmap *map = di->regmap;

	if (cur == 0)
		regmap_update_bits(map, AXP803_CHARGE1, 0x80, 0x00);
	else
		regmap_update_bits(map, AXP803_CHARGE1, 0x80, 0x80);

	if (cur >= 200 && cur <= 2800) {
		tmp = (cur - 200) / 200;
		regmap_update_bits(map, AXP803_CHARGE1, 0x0f, tmp);
	} else if (cur < 200) {
		regmap_update_bits(map, AXP803_CHARGE1, 0x0f, 0x00);
	} else {
		regmap_update_bits(map, AXP803_CHARGE1, 0x0f, 0x0d);
	}

	return 0;
}

static int axp803_get_rest_cap(struct axp803_charger_ps *di)
{
	unsigned char temp_val[2];
	unsigned int reg_value;
	int batt_max_cap, coulumb_counter;
	int rest_vol = 0;
	int ocv_vol = 0;
	int rdc = 0;
	int ret = 0;

	ret = regmap_read(di->regmap, AXP803_CAP, &reg_value);
	if (ret)
		return ret;
	if (reg_value & 0x80)
		rest_vol = (int)(reg_value & 0x7F);

	ret = regmap_bulk_read(di->regmap, AXP803_COUCNT0, temp_val, 2);
	if (ret < 0)
		return ret;
	coulumb_counter = (((temp_val[0] & 0x7f) << 8) + temp_val[1])
						* 1456 / 1000;

	ret = regmap_bulk_read(di->regmap, AXP803_BATCAP0, temp_val, 2);
	if (ret < 0)
		return ret;
	batt_max_cap = (((temp_val[0] & 0x7f) << 8) + temp_val[1])
						* 1456 / 1000;

	if (charger_debug) {
		ret = regmap_bulk_read(di->regmap, AXP803_OCVBATH_RES, temp_val, 2);
		if (ret < 0)
			return ret;
		ocv_vol  =  ((temp_val[0] << 4) | (temp_val[1] & 0xF)) * 1100 / 1000;

		ret = regmap_bulk_read(di->regmap, AXP803_RDC0, temp_val, 2);
		if (ret < 0)
			return ret;
		rdc  =  (((temp_val[0] & 0x1f) << 8) + temp_val[1]) * 10742 / 10000;

		pr_debug("calc_info: ocv_vol:%d rdc:%d coulumb_counter:%d batt_max_cap:%d\n",
			ocv_vol, rdc, coulumb_counter, batt_max_cap);
	}

	return rest_vol;
}

static inline int axp_vts_to_mV(u16 reg)
{
	return ((int)(((reg >> 8) << 4) | (reg & 0x000F))) * 800 / 1000;
}

static inline int axp_vts_to_temp(int data,
		const struct axp_config_info *axp_config)
{
	int temp;

	if (data < 80 || !axp_config->pmu_bat_temp_enable)
		return 30;
	else if (data < axp_config->pmu_bat_temp_para16)
		return 80;
	else if (data <= axp_config->pmu_bat_temp_para15) {
		temp = 70 + (axp_config->pmu_bat_temp_para15-data)*10/
		(axp_config->pmu_bat_temp_para15-axp_config->pmu_bat_temp_para16);
	} else if (data <= axp_config->pmu_bat_temp_para14) {
		temp = 60 + (axp_config->pmu_bat_temp_para14-data)*10/
		(axp_config->pmu_bat_temp_para14-axp_config->pmu_bat_temp_para15);
	} else if (data <= axp_config->pmu_bat_temp_para13) {
		temp = 55 + (axp_config->pmu_bat_temp_para13-data)*5/
		(axp_config->pmu_bat_temp_para13-axp_config->pmu_bat_temp_para14);
	} else if (data <= axp_config->pmu_bat_temp_para12) {
		temp = 50 + (axp_config->pmu_bat_temp_para12-data)*5/
		(axp_config->pmu_bat_temp_para12-axp_config->pmu_bat_temp_para13);
	} else if (data <= axp_config->pmu_bat_temp_para11) {
		temp = 45 + (axp_config->pmu_bat_temp_para11-data)*5/
		(axp_config->pmu_bat_temp_para11-axp_config->pmu_bat_temp_para12);
	} else if (data <= axp_config->pmu_bat_temp_para10) {
		temp = 40 + (axp_config->pmu_bat_temp_para10-data)*5/
		(axp_config->pmu_bat_temp_para10-axp_config->pmu_bat_temp_para11);
	} else if (data <= axp_config->pmu_bat_temp_para9) {
		temp = 30 + (axp_config->pmu_bat_temp_para9-data)*10/
		(axp_config->pmu_bat_temp_para9-axp_config->pmu_bat_temp_para10);
	} else if (data <= axp_config->pmu_bat_temp_para8) {
		temp = 20 + (axp_config->pmu_bat_temp_para8-data)*10/
		(axp_config->pmu_bat_temp_para8-axp_config->pmu_bat_temp_para9);
	} else if (data <= axp_config->pmu_bat_temp_para7) {
		temp = 10 + (axp_config->pmu_bat_temp_para7-data)*10/
		(axp_config->pmu_bat_temp_para7-axp_config->pmu_bat_temp_para8);
	} else if (data <= axp_config->pmu_bat_temp_para6) {
		temp = 5 + (axp_config->pmu_bat_temp_para6-data)*5/
		(axp_config->pmu_bat_temp_para6-axp_config->pmu_bat_temp_para7);
	} else if (data <= axp_config->pmu_bat_temp_para5) {
		temp = 0 + (axp_config->pmu_bat_temp_para5-data)*5/
		(axp_config->pmu_bat_temp_para5-axp_config->pmu_bat_temp_para6);
	} else if (data <= axp_config->pmu_bat_temp_para4) {
		temp = -5 + (axp_config->pmu_bat_temp_para4-data)*5/
		(axp_config->pmu_bat_temp_para4-axp_config->pmu_bat_temp_para5);
	} else if (data <= axp_config->pmu_bat_temp_para3) {
		temp = -10 + (axp_config->pmu_bat_temp_para3-data)*5/
		(axp_config->pmu_bat_temp_para3-axp_config->pmu_bat_temp_para4);
	} else if (data <= axp_config->pmu_bat_temp_para2) {
		temp = -15 + (axp_config->pmu_bat_temp_para2-data)*5/
		(axp_config->pmu_bat_temp_para2-axp_config->pmu_bat_temp_para3);
	} else if (data <= axp_config->pmu_bat_temp_para1) {
		temp = -25 + (axp_config->pmu_bat_temp_para1-data)*10/
		(axp_config->pmu_bat_temp_para1-axp_config->pmu_bat_temp_para2);
	} else
		temp = -25;
	return temp;
}

static int axp803_get_bat_temp(struct axp803_charger_ps *di)
{
	unsigned char temp_val[2];
	unsigned short ts_res;
	int bat_temp_mv, bat_temp;
	int ret = 0;

	struct axp_config_info *axp803_config = &di->dts_info;

	ret = regmap_bulk_read(di->regmap, AXP803_VTS_RES, temp_val, 2);
	if (ret < 0)
		return ret;

	ts_res = ((unsigned short) temp_val[0] << 8) | temp_val[1];
	bat_temp_mv = axp_vts_to_mV(ts_res);
	bat_temp = axp_vts_to_temp(bat_temp_mv, axp803_config);

	pr_debug("bat_temp: %d\n", bat_temp);

	return bat_temp;
}

static int axp803_get_bat_status(struct power_supply *psy,
					union power_supply_propval *val)
{
	bool bat_det, bat_charging;
	bool ac_valid, vbus_valid;
	unsigned int rest_vol;
	unsigned int reg_value;
	int ret;

	struct axp803_charger_ps *di = power_supply_get_drvdata(psy);

	ret = regmap_read(di->regmap, AXP803_MODE_CHGSTATUS, &reg_value);
	if (ret)
		return ret;
	bat_det = !!(reg_value & AXP803_CHGSTATUS_BAT_PST_VALID) &&
		!!(reg_value & AXP803_CHGSTATUS_BAT_PRESENT);
	bat_charging = !!(reg_value & AXP803_CHGSTATUS_BAT_CHARGING);

	ret = regmap_read(di->regmap, AXP803_STATUS, &reg_value);
	if (ret)
		return ret;
	ac_valid = !!(reg_value & AXP803_STATUS_AC_USED);
	vbus_valid = !!(reg_value & AXP803_STATUS_VBUS_USED);

	rest_vol = axp803_get_rest_cap(di);

	if (bat_det) {
		if (ac_valid || vbus_valid) {
			if (rest_vol == 100)
				val->intval = POWER_SUPPLY_STATUS_FULL;
			else if (bat_charging)
				val->intval = POWER_SUPPLY_STATUS_CHARGING;
			else
				val->intval = POWER_SUPPLY_STATUS_NOT_CHARGING;
		} else {
			val->intval = POWER_SUPPLY_STATUS_DISCHARGING;
		}
	} else {
		val->intval = POWER_SUPPLY_STATUS_UNKNOWN;
	}

	return 0;
}

static enum power_supply_property axp803_battery_props[] = {
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_STATUS,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
	POWER_SUPPLY_PROP_HEALTH,
	POWER_SUPPLY_PROP_VOLTAGE_NOW,
	POWER_SUPPLY_PROP_CAPACITY,
	POWER_SUPPLY_PROP_TEMP,
};

static enum power_supply_property axp803_ac_props[] = {
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
};

static enum power_supply_property axp803_usb_props[] = {
	POWER_SUPPLY_PROP_MODEL_NAME,
	POWER_SUPPLY_PROP_PRESENT,
	POWER_SUPPLY_PROP_ONLINE,
};

static int axp803_ac_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;
	unsigned int reg_value;
	struct axp803_charger_ps *di = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_MODEL_NAME:
		val->strval = psy->desc->name;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		ret = regmap_read(di->regmap, AXP803_STATUS, &reg_value);
		if (ret)
			return ret;
		val->intval = !!(reg_value & AXP803_STATUS_AC_PRESENT);
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		ret = regmap_read(di->regmap, AXP803_STATUS, &reg_value);
		if (ret)
			return ret;
		val->intval = !!(reg_value & AXP803_STATUS_AC_USED);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int axp803_usb_get_property(struct power_supply *psy,
					enum power_supply_property psp,
					union power_supply_propval *val)
{
	int ret = 0;
	unsigned int reg_value;
	struct axp803_charger_ps *di = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_MODEL_NAME:
		val->strval = psy->desc->name;
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		ret = regmap_read(di->regmap, AXP803_STATUS, &reg_value);
		if (ret)
			return ret;
		val->intval = !!(reg_value & AXP803_STATUS_VBUS_PRESENT);
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		ret = regmap_read(di->regmap, AXP803_STATUS, &reg_value);
		if (ret)
			return ret;
		val->intval = !!(reg_value & AXP803_STATUS_VBUS_USED);
		break;
	default:
		ret = -EINVAL;
		break;
	}

	return ret;
}

static int axp803_battery_get_property(struct power_supply *psy,
				enum power_supply_property psp,
				union power_supply_propval *val)
{
	int ret = 0;
	unsigned int reg_value;
	struct axp803_charger_ps *di = power_supply_get_drvdata(psy);

	switch (psp) {
	case POWER_SUPPLY_PROP_MODEL_NAME:
		val->strval = psy->desc->name;
		break;
	case POWER_SUPPLY_PROP_STATUS:
		ret = axp803_get_bat_status(psy, val);
		break;
	case POWER_SUPPLY_PROP_HEALTH:
		val->intval = axp803_get_bat_health(di);
		break;
	case POWER_SUPPLY_PROP_ONLINE:
		ret = regmap_read(di->regmap, AXP803_STATUS, &reg_value);
		if (ret)
			return ret;
		val->intval = !(reg_value & AXP803_STATUS_BAT_CUR_DIRCT);
		break;
	case POWER_SUPPLY_PROP_PRESENT:
		ret = regmap_read(di->regmap, AXP803_MODE_CHGSTATUS, &reg_value);
		if (ret)
			return ret;
		val->intval = !!(reg_value & AXP803_CHGSTATUS_BAT_PST_VALID) &&
			!!(reg_value & AXP803_CHGSTATUS_BAT_PRESENT);
		break;
	case POWER_SUPPLY_PROP_VOLTAGE_NOW:
		val->intval = axp803_get_vbat(di) * 1000;
		break;
	case POWER_SUPPLY_PROP_CAPACITY:
		val->intval = axp803_get_rest_cap(di);
		break;
	case POWER_SUPPLY_PROP_TEMP:
		val->intval = axp803_get_bat_temp(di);
		break;
	default:
		return -EINVAL;
	}
	return ret;
}

static const struct power_supply_desc axp803_bat_desc = {
	.name = "axp803-battery",
	.type = POWER_SUPPLY_TYPE_BATTERY,
	.get_property = axp803_battery_get_property,
	.properties = axp803_battery_props,
	.num_properties = ARRAY_SIZE(axp803_battery_props),
	.use_for_apm = 1,
};

static const struct power_supply_desc axp803_ac_desc = {
	.name = "axp803-ac",
	.type = POWER_SUPPLY_TYPE_MAINS,
	.get_property = axp803_ac_get_property,
	.properties = axp803_ac_props,
	.num_properties = ARRAY_SIZE(axp803_ac_props),
};

static const struct power_supply_desc axp803_usb_desc = {
	.name = "axp803-usb",
	.type = POWER_SUPPLY_TYPE_USB,
	.get_property = axp803_usb_get_property,
	.properties = axp803_usb_props,
	.num_properties = ARRAY_SIZE(axp803_usb_props),
};

static int axp803_power_supply_register(struct axp803_charger_ps *di)
{
	int ret = 0;
	struct power_supply_config psy_cfg = {
		.of_node = di->dev->of_node,
		.drv_data = di,
	};

	di->bat = devm_power_supply_register(di->dev, &axp803_bat_desc, &psy_cfg);
	if (IS_ERR(di->bat)) {
		pr_err("failed to register battery\n");
		ret = PTR_ERR(di->bat);
		return ret;
	}

	di->usb = devm_power_supply_register(di->dev, &axp803_usb_desc, &psy_cfg);
	if (IS_ERR(di->usb)) {
		pr_err("failed to register usb\n");
		ret = PTR_ERR(di->bat);
		return ret;
	}

	di->ac = devm_power_supply_register(di->dev, &axp803_ac_desc, &psy_cfg);
	if (IS_ERR(di->ac)) {
		pr_err("failed to register battery\n");
		ret = PTR_ERR(di->bat);
		return ret;
	}

	return ret;
}

static int axp803_charger_init(struct axp803_charger_ps *di)
{
	unsigned char ocv_cap[32];
	unsigned int val;
	int cur_coulomb_counter, rdc;
	int i;
	int update_min_times[8] = {30, 60, 120, 164, 0, 5, 10, 20};
	int ocv_cou_adjust_time[4] = {60, 120, 15, 30};

	struct axp_config_info *axp803_config = &di->dts_info;
	struct regmap *map = di->regmap;

	if (axp803_config->pmu_init_chgend_rate == 10)
		val = 0;
	else
		val = 1;
	val <<= 4;
	regmap_update_bits(map, AXP803_CHARGE1, 0x10, val);

	if (axp803_config->pmu_init_chg_pretime < 40)
		axp803_config->pmu_init_chg_pretime = 40;

	if (axp803_config->pmu_init_chg_csttime < 360)
		axp803_config->pmu_init_chg_csttime = 360;

	val = ((((axp803_config->pmu_init_chg_pretime - 40) / 10) << 6)
			| ((axp803_config->pmu_init_chg_csttime - 360) / 120));
	regmap_update_bits(map, AXP803_CHARGE2, 0xc2, val);

	/* adc set */
	val = AXP803_ADC_BATVOL_ENABLE | AXP803_ADC_BATCUR_ENABLE;
	if (axp803_config->pmu_bat_temp_enable != 0)
		val = val | AXP803_ADC_TSVOL_ENABLE;
	regmap_update_bits(map, AXP803_ADC_EN,
			AXP803_ADC_BATVOL_ENABLE
			| AXP803_ADC_BATCUR_ENABLE
			| AXP803_ADC_TSVOL_ENABLE,
			val);

	regmap_read(map, AXP803_ADC_SPEED_SET, &val);
	switch (axp803_config->pmu_init_adc_freq / 100) {
	case 1:
		val &= ~(0x3 << 4);
		break;
	case 2:
		val &= ~(0x3 << 4);
		val |= 0x1 << 4;
		break;
	case 4:
		val &= ~(0x3 << 4);
		val |= 0x2 << 4;
		break;
	case 8:
		val |= 0x3 << 4;
		break;
	default:
		break;
	}

	if (axp803_config->pmu_bat_temp_enable != 0)
		val &= (~(0x1 << 2));
	regmap_write(map, AXP803_ADC_SPEED_SET, val);

	/* bat para */
	regmap_write(map, AXP803_WARNING_LEVEL,
		((axp803_config->pmu_battery_warning_level1 - 5) << 4)
		+ axp803_config->pmu_battery_warning_level2);

	/* set target voltage */
	if (axp803_config->pmu_init_chgvol < 4150) {
		val = 0;
	} else if (axp803_config->pmu_init_chgvol < 4200) {
		val = 1;
	} else if (axp803_config->pmu_init_chgvol < 4350) {
		val = 2;
	} else {
		val = 3;
	}
	val <<= 5;
	regmap_update_bits(map, AXP803_CHARGE1, 0x60, val);

	ocv_cap[0]  = axp803_config->pmu_bat_para1;
	ocv_cap[1]  = axp803_config->pmu_bat_para2;
	ocv_cap[2]  = axp803_config->pmu_bat_para3;
	ocv_cap[3]  = axp803_config->pmu_bat_para4;
	ocv_cap[4]  = axp803_config->pmu_bat_para5;
	ocv_cap[5]  = axp803_config->pmu_bat_para6;
	ocv_cap[6]  = axp803_config->pmu_bat_para7;
	ocv_cap[7]  = axp803_config->pmu_bat_para8;
	ocv_cap[8]  = axp803_config->pmu_bat_para9;
	ocv_cap[9]  = axp803_config->pmu_bat_para10;
	ocv_cap[10] = axp803_config->pmu_bat_para11;
	ocv_cap[11] = axp803_config->pmu_bat_para12;
	ocv_cap[12] = axp803_config->pmu_bat_para13;
	ocv_cap[13] = axp803_config->pmu_bat_para14;
	ocv_cap[14] = axp803_config->pmu_bat_para15;
	ocv_cap[15] = axp803_config->pmu_bat_para16;
	ocv_cap[16] = axp803_config->pmu_bat_para17;
	ocv_cap[17] = axp803_config->pmu_bat_para18;
	ocv_cap[18] = axp803_config->pmu_bat_para19;
	ocv_cap[19] = axp803_config->pmu_bat_para20;
	ocv_cap[20] = axp803_config->pmu_bat_para21;
	ocv_cap[21] = axp803_config->pmu_bat_para22;
	ocv_cap[22] = axp803_config->pmu_bat_para23;
	ocv_cap[23] = axp803_config->pmu_bat_para24;
	ocv_cap[24] = axp803_config->pmu_bat_para25;
	ocv_cap[25] = axp803_config->pmu_bat_para26;
	ocv_cap[26] = axp803_config->pmu_bat_para27;
	ocv_cap[27] = axp803_config->pmu_bat_para28;
	ocv_cap[28] = axp803_config->pmu_bat_para29;
	ocv_cap[29] = axp803_config->pmu_bat_para30;
	ocv_cap[30] = axp803_config->pmu_bat_para31;
	ocv_cap[31] = axp803_config->pmu_bat_para32;
	regmap_bulk_write(map, AXP803_OCVCAP, ocv_cap, 32);

	/* Init CHGLED function */
	if (axp803_config->pmu_chgled_func)
		regmap_update_bits(map, AXP803_OFF_CTL, 0x08, 0x08); /* by charger */
	else
		regmap_update_bits(map, AXP803_OFF_CTL, 0x08, 0x00); /* drive MOTO */

	/* set CHGLED Indication Type */
	if (axp803_config->pmu_chgled_type)
		regmap_update_bits(map, AXP803_CHARGE2, 0x10, 0x10); /* Type B */
	else
		regmap_update_bits(map, AXP803_CHARGE2, 0x10, 0x00); /* Type A */

	/* Init battery capacity correct function */
	if (axp803_config->pmu_batt_cap_correct)
		regmap_update_bits(map, AXP803_COULOMB_CTL, 0x20, 0x20);
	else
		regmap_update_bits(map, AXP803_COULOMB_CTL, 0x20, 0x00);

	/* Init battery regulator enable or not when charge finish */
	if (axp803_config->pmu_chg_end_on_en)
		regmap_update_bits(map, AXP803_CHARGE2, 0x20, 0x20);
	else
		regmap_update_bits(map, AXP803_CHARGE2, 0x20, 0x00);

	if (axp803_config->pmu_batdeten)
		regmap_update_bits(map, AXP803_OFF_CTL, 0x40, 0x40);
	else
		regmap_update_bits(map, AXP803_OFF_CTL, 0x40, 0x00);

	/* RDC initial */
	regmap_read(map, AXP803_RDC0, &val);
	if ((axp803_config->pmu_battery_rdc) && (!(val & 0x40))) {
		rdc = (axp803_config->pmu_battery_rdc * 10000 + 5371) / 10742;
		regmap_write(map, AXP803_RDC0, ((rdc >> 8) & 0x1F)|0x80);
		regmap_write(map, AXP803_RDC1, rdc & 0x00FF);
	}

	regmap_read(map, AXP803_BATCAP0, &val);
	if ((axp803_config->pmu_battery_cap) && (!(val & 0x80))) {
		cur_coulomb_counter = axp803_config->pmu_battery_cap
					* 1000 / 1456;
		regmap_write(map, AXP803_BATCAP0, ((cur_coulomb_counter >> 8) | 0x80));
		regmap_write(map, AXP803_BATCAP1, cur_coulomb_counter & 0x00FF);
	} else if (!axp803_config->pmu_battery_cap) {
		regmap_write(map, AXP803_BATCAP0, 0x00);
		regmap_write(map, AXP803_BATCAP1, 0x00);
	}

	/*
	 * As datasheet decripted:
	 * TS_VOL = reg_value * 16 * 10K * 80ua
	 */
	if (axp803_config->pmu_bat_temp_enable == 1) {
		regmap_write(map, AXP803_VLTF_CHARGE,
				axp803_config->pmu_bat_charge_ltf * 10 / 128);
		regmap_write(map, AXP803_VHTF_CHARGE,
				axp803_config->pmu_bat_charge_htf * 10 / 128);
		regmap_write(map, AXP803_VLTF_WORK,
				axp803_config->pmu_bat_shutdown_ltf * 10 / 128);
		regmap_write(map, AXP803_VHTF_WORK,
				axp803_config->pmu_bat_shutdown_htf * 10 / 128);
	}

	if (axp803_config->pmu_ocv_en == 0) {
		pr_warn("axp803 ocv must be enabled\n");
		axp803_config->pmu_ocv_en = 1;
	}
	if (axp803_config->pmu_init_bc_en == 1) {
		regmap_update_bits(map, AXP803_BC_CTL, 0x01, 0x01);
	} else {
		regmap_update_bits(map, AXP803_BC_CTL, 0x01, 0x00);
	}

	if (axp803_config->pmu_cou_en == 1) {
		/* use ocv and cou */
		regmap_update_bits(map, AXP803_COULOMB_CTL, 0x80, 0x80);
		regmap_update_bits(map, AXP803_COULOMB_CTL, 0x40, 0x40);
	} else if (axp803_config->pmu_cou_en == 0) {
		/* only use ocv */
		regmap_update_bits(map, AXP803_COULOMB_CTL, 0x80, 0x80);
		regmap_update_bits(map, AXP803_COULOMB_CTL, 0x40, 0x00);
	}

	for (i = 0; i < ARRAY_SIZE(update_min_times); i++) {
		if (update_min_times[i] == axp803_config->pmu_update_min_time)
			break;
	}
	regmap_update_bits(map, AXP803_ADJUST_PARA, 0x07, i);
	for (i = 0; i < ARRAY_SIZE(ocv_cou_adjust_time); i++) {
		if (ocv_cou_adjust_time[i] == axp803_config->pmu_ocv_cou_adjust_time)
			break;
	}
	i <<= 6;
	regmap_update_bits(map, AXP803_ADJUST_PARA1, 0xc0, i);

	return 0;
}

static irqreturn_t axp803_irq_handler_thread(int irq, void *data)
{
	struct axp803_charger_ps *di = data;

	pr_debug("%s: enter interrupt %d\n", __func__, irq);

	power_supply_changed(di->bat);

	return IRQ_HANDLED;
}

enum axp803_charger_virqs {
	AXP803_VIRQ_USBIN,
	AXP803_VIRQ_USBRE,
	AXP803_VIRQ_ACIN,
	AXP803_VIRQ_ACRE,
	AXP803_VIRQ_CHAST,
	AXP803_VIRQ_CHAOV,
	AXP803_VIRQ_BATIN,
	AXP803_VIRQ_BATRE,
	AXP803_VIRQ_BATINWORK,
	AXP803_VIRQ_BATOVWORK,
	AXP803_VIRQ_BATINCHG,
	AXP803_VIRQ_BATOVCHG,
	AXP803_VIRQ_LOWN2,
	AXP803_VIRQ_LOWN1,

	AXP803_VIRQ_MAX_VIRQ,
};

static struct axp_interrupts axp_charger_irq[] = {
	[AXP803_VIRQ_USBIN] = {"usb in", axp803_irq_handler_thread},
	[AXP803_VIRQ_USBRE] = {"usb out", axp803_irq_handler_thread},
	[AXP803_VIRQ_ACIN] = {"ac in", axp803_irq_handler_thread},
	[AXP803_VIRQ_ACRE] = {"ac out", axp803_irq_handler_thread},
	[AXP803_VIRQ_CHAST] = {"charging", axp803_irq_handler_thread},
	[AXP803_VIRQ_CHAOV] = {"charge over", axp803_irq_handler_thread},
	[AXP803_VIRQ_BATIN] = {"bat in", axp803_irq_handler_thread},
	[AXP803_VIRQ_BATRE] = {"bat out", axp803_irq_handler_thread},
	[AXP803_VIRQ_BATINWORK] = {"bat untemp work", axp803_irq_handler_thread},
	[AXP803_VIRQ_BATOVWORK] = {"bat ovtemp work", axp803_irq_handler_thread},
	[AXP803_VIRQ_BATINCHG] = {"bat untemp chg", axp803_irq_handler_thread},
	[AXP803_VIRQ_BATOVCHG] = {"bat ovtemp chg", axp803_irq_handler_thread},
	[AXP803_VIRQ_LOWN2] = {"low warning2", axp803_irq_handler_thread},
	[AXP803_VIRQ_LOWN1] = {"low warning1", axp803_irq_handler_thread},
};

static void axp803_charger_monitor(struct work_struct *work)
{
	struct axp803_charger_ps *di =
		container_of(work, typeof(*di), charger_mon.work);

	schedule_delayed_work(&di->charger_mon, msecs_to_jiffies(500));
}


#define AXP_OF_PROP_READ(name, def_value)\
do {\
	if (of_property_read_u32(node, #name, &axp_config->name))\
		axp_config->name = def_value;\
} while (0)

static int axp_charger_dt_parse(struct device_node *node,
			 struct axp_config_info *axp_config)
{
	if (!of_device_is_available(node)) {
		pr_err("%s: failed\n", __func__);
		return -1;
	}

	AXP_OF_PROP_READ(pmu_battery_rdc,              BATRDC);
	AXP_OF_PROP_READ(pmu_battery_cap,                4000);
	AXP_OF_PROP_READ(pmu_batdeten,                      1);
	AXP_OF_PROP_READ(pmu_chg_ic_temp,                   0);
	AXP_OF_PROP_READ(pmu_runtime_chgcur, INTCHGCUR / 1000);
	AXP_OF_PROP_READ(pmu_suspend_chgcur,             1200);
	AXP_OF_PROP_READ(pmu_shutdown_chgcur,            1200);
	AXP_OF_PROP_READ(pmu_init_chgvol,    INTCHGVOL / 1000);
	AXP_OF_PROP_READ(pmu_init_chgend_rate,  INTCHGENDRATE);
	AXP_OF_PROP_READ(pmu_init_chg_enabled,              1);
	AXP_OF_PROP_READ(pmu_init_bc_en,                    0);
	AXP_OF_PROP_READ(pmu_init_adc_freq,        INTADCFREQ);
	AXP_OF_PROP_READ(pmu_init_adcts_freq,     INTADCFREQC);
	AXP_OF_PROP_READ(pmu_init_chg_pretime,  INTCHGPRETIME);
	AXP_OF_PROP_READ(pmu_init_chg_csttime,  INTCHGCSTTIME);
	AXP_OF_PROP_READ(pmu_batt_cap_correct,              1);
	AXP_OF_PROP_READ(pmu_chg_end_on_en,                 0);
	AXP_OF_PROP_READ(ocv_coulumb_100,                   0);
	AXP_OF_PROP_READ(pmu_bat_para1,               OCVREG0);
	AXP_OF_PROP_READ(pmu_bat_para2,               OCVREG1);
	AXP_OF_PROP_READ(pmu_bat_para3,               OCVREG2);
	AXP_OF_PROP_READ(pmu_bat_para4,               OCVREG3);
	AXP_OF_PROP_READ(pmu_bat_para5,               OCVREG4);
	AXP_OF_PROP_READ(pmu_bat_para6,               OCVREG5);
	AXP_OF_PROP_READ(pmu_bat_para7,               OCVREG6);
	AXP_OF_PROP_READ(pmu_bat_para8,               OCVREG7);
	AXP_OF_PROP_READ(pmu_bat_para9,               OCVREG8);
	AXP_OF_PROP_READ(pmu_bat_para10,              OCVREG9);
	AXP_OF_PROP_READ(pmu_bat_para11,              OCVREGA);
	AXP_OF_PROP_READ(pmu_bat_para12,              OCVREGB);
	AXP_OF_PROP_READ(pmu_bat_para13,              OCVREGC);
	AXP_OF_PROP_READ(pmu_bat_para14,              OCVREGD);
	AXP_OF_PROP_READ(pmu_bat_para15,              OCVREGE);
	AXP_OF_PROP_READ(pmu_bat_para16,              OCVREGF);
	AXP_OF_PROP_READ(pmu_bat_para17,             OCVREG10);
	AXP_OF_PROP_READ(pmu_bat_para18,             OCVREG11);
	AXP_OF_PROP_READ(pmu_bat_para19,             OCVREG12);
	AXP_OF_PROP_READ(pmu_bat_para20,             OCVREG13);
	AXP_OF_PROP_READ(pmu_bat_para21,             OCVREG14);
	AXP_OF_PROP_READ(pmu_bat_para22,             OCVREG15);
	AXP_OF_PROP_READ(pmu_bat_para23,             OCVREG16);
	AXP_OF_PROP_READ(pmu_bat_para24,             OCVREG17);
	AXP_OF_PROP_READ(pmu_bat_para25,             OCVREG18);
	AXP_OF_PROP_READ(pmu_bat_para26,             OCVREG19);
	AXP_OF_PROP_READ(pmu_bat_para27,             OCVREG1A);
	AXP_OF_PROP_READ(pmu_bat_para28,             OCVREG1B);
	AXP_OF_PROP_READ(pmu_bat_para29,             OCVREG1C);
	AXP_OF_PROP_READ(pmu_bat_para30,             OCVREG1D);
	AXP_OF_PROP_READ(pmu_bat_para31,             OCVREG1E);
	AXP_OF_PROP_READ(pmu_bat_para32,             OCVREG1F);
	AXP_OF_PROP_READ(pmu_ac_vol,                     4400);
	AXP_OF_PROP_READ(pmu_usbpc_vol,                  4400);
	AXP_OF_PROP_READ(pmu_ac_cur,                        0);
	AXP_OF_PROP_READ(pmu_usbpc_cur,                     0);
	AXP_OF_PROP_READ(pmu_pwroff_vol,                 3300);
	AXP_OF_PROP_READ(pmu_pwron_vol,                  2900);
	AXP_OF_PROP_READ(pmu_battery_warning_level1,       15);
	AXP_OF_PROP_READ(pmu_battery_warning_level2,        0);
	AXP_OF_PROP_READ(pmu_restvol_adjust_time,          30);
	AXP_OF_PROP_READ(pmu_ocv_cou_adjust_time,          60);
	AXP_OF_PROP_READ(pmu_chgled_func,                   0);
	AXP_OF_PROP_READ(pmu_chgled_type,                   0);
	AXP_OF_PROP_READ(pmu_bat_temp_enable,               0);
	AXP_OF_PROP_READ(pmu_bat_charge_ltf,             0xA5);
	AXP_OF_PROP_READ(pmu_bat_charge_htf,             0x1F);
	AXP_OF_PROP_READ(pmu_bat_shutdown_ltf,           0xFC);
	AXP_OF_PROP_READ(pmu_bat_shutdown_htf,           0x16);
	AXP_OF_PROP_READ(pmu_bat_temp_para1,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para2,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para3,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para4,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para5,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para6,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para7,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para8,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para9,                0);
	AXP_OF_PROP_READ(pmu_bat_temp_para10,               0);
	AXP_OF_PROP_READ(pmu_bat_temp_para11,               0);
	AXP_OF_PROP_READ(pmu_bat_temp_para12,               0);
	AXP_OF_PROP_READ(pmu_bat_temp_para13,               0);
	AXP_OF_PROP_READ(pmu_bat_temp_para14,               0);
	AXP_OF_PROP_READ(pmu_bat_temp_para15,               0);
	AXP_OF_PROP_READ(pmu_bat_temp_para16,               0);
	AXP_OF_PROP_READ(pmu_bat_unused,                    0);
	AXP_OF_PROP_READ(power_start,                       0);
	AXP_OF_PROP_READ(pmu_ocv_en,                        1);
	AXP_OF_PROP_READ(pmu_cou_en,                        1);
	AXP_OF_PROP_READ(pmu_update_min_time,   UPDATEMINTIME);

	axp_config->wakeup_usb_in =
		of_property_read_bool(node, "wakeup_usb_in");
	axp_config->wakeup_usb_out =
		of_property_read_bool(node, "wakeup_usb_out");
	axp_config->wakeup_bat_in =
		of_property_read_bool(node, "wakeup_bat_in");
	axp_config->wakeup_bat_out =
		of_property_read_bool(node, "wakeup_bat_out");
	axp_config->wakeup_bat_charging =
		of_property_read_bool(node, "wakeup_bat_charging");
	axp_config->wakeup_bat_charge_over =
		of_property_read_bool(node, "wakeup_bat_charge_over");
	axp_config->wakeup_low_warning1 =
		of_property_read_bool(node, "wakeup_low_warning1");
	axp_config->wakeup_low_warning2 =
		of_property_read_bool(node, "wakeup_low_warning2");
	axp_config->wakeup_bat_untemp_work =
		of_property_read_bool(node, "wakeup_bat_untemp_work");
	axp_config->wakeup_bat_ovtemp_work =
		of_property_read_bool(node, "wakeup_bat_ovtemp_work");
	axp_config->wakeup_untemp_chg =
		of_property_read_bool(node, "wakeup_bat_untemp_chg");
	axp_config->wakeup_ovtemp_chg =
		of_property_read_bool(node, "wakeup_bat_ovtemp_chg");

	return 0;
}

static int axp803_charger_dt_parse(struct axp803_charger_ps *di)
{
	int ret;
	struct axp_config_info *cfg = &di->dts_info;
	struct device_node *node = di->dev->of_node;

	if (!of_device_is_available(node)) {
		pr_err("%s: failed\n", __func__);
		return -1;
	}

	ret = axp_charger_dt_parse(node, cfg);
	if (ret) {
		pr_info("can not parse device tree err\n");
		return ret;
	}

	return 0;
}

static int axp803_charger_probe(struct platform_device *pdev)
{
	struct axp20x_dev *axp_dev = dev_get_drvdata(pdev->dev.parent);
	struct axp803_charger_ps *di;
	int i, irq;
	int ret = 0;

	if (!axp_dev->irq) {
		pr_err("can not register axp803-charger without irq\n");
		return -EINVAL;
	}

	di = devm_kzalloc(&pdev->dev, sizeof(*di), GFP_KERNEL);
	if (!di) {
		pr_err("axp803_charger_ps alloc failed\n");
		ret = -ENOMEM;
		goto err;
	}

	di->name = "axp803-charger";
	di->dev = &pdev->dev;
	di->regmap = axp_dev->regmap;

	ret = axp803_charger_dt_parse(di);
	if (ret) {
		pr_err("%s parse device tree err\n", __func__);
		return -EINVAL;
	}

	axp803_charger = di;

	ret = axp803_charger_init(axp803_charger);
	if (ret < 0) {
		pr_err("axp210x init chip fail!\n");
		ret = -ENODEV;
		goto err;
	}

	ret = axp803_power_supply_register(axp803_charger);
	if (ret < 0) {
		pr_err("axp210x register battery dev fail!\n");
		goto err;
	}

	for (i = 0; i < ARRAY_SIZE(axp_charger_irq); i++) {
		irq = platform_get_irq_byname(pdev, axp_charger_irq[i].name);
		if (irq < 0)
			continue;

		irq = regmap_irq_get_virq(axp_dev->regmap_irqc, irq);
		if (irq < 0) {
			dev_err(&pdev->dev, "can not get irq\n");
			return irq;
		}
		/* we use this variable to suspend irq */
		axp_charger_irq[i].irq = irq;
		ret = devm_request_any_context_irq(&pdev->dev, irq,
						   axp_charger_irq[i].isr, 0,
						   axp_charger_irq[i].name, di);
		if (ret < 0) {
			dev_err(&pdev->dev, "failed to request %s IRQ %d: %d\n",
				axp_charger_irq[i].name, irq, ret);
			return ret;
		} else {
			ret = 0;
		}

		dev_dbg(&pdev->dev, "Requested %s IRQ %d: %d\n",
			axp_charger_irq[i].name, irq, ret);
	}

	platform_set_drvdata(pdev, di);

	INIT_DELAYED_WORK(&di->charger_mon, axp803_charger_monitor);
	schedule_delayed_work(&di->charger_mon, msecs_to_jiffies(500));

	return ret;

err:
	pr_err("%s,probe fail, ret = %d\n", __func__, ret);

	return ret;
}

static int axp803_charger_remove(struct platform_device *pdev)
{
	struct axp803_charger_ps *di = platform_get_drvdata(pdev);

	if (di->bat)
		power_supply_unregister(di->bat);

	if (di->ac)
		power_supply_unregister(di->ac);

	if (di->usb)
		power_supply_unregister(di->usb);

	axp803_charger = NULL;

	return 0;
}

static inline void axp_irq_set(unsigned int irq, bool enable)
{
	if (enable)
		enable_irq(irq);
	else
		disable_irq(irq);
}

static void axp803_virq_dts_set(struct axp803_charger_ps *di, bool enable)
{
	struct axp_config_info *dts_info = &di->dts_info;

	if (!dts_info->wakeup_usb_in)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_USBIN].irq,
				enable);
	if (!dts_info->wakeup_usb_out)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_USBRE].irq,
				enable);
	if (!dts_info->wakeup_bat_in)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_BATIN].irq,
				enable);
	if (!dts_info->wakeup_bat_out)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_BATRE].irq,
				enable);
	if (!dts_info->wakeup_bat_charging)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_CHAST].irq,
				enable);
	if (!dts_info->wakeup_bat_charge_over)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_CHAOV].irq,
				enable);
	if (!dts_info->wakeup_low_warning1)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_LOWN1].irq,
				enable);
	if (!dts_info->wakeup_low_warning2)
		axp_irq_set(axp_charger_irq[AXP803_VIRQ_LOWN2].irq,
				enable);
	if (!dts_info->wakeup_bat_untemp_work)
		axp_irq_set(
			axp_charger_irq[AXP803_VIRQ_BATINWORK].irq,
			enable);
	if (!dts_info->wakeup_bat_ovtemp_work)
		axp_irq_set(
			axp_charger_irq[AXP803_VIRQ_BATOVWORK].irq,
			enable);
	if (!dts_info->wakeup_untemp_chg)
		axp_irq_set(
			axp_charger_irq[AXP803_VIRQ_BATINCHG].irq,
			enable);
	if (!dts_info->wakeup_ovtemp_chg)
		axp_irq_set(
			axp_charger_irq[AXP803_VIRQ_BATOVCHG].irq,
			enable);
}

static void axp803_shutdown(struct platform_device *pdev)
{
	struct axp803_charger_ps *di = platform_get_drvdata(pdev);

	axp803_set_chg_cur(di, di->dts_info.pmu_shutdown_chgcur);
}

static int axp803_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct axp803_charger_ps *di = platform_get_drvdata(pdev);

	axp803_set_chg_cur(di, di->dts_info.pmu_suspend_chgcur);

	axp803_virq_dts_set(di, false);

	return 0;
}

static int axp803_resume(struct platform_device *pdev)
{
	struct axp803_charger_ps *di = platform_get_drvdata(pdev);

	axp803_set_chg_cur(di, di->dts_info.pmu_runtime_chgcur);

	axp803_virq_dts_set(di, true);

	return 0;
}

static const struct platform_device_id axp803_charger_dt_ids[] = {
	{ .name = "axp803-power-supply", },
	{},
};
MODULE_DEVICE_TABLE(of, axp803_charger_dt_ids);

static struct platform_driver axp803_charger_driver = {
	.driver = {
		.name = "axp803-power-supply",
	},
	.probe = axp803_charger_probe,
	.remove = axp803_charger_remove,
	.id_table = axp803_charger_dt_ids,
	.shutdown = axp803_shutdown,
	.suspend = axp803_suspend,
	.resume = axp803_resume,
};

module_platform_driver(axp803_charger_driver);

MODULE_AUTHOR("wangxiaoliang <wangxiaoliang@x-powers.com>");
MODULE_DESCRIPTION("axp803 charger driver");
MODULE_LICENSE("GPL");
