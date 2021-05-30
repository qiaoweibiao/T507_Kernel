/* SPDX-License-Identifier: GPL-2.0 */
/**
 * rawnand_chip.c
 *
 * Copyright (C) 2019 Allwinner.
 *
 * 2019.9.11 cuizhikui<cuizhikui@allwinnertech.com>
 */


#include "rawnand.h"
#include "rawnand_chip.h"
#include "rawnand_debug.h"
#include "rawnand_ids.h"

struct itf_ops_t rawnand_itf_ops;

int set_nand_onfi_ddr2_confiure_default(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	u8 pr[4]; //read back value

	addr = 0x02; //feature address 02h, NV-DDR2 Configuration

	p[0] = 0x0;
	p[0] |= (nci->nfc_init_ddr_info->en_ext_verf & 0x1);
	p[0] |= ((nci->nfc_init_ddr_info->en_dqs_c & 0x1) << 1);
	p[0] |= ((nci->nfc_init_ddr_info->en_re_c & 0x1) << 2);
	p[0] |= ((nci->nfc_init_ddr_info->odt & 0x7) << 4);
	p[1] = 0x0;
	p[1] |= (nci->nfc_init_ddr_info->dout_re_warmup_cycle & 0x7);
	p[1] |= ((nci->nfc_init_ddr_info->din_dqs_warmup_cycle & 0x7) << 4);
	p[2] = 0x0;
	p[3] = 0x0;

	nand_set_feature(nci, &addr, p);
	nand_get_feature(nci, &addr, pr);
	if ((pr[0] != p[0]) || (pr[1] != p[1])) {
		RAWNAND_ERR("set feature(addr 0x02) NV-DDR2 Configuration failed!\n");
		return ERR_NO_113;
	}

	return 0;
}

int set_nand_onfi_driver_strength_default(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	u8 pr[4]; //read back value
	u32 drive;

	drive = nci->nfc_init_ddr_info->output_driver_strength;
	if (drive > 3) {
		RAWNAND_ERR("wrong onfi nand flash driver strength value: %d. keep default value\n", drive);
		return 0;
	}

	addr = 0x10; //feature address 10h, Programmable output driver strength
	p[0] = 0x0;
	p[0] |= ((nci->nfc_init_ddr_info->output_driver_strength & 0x3) << 0);
	p[1] = 0x0;
	p[2] = 0x0;
	p[3] = 0x0;
	nand_set_feature(nci, &addr, p);
	nand_get_feature(nci, &addr, pr);
	if (pr[0] != p[0]) {
		RAWNAND_ERR("set feature(addr 0x10) Programmable output driver strength failed!\n");
		return ERR_NO_112;
	}

	return 0;
}

int micron_onfi_rb_strength_setting(struct nand_chip_info *nci)
{
	/*int ret = 0;*/
	u8 addr;
	u8 p[4];
	u8 pr[4]; //read back value
	/*micron nand flash*/
	if (nci->nfc_init_ddr_info->output_driver_strength <= 3) {
		addr = 0x80; //feature address 80h, Programmable output driver strength
		p[0] = 0x0;
		p[0] |= ((nci->nfc_init_ddr_info->output_driver_strength & 0x3) << 0);
		p[1] = 0x0;
		p[2] = 0x0;
		p[3] = 0x0;
		nand_set_feature(nci, &addr, p);
		nand_get_feature(nci, &addr, pr);
		if (pr[0] != p[0]) {
			RAWNAND_ERR("set feature(addr 0x80) Programmable output driver strength failed!\n");
			return ERR_NO_111;
		}
	}

	if (nci->nfc_init_ddr_info->rb_pull_down_strength <= 3) {
		addr = 0x81; //feature address 81h, Programmable R/B# Pull-Down strength
		p[0] = 0x0;
		p[0] |= ((nci->nfc_init_ddr_info->rb_pull_down_strength & 0x3) << 0);
		p[1] = 0x0;
		p[2] = 0x0;
		p[3] = 0x0;
		nand_set_feature(nci, &addr, p);
		nand_get_feature(nci, &addr, pr);
		if (pr[0] != p[0]) {
			RAWNAND_ERR("set feature(addr 0x80) Programmable R/B# Pull-Down strength failed!\n");
			return ERR_NO_110;
		}
	}
	return NAND_OP_TRUE;
}

