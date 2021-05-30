/*
 * nand_ids.c
 *
 * Copyright (C) 2019 Allwinner.
 *
 * cuizhikui <cuizhikui@allwinnertech.com>
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include "rawnand_ids.h"
#include "rawnand_chip.h"
#include "rawnand_boot.h"
#include "rawnand_debug.h"

/*cmd stay to handle*/
struct nand_phy_op_par phy_op_para[] = {
    {
	// 0 index
	.instr = {
		.read_instr = {0x00, 0x30},
		.multi_plane_read_instr = {0x00, 0x30, 0x00, 0x30}, //simulate
		.write_instr = {0x80, 0x10},
		.multi_plane_write_instr = {0x80, 0x10, 0x80, 0x10}, //simulate
	},
    },
    {
	// OP_SET_1 index
	.instr = {
		.read_instr = {0x00, 0x30},
		.multi_plane_read_instr = {0x00, 0x30, 0x00, 0x30}, //simulate
		.write_instr = {0x80, 0x10},
		.multi_plane_write_instr = {0x80, 0x11, 0x80, 0x10},
	},
    },
    {
	// OP_SET_2 index
	.instr = {
		.read_instr = {0x00, 0x30},
		.multi_plane_read_instr = {0x00, 0x32, 0x00, 0x30},
		.write_instr = {0x80, 0x10},
		.multi_plane_write_instr = {0x80, 0x11, 0x81, 0x10},
	},
    },
    {
	// OP_SET_3 index
	.instr = {
		.read_instr = {0x00, 0x30},
		.multi_plane_read_instr = {0x00, 0x32, 0x00, 0x30},
		.write_instr = {0x80, 0x10},
		.multi_plane_write_instr = {0x80, 0x11, 0x80, 0x10},
	},
    },
};

struct sunxi_nand_flash_device raw_sandisk[] = {
    {
	.name = "SDTNSGAMA-016G",
	.id = {0x45, 0x3A, 0x94, 0x93, 0x76, 0x51, 0xff, 0xff},
	.die_cnt_per_chip = 1,
	.sect_cnt_per_page = 32,
	.page_cnt_per_blk = 256,
	.blk_cnt_per_die = 4212,
	.operation_opt = NAND_MULTI_PROGRAM | NAND_RANDOM |
			SANDISK_LSB_PAGE,
	.valid_blk_ratio = VALID_BLK_RATIO_DEFAULT,
	.access_freq = 40,
	.ecc_mode = BCH_40,
	.read_retry_type = 0x342009,
	.ddr_type = SDR,
	.bad_block_flag_position = FIRST_TWO_PAGES,
	.multi_plane_block_offset = 1,
	.option_physic_op_no = OP_SET_2,
	.selected_write_boot0_no = NAND_WRITE_BOOT0_GENERIC,
	.selected_readretry_no = NAND_READRETRY_SANDISK,
	.ddr_info_no = 1,
	.id_number = 0x0,
	.max_blk_erase_times = 3000,
	.access_high_freq = 40,
    },
};

struct sunxi_nand_flash_device raw_toshiba[] = {
    {
	.name = "TC58TEG6DDLTA00",
	.id = {0x98, 0xde, 0x94, 0x93, 0x76, 0x51, 0xff, 0xff},
	.die_cnt_per_chip = 1,
	.sect_cnt_per_page = 32,
	.page_cnt_per_blk = 256,
	.blk_cnt_per_die = 2148,
	.operation_opt = NAND_MULTI_PROGRAM | NAND_RANDOM |
			TOSHIBA_LSB_PAGE | NAND_VCCQ_1P8V |
			NAND_IO_DRIVER_STRENGTH | NAND_VENDOR_SPECIFIC_CFG,
	.valid_blk_ratio = VALID_BLK_RATIO_DEFAULT,
	.access_freq = 60,
	.ecc_mode = BCH_40,
	.read_retry_type = 0x120a05,
	.ddr_type = TOG_DDR,
	.ddr_opt = NAND_VCCQ_1P8V | NAND_TOGGLE_IO_DRIVER_STRENGTH
				| NAND_TOGGLE_VENDOR_SPECIFIC_CFG,
	.bad_block_flag_position = LAST_PAGE,
	.multi_plane_block_offset = 1,
	.option_physic_op_no = OP_SET_2,
	.selected_write_boot0_no = NAND_WRITE_BOOT0_GENERIC,
	.selected_readretry_no = NAND_READRETRY_TOSHIBA,
	.ddr_info_no = 0,
	.id_number = 0x0,
	.max_blk_erase_times = 3000,
	.access_high_freq = 60,
    },
};

struct sunxi_nand_flash_device raw_micron[] = {
    {
	.name = "MT29F64G08CBABA",
	.id = {0x2c, 0x64, 0x44, 0x4B, 0xA9, 0xff, 0xff, 0xff},
	.die_cnt_per_chip = 1,
	.sect_cnt_per_page = 16,
	.page_cnt_per_blk = 256,
	.blk_cnt_per_die = 4096,
	.operation_opt = NAND_MULTI_PROGRAM | NAND_RANDOM |
			  MICRON_0x41_LSB_PAGE | NAND_TIMING_MODE |
			 NAND_IO_DRIVER_STRENGTH,
	.valid_blk_ratio = VALID_BLK_RATIO_DEFAULT,
	.access_freq = 40,
	.ecc_mode = BCH_48,
	.read_retry_type = 0x400a01,
	/*.ddr_type = ONFI_DDR,*/
	.ddr_type = SDR,
	.bad_block_flag_position = FIRST_TWO_PAGES,
	.multi_plane_block_offset = 1,
	.option_physic_op_no = OP_SET_1,
	.selected_write_boot0_no = NAND_WRITE_BOOT0_GENERIC,
	.selected_readretry_no = NAND_READRETRY_MICRON,
	.ddr_info_no = 0,
	.id_number = 0x0,
	.max_blk_erase_times = 3000,
	.access_high_freq = 40,
    },
    {
	.name = "MT29F32G08CBADB",
	.id = {0x2c, 0x44, 0x44, 0x4B, 0xA9, 0xff, 0xff, 0xff},
	.die_cnt_per_chip = 1,
	.sect_cnt_per_page = 16,
	.page_cnt_per_blk = 256,
	.blk_cnt_per_die = 2128,
	.operation_opt = NAND_MULTI_PROGRAM | NAND_RANDOM |
			 MICRON_0x41_LSB_PAGE,
	.valid_blk_ratio = VALID_BLK_RATIO_DEFAULT,
	.access_freq = 60,
	.ecc_mode = BCH_48,
	.read_retry_type = 0x400a01,
	.ddr_type = ONFI_DDR,
	.ddr_opt = NAND_ONFI_TIMING_MODE | NAND_ONFI_IO_DRIVER_STRENGTH
				| NAND_ONFI_RB_STRENGTH,
	.bad_block_flag_position = FIRST_TWO_PAGES,
	.multi_plane_block_offset = 1,
	.option_physic_op_no = OP_SET_1,
	.selected_write_boot0_no = NAND_WRITE_BOOT0_GENERIC,
	.selected_readretry_no = NAND_READRETRY_MICRON,
	.ddr_info_no = 0,
	.id_number = 0x1,
	.max_blk_erase_times = 3000,
	.access_high_freq = 60,
    },
    {
	.name = "MT29F128G08CFAAA",
	.id = {0x2c, 0x88, 0x04, 0x4B, 0xA9, 0xff, 0xff, 0xff},
	.die_cnt_per_chip = 1,
	.sect_cnt_per_page = 16,
	.page_cnt_per_blk = 256,
	.blk_cnt_per_die = 4096,
	.operation_opt = NAND_MULTI_PROGRAM | NAND_RANDOM |
			 GENERIC_LSB_PAGE,
	.valid_blk_ratio = VALID_BLK_RATIO_DEFAULT,
	.access_freq = 40,
	.ecc_mode = BCH_28,
	.read_retry_type = 0,
	.ddr_type = SDR,
	.bad_block_flag_position = FIRST_PAGE,
	.multi_plane_block_offset = 1,
	.option_physic_op_no = OP_SET_3,
	.selected_write_boot0_no = NAND_WRITE_BOOT0_GENERIC,
	.selected_readretry_no = NAND_READRETRY_NO,
	.ddr_info_no = 0,
	.id_number = 0x2,
	.max_blk_erase_times = 3000,
	.access_high_freq = 40,
    },
};