int set_nand_onfi_rb_strength_default(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	u8 pr[4]; //read back value

	if (nci->id[0] == 0x2c) {
		/*micron nand flash*/
		if (nci->nfc_init_ddr_info->output_driver_strength <= 3) {
			addr = 0x80; //feature address 80h, Programmable output driver strength
			p[0] = 0x0;
			p[0] |= ((nci->nfc_init_ddr_info->output_driver_strength & 0x3) << 0);
			p[1] = 0x0;
			p[2] = 0x0;
			p[3] = 0x0;
			nand_set_feature(nci, &addr, p);
			nand_get_feature(nci, &addr, pr);
			if (pr[0] != p[0]) {
				RAWNAND_ERR("set feature(addr 0x80) Programmable output driver strength failed!\n");
				return ERR_NO_111;
			}
		}

		if (nci->nfc_init_ddr_info->rb_pull_down_strength <= 3) {
			addr = 0x81; //feature address 81h, Programmable R/B# Pull-Down strength
			p[0] = 0x0;
			p[0] |= ((nci->nfc_init_ddr_info->rb_pull_down_strength & 0x3) << 0);
			p[1] = 0x0;
			p[2] = 0x0;
			p[3] = 0x0;
			nand_set_feature(nci, &addr, p);
			nand_get_feature(nci, &addr, pr);
			if (pr[0] != p[0]) {
				RAWNAND_ERR("set feature(addr 0x80) Programmable R/B# Pull-Down strength failed!\n");
				return ERR_NO_110;
			}
		}
	}

	return 0;
}

int set_nand_onfi_timing_mode_default(struct nand_chip_info *nci, nand_if_type if_type, u32 timing_mode)
{
	u8 addr;
	u8 p[4];

	if (!SUPPORT_CHANGE_ONFI_TIMING_MODE) {
		RAWNAND_ERR("don't support change onfi timing mode. if_type: %d\n", if_type);
		return ERR_NO_71;
	}

	if ((if_type != SDR) && (if_type != ONFI_DDR) && (if_type != ONFI_DDR2)) {
		RAWNAND_ERR("wrong onfi interface type: %d\n", if_type);
		return ERR_NO_70;
	}

	if ((if_type == SDR) && (timing_mode > 5)) {
		RAWNAND_ERR("wrong onfi timing mode(%d) in interface type(%d)\n", if_type, timing_mode);
		return ERR_NO_69;
	}
	if ((if_type == ONFI_DDR) && (timing_mode > 5)) {
		RAWNAND_ERR("wrong onfi timing mode(%d) in interface type(%d)\n", if_type, timing_mode);
		return ERR_NO_68;
	}
	if ((if_type == ONFI_DDR2) && (timing_mode > 7)) {
		RAWNAND_ERR("wrong onfi timing mode(%d) in interface type(%d)\n", if_type, timing_mode);
		return ERR_NO_67;
	}

	addr = 0x01; //feature address 01h, Timing Mode
	p[0] = 0;
	if (if_type == ONFI_DDR)
		p[0] = (0x1U << 4) | (timing_mode & 0xf);
	else if (if_type == ONFI_DDR2)
		p[0] = (0x2U << 4) | (timing_mode & 0xf);
	else
		p[0] = (timing_mode & 0xf);

	p[1] = 0x0;
	p[2] = 0x0;
	p[3] = 0x0;
	nand_set_feature(nci, &addr, p);
	//aw_delay(0x100); //max tITC is 1us

	return 0;
}

s32 set_nand_toggle_specific_setting_default(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	u8 pr[4]; //read back value

	addr = 0x02; //feature address 02h, Toggle 2.0-specific Configuration
	p[0] = 0x0;
	p[0] |= (nci->nfc_init_ddr_info->en_ext_verf & 0x1);
	p[0] |= ((nci->nfc_init_ddr_info->en_dqs_c & 0x1) << 1);
	p[0] |= ((nci->nfc_init_ddr_info->en_re_c & 0x1) << 2);
	p[0] |= ((nci->nfc_init_ddr_info->odt & 0x7) << 4);
	p[1] = 0x0;
	p[1] |= ((nci->nfc_init_ddr_info->dout_re_warmup_cycle & 0x7) << 0);
	p[1] |= ((nci->nfc_init_ddr_info->din_dqs_warmup_cycle & 0x7) << 4);
	p[2] = 0x0;
	p[3] = 0x0;
	nand_set_feature(nci, &addr, p);
	nand_get_feature(nci, &addr, pr);
	if ((pr[0] != p[0]) || (pr[1] != p[1])) {
		RAWNAND_ERR("set feature(addr 0x02) Toggle 2.0-specific Configuration failed!\n");
		return ERR_NO_66;
	}

	return 0;
}

int set_nand_toggle_driver_strength_default(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	u8 pr[4]; //read back value
	u32 drive;

	drive = nci->nfc_init_ddr_info->output_driver_strength;
	if ((drive != 2) && (drive != 4) && (drive != 6)) {
		RAWNAND_INFO("reserved toggle nand flash driver strength value: %d. keep default value.\n", drive);
		return 0;
	}

	addr = 0x10; //feature address 10h, Programmable output driver strength
	p[0] = 0x0;
	p[0] |= ((nci->nfc_init_ddr_info->output_driver_strength & 0x7) << 0); //2, 4, 6
	p[1] = 0x0;
	p[2] = 0x0;
	p[3] = 0x0;
	nand_set_feature(nci, &addr, p);
	nand_get_feature(nci, &addr, pr);
	if (pr[0] != p[0]) {
		RAWNAND_ERR("set feature(addr 0x10) Programmable output driver strength failed!\n");
		return ERR_NO_65;
	}

	return 0;
}

int set_nand_toggle_vendor_specific_setting_default(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	//u8 pr[4]; //read back value

	if ((nci->id[0] == 0x45) || (nci->id[0] == 0x98)) {
		/*sandisk/toshiba nand flash*/
		addr = 0x80; //Sandisk: This address (80h) is a vendor-specific setting used to turn on or turn off Toggle Mode
		p[0] = 0x0;
		if ((nci->interface_type == TOG_DDR) || (nci->interface_type == TOG_DDR2))
			p[0] = 0x0; //enable toggle mode
		else
			p[0] = 0x1; //disable toggle mode
		p[1] = 0x0;
		p[2] = 0x0;
		p[3] = 0x0;
		nand_set_feature(nci, &addr, p);
#if 0
		nfc_get_feature(addr, pr);
		if (pr[0] != p[0]) {
			RAWNAND_ERR("set feature(addr 0x80) Programmable output driver strength failed!\n");
			return ERR_NO_64;
		}
#endif
	}

	return 0;
}

int sandisk_toggle_vendor_specific_setting(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	addr = 0x80; //Sandisk: This address (80h) is a vendor-specific setting used to turn on or turn off Toggle Mode
	p[0] = 0x0;
	if ((nci->interface_type == TOG_DDR) || (nci->interface_type == TOG_DDR2))
		p[0] = 0x0; //enable toggle mode
	else
		p[0] = 0x1; //disable toggle mode
	p[1] = 0x0;
	p[2] = 0x0;
	p[3] = 0x0;
	nand_set_feature(nci, &addr, p);

	return NAND_OP_TRUE;
}

int toshiba_toggle_vendor_specific_setting(struct nand_chip_info *nci)
{
	u8 addr;
	u8 p[4];
	addr = 0x80; //toshiba: This address (80h) is a vendor-specific setting used to turn on or turn off Toggle Mode
	p[0] = 0x0;
	if ((nci->interface_type == TOG_DDR) || (nci->interface_type == TOG_DDR2))
		p[0] = 0x0; //to toggle ddr1.0
	else
		p[0] = 0x1; //to sdr
	p[1] = 0x0;
	p[2] = 0x0;
	p[3] = 0x0;
	nand_set_feature(nci, &addr, p);

	return NAND_OP_TRUE;
}