struct sunxi_nand_flash_device raw_spansion[] = {
    {
	.name = "S34ML01G200TFI000",
	.id = {0x01, 0xf1, 0x80, 0x1d, 0xff, 0xff, 0xff, 0xff},
	.die_cnt_per_chip = 1,
	.sect_cnt_per_page = 4,
	.page_cnt_per_blk = 64,
	.blk_cnt_per_die = 1024,
	.operation_opt = NAND_RANDOM,
	.valid_blk_ratio = VALID_BLK_RATIO_DEFAULT,
	.access_freq = 30,
	.ecc_mode = BCH_16,
	.read_retry_type = READ_RETRY_TYPE_NULL,
	.ddr_type = SDR,
	.bad_block_flag_position = FIRST_TWO_PAGES,
	.multi_plane_block_offset = 1,
	.option_physic_op_no = OP_SET_0,
	.selected_write_boot0_no = NAND_WRITE_BOOT0_GENERIC,
	.selected_readretry_no = NAND_READRETRY_NO,
	.ddr_info_no = 0,
	.id_number = 0x0,
	.max_blk_erase_times = 60000,
	.access_high_freq = 30,
    },
};
struct nand_manufacture mfr_tbl[] = {
    NAND_MANUFACTURE(NAND_MFR_MICRON, "micro", raw_micron),
    NAND_MANUFACTURE(NAND_MFR_SPANSION, "spansion", raw_spansion),
    NAND_MANUFACTURE(NAND_MFR_SANDISK, "sandisk", raw_sandisk),
    NAND_MANUFACTURE(NAND_MFR_TOSHIBA, "toshiba", raw_toshiba),
};

/**
 * sunxi_search_id: search id is id table
 * @id : this id is read from device
 * */
struct sunxi_nand_flash_device *sunxi_search_id(unsigned char *id)
{
	int m = 0;
	int d = 0;
	int i = 0;
	u8 mfr_id = id[0];
	u8 dev_id = id[1];
	int cnt = 0;
	int match_id_cnt = 0;
	struct sunxi_nand_flash_device *match = NULL;

	for (m = 0; m < sizeof(mfr_tbl) / sizeof(struct nand_manufacture); m++) {
		struct nand_manufacture *mfr = &mfr_tbl[m];

		if (mfr_id == mfr->id) {
			for (d = 0; d < mfr->ndev; d++) {
				struct sunxi_nand_flash_device *dev =
				    &mfr->dev[d];
				cnt = 0;
				if (dev_id == dev->dev_id) {
					for (i = 0; i < NAND_MAX_ID_LEN; i++) {
						if (dev->id[i] == id[i])
							cnt++;
						else
							break;
					}
					if (cnt > match_id_cnt) {
						if (cnt >= NAND_MIN_ID_LEN) {
							match = dev;
							match_id_cnt = cnt;
						}
					}
				}
			}
		}
	}
	return match;
}

int generic_is_lsb_page(__u32 page_num) //0x00
{
	return 1;
}