int rawnand_async_to_onfi_ddr_or_ddr2_set(struct nand_chip_info *nci, nand_if_type ddr_type)
{
	int ret = 0;
	/* Async => ONFI DDR/DDR2 */
	RAWNAND_DBG("mode 1 : Async => ONFI DDR/DDR2\n");
	if ((nci->itf_cfg.onfi_cfg.support_ddr2_specific_cfg) && (ddr_type == ONFI_DDR2)) {
		if (rawnand_itf_ops.onfi.ddr2_cfg) {
			ret = rawnand_itf_ops.onfi.ddr2_cfg(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand onfi async set to ddr2 failed,while setup nand onfi ddr2 para!\n", __func__);
				return ret;
			}
		}
	}

	if (nci->itf_cfg.onfi_cfg.support_io_driver_strength) {
		if (rawnand_itf_ops.onfi.driver_strength) {
			ret = rawnand_itf_ops.onfi.driver_strength(nci);
			/*ret = _setup_nand_onfi_driver_strength(nci);*/
			if (ret) {
				RAWNAND_ERR("rawnand onfi async set to %s failed,while setup io driver strength!\n",
					    (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	if (nci->itf_cfg.onfi_cfg.support_rb_pull_down_strength) {
		if (rawnand_itf_ops.onfi.rb_strength) {
			/*ret = _setup_nand_onfi_vendor_specific_feature(nci);*/
			ret = rawnand_itf_ops.onfi.rb_strength(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand onfi async set to %s failed,while setup nand vendor_specific feature!\n", __func__,
					    (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	if (nci->itf_cfg.onfi_cfg.support_change_onfi_timing_mode) {
		if (rawnand_itf_ops.onfi.timing_mode) {
			/*ret = _change_nand_onfi_timing_mode(nci, ddr_type, nci->timing_mode);*/
			ret = rawnand_itf_ops.onfi.timing_mode(nci, ddr_type, nci->timing_mode);
			if (ret) {
				RAWNAND_ERR("%s rawnand onfi async set to %s failed,while setup onfi timing mode!\n", __func__,
					    (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
				RAWNAND_ERR("nand flash switch to nv-ddr or nv-ddr2 failed!\n");
				return ret;
			}
		}
	}

	return NAND_OP_TRUE;
}

int rawnand_onfi_ddr_or_ddr2_to_async_set(struct nand_chip_info *nci, nand_if_type ddr_type,
					  nand_if_type pre_ddr_type)
{
	int ret = 0;

	/* ONFI DDR/DDR2 => Async */
	RAWNAND_DBG("mode 2 : ONFI DDR/DDR2 => Async\n");
	if ((nci->itf_cfg.onfi_cfg.support_ddr2_specific_cfg) && (pre_ddr_type == ONFI_DDR2)) {
		if (rawnand_itf_ops.onfi.ddr2_cfg) {
			/*ret = _setup_nand_onfi_ddr2_para(nci);*/
			ret = rawnand_itf_ops.onfi.ddr2_cfg(nci);
			if (ret != 0) {
				RAWNAND_ERR("%s rawnand onfi ddr2 set to async failed,while setup nand onfi ddr2 para!\n", __func__);
				return ret;
			}
		}
	}

	if (nci->itf_cfg.onfi_cfg.support_io_driver_strength) {
		if (rawnand_itf_ops.onfi.driver_strength) {
			/*ret = _setup_nand_onfi_driver_strength(nci);*/
			ret = rawnand_itf_ops.onfi.driver_strength(nci);
			if (ret != 0) {
				RAWNAND_ERR("%s rawnand onfi async set to %s failed,while setup io driver strength!\n", __func__, (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	if (nci->itf_cfg.onfi_cfg.support_rb_pull_down_strength) {
		if (rawnand_itf_ops.onfi.rb_strength) {
			/*ret = _setup_nand_onfi_vendor_specific_feature(nci);*/
			ret = rawnand_itf_ops.onfi.rb_strength(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand onfi async set to %s failed,while setup vendor specific cfg!\n", __func__, (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	/* use async reset to aysnc interface */
	nand_reset_chip(nci);

	/* change to proper timing mode in async interface */
	if (nci->itf_cfg.onfi_cfg.support_change_onfi_timing_mode) {
		if (rawnand_itf_ops.onfi.timing_mode) {
			/*ret = _change_nand_onfi_timing_mode(nci, ddr_type, nci->timing_mode);*/
			ret = rawnand_itf_ops.onfi.timing_mode(nci, ddr_type, nci->timing_mode);
			if (ret) {
				RAWNAND_ERR("%s rawnand onfi async set to %s failed,while set onfi timing mode!\n", __func__, (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
				RAWNAND_ERR("nand flash change timing mode at async interface failed!\n");
				return ret;
			}
		}
	}

	return NAND_OP_TRUE;
}

int rawnand_async_to_toggle_ddr_or_ddr2_set(struct nand_chip_info *nci, nand_if_type ddr_type)
{
	int ret = 0;
	/* Async => Toggle DDR/DDR2 */
	RAWNAND_DBG("mode 3 : Async => Toggle %s\n", (ddr_type == TOG_DDR2) ? "DDR2" : "DDR");
	nand_reset_chip(nci);
	if (nci->itf_cfg.toggle_cfg.support_specific_setting) {
		if (ddr_type == TOG_DDR2) {
			if (rawnand_itf_ops.toggle.specific_setting) {
				/*ret = _setup_nand_toggle_ddr2_para(nci);*/
				ret = rawnand_itf_ops.toggle.specific_setting(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand async set to toggle ddr2 failed,while specific setting!\n", __func__);
					return ret;
				}
			}
		}
	}

	if (nci->itf_cfg.toggle_cfg.support_io_driver_strength_setting) {
		if (rawnand_itf_ops.toggle.driver_strength) {
			/*ret = _setup_nand_toggle_driver_strength(nci);*/
			ret = rawnand_itf_ops.toggle.driver_strength(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand async set to toggle %s failed,while toggle driver strength!\n", __func__,
					(ddr_type == TOG_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	if (nci->itf_cfg.toggle_cfg.support_vendor_specific_setting) {
		if (rawnand_itf_ops.toggle.vendor_specific_setting) {
			/*ret = _setup_nand_toggle_vendor_specific_feature(nci);*/
			ret = rawnand_itf_ops.toggle.vendor_specific_setting(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand async set to toggle %s failed,while toggle vendor specific setting!\n", __func__,
					(ddr_type == TOG_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	return NAND_OP_TRUE;
}
int rawnand_toggle_ddr_or_ddr2_to_async_set(struct nand_chip_info *nci, nand_if_type ddr_type,
					    nand_if_type pre_ddr_type)
{
	int ret = 0;

	/* Toggle DDR/DDR2 => Async */
	RAWNAND_DBG("mode 4 : Toggle DDR/DDR2 => Async\n");
	if ((nci->itf_cfg.toggle_cfg.support_specific_setting) && (pre_ddr_type == TOG_DDR2)) {
		// clear ddr2 parameter
		if (rawnand_itf_ops.toggle.specific_setting) {
			/*ret = _setup_nand_toggle_ddr2_para(nci);*/
			ret = rawnand_itf_ops.toggle.specific_setting(nci);
			if (ret != 0) {
				RAWNAND_ERR("%s rawnand async set to toggle ddr2 failed,while specific setting!\n", __func__);
				return ret;
			}
		}
	}

	if (nci->itf_cfg.toggle_cfg.support_io_driver_strength_setting) {
		if (rawnand_itf_ops.toggle.driver_strength) {
			/*ret = _setup_nand_toggle_driver_strength(nci);*/
			ret = rawnand_itf_ops.toggle.driver_strength(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand async set to toggle %s failed,while toggle driver strength!\n", __func__,
					    (ddr_type == TOG_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	if (nci->itf_cfg.toggle_cfg.support_vendor_specific_setting) {
		if (rawnand_itf_ops.toggle.vendor_specific_setting) {
			/*ret = _setup_nand_toggle_vendor_specific_feature(nci);*/
			ret = rawnand_itf_ops.toggle.vendor_specific_setting(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand async set to toggle %s failed,while toggle vendor specific setting!\n",
					    __func__, (ddr_type == TOG_DDR2) ? "ddr2" : "ddr");
				return ret;
			}
		}
	}

	return NAND_OP_TRUE;
}

int rawnand_toggle_ddr2_to_toggle_ddr_set(struct nand_chip_info *nci)
{
	int ret = 0;
	/* Toggle DDR2 <=> Toggle DDR */
	RAWNAND_DBG("mode 5 : Toggle DDR2 <=> Toggle DDR\n");
	if (nci->itf_cfg.toggle_cfg.support_specific_setting) {
		if (rawnand_itf_ops.toggle.specific_setting) {
			/*ret = _setup_nand_toggle_ddr2_para(nci);*/
			ret = rawnand_itf_ops.toggle.specific_setting(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand toggle ddr2 to toggle ddr failed,while specific setting!\n", __func__);
				return ret;
			}
		}
	}

	if (nci->itf_cfg.toggle_cfg.support_io_driver_strength_setting) {
		if (rawnand_itf_ops.toggle.driver_strength) {
			/*ret = _setup_nand_toggle_driver_strength(nci);*/
			ret = rawnand_itf_ops.toggle.driver_strength(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand toggle ddr to toggle ddr failed,while set io driver strenth!\n", __func__);
				return ret;
			}
		}
	}

	if (nci->itf_cfg.toggle_cfg.support_vendor_specific_setting) {
		if (rawnand_itf_ops.toggle.vendor_specific_setting) {
			/*ret = _setup_nand_toggle_vendor_specific_feature(nci);*/
			ret = rawnand_itf_ops.toggle.vendor_specific_setting(nci);
			if (ret) {
				RAWNAND_ERR("%s rawnand toggle ddr2 to toggle ddr failed,while set vendor specific feature!\n", __func__);
				return ret;
			}
		}
	}

	return NAND_OP_TRUE;
}
int rawnand_itf_unchanged_set(struct nand_chip_info *nci, nand_if_type ddr_type,
			      nand_if_type pre_ddr_type)
{
	int ret = 0;
	RAWNAND_DBG("mode 6\n");
	if (ddr_type == SDR) {
		if (nci->itf_cfg.onfi_cfg.support_change_onfi_timing_mode) {
			if (rawnand_itf_ops.onfi.timing_mode) {
				/*ret = _change_nand_onfi_timing_mode(nci, ddr_type, nci->timing_mode);*/
				ret = rawnand_itf_ops.onfi.timing_mode(nci, ddr_type, nci->timing_mode);
				if (ret) {
					RAWNAND_ERR("%s nand flash change timing mode at async interface failed!continue...\n", __func__);
					return ret;
				}
			}
		}
	} else if ((ddr_type == ONFI_DDR) || (ddr_type == ONFI_DDR2)) {
		if ((nci->itf_cfg.onfi_cfg.support_ddr2_specific_cfg) && (ddr_type == ONFI_DDR2)) {
			if (rawnand_itf_ops.onfi.ddr2_cfg) {
				/*ret = _setup_nand_onfi_ddr2_para(nci);*/
				ret = rawnand_itf_ops.onfi.ddr2_cfg(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand set onfi ddr2 fail,while onfi ddr2 para!\n", __func__);
					return ret;
				}
			}
		}

		if (nci->itf_cfg.onfi_cfg.support_io_driver_strength) {
			if (rawnand_itf_ops.onfi.driver_strength) {
				/*ret = _setup_nand_onfi_driver_strength(nci);*/
				ret = rawnand_itf_ops.onfi.driver_strength(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand set onfi %s failed,while onfi driver strength!\n", __func__,
						    (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
					return ret;
				}
			}
		}

		if (nci->itf_cfg.onfi_cfg.support_rb_pull_down_strength) {
			if (rawnand_itf_ops.onfi.rb_strength) {
				/*ret = _setup_nand_onfi_vendor_specific_feature(nci);*/
				ret = rawnand_itf_ops.onfi.rb_strength(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand set onfi %s failed,while onfi vendor specific feature!\n", __func__,
						    (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
					return ret;
				}
			}
		}

		if (nci->itf_cfg.onfi_cfg.support_change_onfi_timing_mode) {
			if (rawnand_itf_ops.onfi.timing_mode) {
				/*ret = _change_nand_onfi_timing_mode(nci, ddr_type, nci->timing_mode);*/
				ret = rawnand_itf_ops.onfi.timing_mode(nci, ddr_type, nci->timing_mode);
				if (ret) {
					RAWNAND_ERR("%s rawnand set onfi %s failed,while onfi timing mode!\n", __func__,
						    (ddr_type == ONFI_DDR2) ? "ddr2" : "ddr");
					return ret;
				}
			}
		}
	} else if ((ddr_type == TOG_DDR) || (ddr_type == TOG_DDR2)) {
		if ((nci->itf_cfg.toggle_cfg.support_specific_setting) && (ddr_type == TOG_DDR2)) {
			if (rawnand_itf_ops.toggle.specific_setting) {
				/*ret = _setup_nand_toggle_ddr2_para(nci);*/
				ret = rawnand_itf_ops.toggle.specific_setting(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand toggle ddr2 to toggle ddr failed,while specific setting!\n", __func__);
					return ret;
				}
			}
		}

		if (nci->itf_cfg.toggle_cfg.support_io_driver_strength_setting) {
			if (rawnand_itf_ops.toggle.driver_strength) {
				/*ret = _setup_nand_toggle_driver_strength(nci);*/
				ret = rawnand_itf_ops.toggle.driver_strength(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand set to toggle %s failed,while toggle driver strength!\n", __func__,
						    (ddr_type == TOG_DDR2) ? "ddr2" : "ddr");
					return ret;
				}
			}
		}

		if (nci->itf_cfg.toggle_cfg.support_vendor_specific_setting) {
			if (rawnand_itf_ops.toggle.vendor_specific_setting) {
				/*ret = _setup_nand_toggle_vendor_specific_feature(nci);*/
				ret = rawnand_itf_ops.toggle.vendor_specific_setting(nci);
				if (ret) {
					RAWNAND_ERR("%s rawnand set to toggle %s failed,while toggle vendor specific setting!\n", __func__,
						    (ddr_type == TOG_DDR2) ? "ddr2" : "ddr");
					return ret;
				}
			}
		}
	} else {
		RAWNAND_ERR("wrong nand interface type!\n");
		return -1;
	}
	return NAND_OP_TRUE;
}

void rawnand_update_timings_ift_ops(int mfr_type)
{
	switch (mfr_type) {
	case NAND_MFR_MICRON:
		rawnand_itf_ops = micron_itf_ops;
		break;
	case NAND_MFR_HYNIX:
		rawnand_itf_ops = hynix_itf_ops;
		break;
	case NAND_MFR_INTEL:
		rawnand_itf_ops = intel_itf_ops;
		break;
	case NAND_MFR_SANDISK:
		rawnand_itf_ops = sandisk_itf_ops;
		break;
	case NAND_MFR_TOSHIBA:
		rawnand_itf_ops = toshiba_itf_ops;
		break;
	case NAND_MFR_SAMSUNG:
		rawnand_itf_ops = sansumg_itf_ops;
		break;
	default:
		break;
	}
}

struct itf_ops_t sansumg_itf_ops = {
	.toggle = {
		.specific_setting = set_nand_toggle_specific_setting_default,
		.driver_strength = set_nand_toggle_driver_strength_default,
		.vendor_specific_setting = set_nand_toggle_vendor_specific_setting_default,
	}
};

struct itf_ops_t sandisk_itf_ops = {
	.toggle = {
		.specific_setting = set_nand_toggle_specific_setting_default,
		.driver_strength = set_nand_toggle_driver_strength_default,
		.vendor_specific_setting = sandisk_toggle_vendor_specific_setting,
	}
};

struct itf_ops_t toshiba_itf_ops = {
	.toggle = {
		.specific_setting = set_nand_toggle_specific_setting_default,
		.driver_strength = set_nand_toggle_driver_strength_default,
		.vendor_specific_setting = toshiba_toggle_vendor_specific_setting,
	}
};

struct itf_ops_t hynix_itf_ops = {
	.onfi = {
		.ddr2_cfg = set_nand_onfi_ddr2_confiure_default,
		.driver_strength = set_nand_onfi_driver_strength_default,
		.rb_strength = set_nand_onfi_rb_strength_default,
		.timing_mode = set_nand_onfi_timing_mode_default,
	}
};

struct itf_ops_t micron_itf_ops = {
	.onfi = {
		.ddr2_cfg = set_nand_onfi_ddr2_confiure_default,
		.driver_strength = set_nand_onfi_driver_strength_default,
		.rb_strength = micron_onfi_rb_strength_setting,
		.timing_mode = set_nand_onfi_timing_mode_default,
	}
};

struct itf_ops_t intel_itf_ops = {
	.onfi = {
		.ddr2_cfg = set_nand_onfi_ddr2_confiure_default,
		.driver_strength = set_nand_onfi_driver_strength_default,
		.rb_strength = set_nand_onfi_rb_strength_default,
		.timing_mode = set_nand_onfi_timing_mode_default,
	}
};