int hynix20nm_is_lsb_page(__u32 page_num) //0x01
{
	struct nand_chip_info *nci = g_nsi->nci;

	//hynix 20nm
	if ((page_num == 0) || (page_num == 1))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int hynix26nm_is_lsb_page(__u32 page_num) //0x01
{
	struct nand_chip_info *nci = g_nsi->nci;

	//hynix 26nm
	if ((page_num == 0) || (page_num == 1))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int hynix16nm_is_lsb_page(__u32 page_num) //0x02
{
	__u32 pages_per_block;
	//	__u32 read_retry_type;
	struct nand_chip_info *nci = g_nsi->nci;

	pages_per_block = nci->page_cnt_per_blk;

	if (page_num == 0)
		return 1;
	if (page_num == pages_per_block - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}

int toshiba_is_lsb_page(__u32 page_num) //0x10
{
	struct nand_chip_info *nci = g_nsi->nci;

	//toshiba 2xnm 19nm 1ynm
	if (page_num == 0)
		return 1;
	if (page_num == nci->npi->page_cnt_per_blk - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}

int samsung_is_lsb_page(__u32 page_num) //0x20
{
	struct nand_chip_info *nci = g_nsi->nci;

	//(NAND_LSBPAGE_TYPE == 0x20) //samsung 25nm
	if (page_num == 0)
		return 1;
	if (page_num == nci->npi->page_cnt_per_blk - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}
int sandisk_is_lsb_page(__u32 page_num) //0x30
{
	struct nand_chip_info *nci = g_nsi->nci;

	//sandisk 2xnm 19nm 1ynm
	if (page_num == 0)
		return 1;
	if (page_num == nci->npi->page_cnt_per_blk - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}

int micron_0x40_is_lsb_page(__u32 page_num) // 20nm (29f64g08cbaba) 0x40
{
	struct nand_chip_info *nci = g_nsi->nci;

	if ((page_num == 0) || (page_num == 1))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int micron_0x41_is_lsb_page(__u32 page_num) //20nm (29f32g08cbada) 0x41
{
	struct nand_chip_info *nci = g_nsi->nci;

	//micron 20nm L83A L84A L84C L84D L85A L85C
	if ((page_num == 2) || (page_num == 3))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 1;
	if ((page_num % 4 == 0) || (page_num % 4 == 1))
		return 1;
	return 0;
}

int micron_0x42_is_lsb_page(__u32 page_num) // 16nm l95b 0x42
{
	//	struct nand_chip_info *nci = g_nsi->nci;

	//micron 16nm l95b
	if ((page_num == 0) || (page_num == 1) || (page_num == 2) || (page_num == 3) || (page_num == 4) || (page_num == 5) || (page_num == 7) || (page_num == 8) || (page_num == 509))
		return 1;
	if ((page_num == 6) || (page_num == 508) || (page_num == 511))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

lsb_page_t chose_lsb_func(__u32 no)
{
	no = (no << LSB_PAGE_POS);

	switch (no) {
	case GENERIC_LSB_PAGE:
		return generic_is_lsb_page;
	case HYNIX20_26NM_LSB_PAGE:
		return hynix20nm_is_lsb_page;
	case HYNIX16NM_LSB_PAGE:
		return hynix16nm_is_lsb_page;
	case TOSHIBA_LSB_PAGE:
		return toshiba_is_lsb_page;
	case SAMSUNG_LSB_PAGE:
		return samsung_is_lsb_page;
	case SANDISK_LSB_PAGE:
		return sandisk_is_lsb_page;
	case MICRON_0x40_LSB_PAGE:
		return micron_0x40_is_lsb_page;
	case MICRON_0x41_LSB_PAGE:
		return micron_0x41_is_lsb_page;
	case MICRON_0x42_LSB_PAGE:
		return micron_0x42_is_lsb_page;
	default:
		return generic_is_lsb_page;
	}
	return generic_is_lsb_page;
}

struct sunxi_nand_flash_device selected_id_tbl;

struct nfc_init_ddr_info def_ddr_info[] = {
    ////////////////////////////
    {
	0, //en_dqs_c;
	0, //en_re_c;
	0, //odt;
	0, //en_ext_verf;
	0, //dout_re_warmup_cycle;
	0, //din_dqs_warmup_cycle;
	0, //output_driver_strength;
	0, //rb_pull_down_strength;
    },
    ////////////////////////////
    {
	0, //en_dqs_c;
	0, //en_re_c;
	0, //odt;
	0, //en_ext_verf;
	0, //dout_re_warmup_cycle;
	0, //din_dqs_warmup_cycle;
	2, //output_driver_strength;
	2, //rb_pull_down_strength;
    },
    ////////////////////////////
};


