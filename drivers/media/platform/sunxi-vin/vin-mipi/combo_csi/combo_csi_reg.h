/*
 * combo csi module
 *
 * Copyright (c) 2019 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Authors:  Zheng Zequn <zequnzheng@allwinnertech.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "combo_csi_reg_i.h"

#ifndef __COMBO_CSI_REG__H__
#define __COMBO_CSI_REG__H__

int cmb_csi_set_base_addr(unsigned int sel, unsigned long addr);
void cmb_phy_top_set_VRFE_0P9(unsigned int sel, unsigned int en);
void cmb_phy_trescal_enable(unsigned int sel);
void cmb_phy_trescal_disble(unsigned int sel);
void cmb_phy_set_trescal(unsigned int sel, unsigned int en);
void cmb_phy0_OFS0_AUTO(unsigned int sel, unsigned int data);
void cmb_phy0_OFS0_SOFT(unsigned int sel, unsigned int en);
void cmb_phy0_OFS1(unsigned int sel, unsigned int data);

void CMB_PHY0_LP_REFI(unsigned int sel, unsigned int en);
void CMB_PHY0_HS_REFI(unsigned int sel, unsigned int en);

void cmb_phy_top_enable(unsigned int sel);
void cmb_phy_top_disable(unsigned int sel);
void cmb_phy_power_enable(unsigned int sel);
void cmb_phy_power_disable(unsigned int sel);
void cmb_phy0_en(unsigned int sel, unsigned int en);
void cmb_phy_lane_num_enable(unsigned int sel, unsigned int lanedt);
void cmb_phy_set_link_mode(unsigned int sel, unsigned int mode);
void cmb_phy_mipi_lpnum_enable(unsigned int sel, unsigned int lane);

void cmb_phy0_ibias_en(unsigned int sel, unsigned int en);
void cmb_phy0_term_dly(unsigned int sel, unsigned int dly);
void cmb_phy0_hs_dly(unsigned int sel, unsigned int dly);
void cmb_phy0_s2p_width(unsigned int sel, unsigned int width);
void cmb_phy0_s2p_dly(unsigned int sel, unsigned int dly);
void cmb_phy_mipi_lpnum_enable(unsigned int sel, unsigned int lane);
void cmb_phy0_mipilp_dbc_en(unsigned int sel, unsigned int en);
void cmb_phy0_mipi_sync_mode(unsigned int sel, unsigned int en);
void cmb_phy0_set_OFS1(unsigned int sel, unsigned int en);
void cmb_phy1_set_OFS1(unsigned int sel, unsigned int en);

void cmb_port_enable(unsigned int sel);
void cmb_port_disable(unsigned int sel);
void cmb_port_lane_num(unsigned int sel, unsigned int num);
void cmb_port_out_num(unsigned int sel, unsigned int num);
void cmb_port_channel_num(unsigned int sel, unsigned int num);

void cmb_port_lane_map(unsigned int sel);
void cmb_port_mipi_enpack_enable(unsigned int sel);
void cmb_port_mipi_enpack_disable(unsigned int sel);
void cmb_port_mipi_yuv_seq(unsigned int sel, unsigned int seq);
void cmb_port_mipi_ch0_dt(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch1_dt(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch2_dt(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch3_dt(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch0_vc(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch1_vc(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch2_vc(unsigned int sel, unsigned int type);
void cmb_port_mipi_ch3_vc(unsigned int sel, unsigned int type);
void cmb_phy0_SET_INI_PD(unsigned int sel, unsigned int en);

void cmb_port_mipi_ch_trig_en(unsigned int sel, unsigned int en);
void cmb_port_set_user_DT(unsigned int sel);
void cmb_port_set_DT(unsigned int sel);



void cmb_phy0_en_PHYB(unsigned int sel, unsigned int en);
void CMB_PHY0_LP_REFI_PHYB(unsigned int sel, unsigned int en);
void CMB_PHY0_HS_REFI_PHYB(unsigned int sel, unsigned int en);
void cmb_phy_lane_num_enable_PHYB(unsigned int sel, unsigned int lanedt);
void cmb_phy_set_link_mode_PHYB(unsigned int sel, unsigned int mode);
void cmb_phy0_ibias_en_PHYB(unsigned int sel, unsigned int en);
void cmb_phy0_OFS0_AUTO_PHYB(unsigned int sel, unsigned int data);
void cmb_phy0_OFS0_SOFT_PHYB(unsigned int sel, unsigned int en);
void cmb_phy0_term_dly_PHYB(unsigned int sel, unsigned int dly);
void cmb_phy0_s2p_dly_PHYB(unsigned int sel, unsigned int dly);
void cmb_phy0_s2p_width_PHYB(unsigned int sel, unsigned int width);
void cmb_phy_mipi_lpnum_enable_PHYB(unsigned int sel, unsigned int lane);
void cmb_phy0_mipilp_dbc_en_PHYB(unsigned int sel, unsigned int en);
void cmb_phy0_mipi_sync_mode_PHYB(unsigned int sel, unsigned int en);
void cmb_phy0_hs_dly_PHYB(unsigned int sel, unsigned int dly);
void cmb_phy0_SET_INI_PD_PHYB(unsigned int sel, unsigned int en);
void cmb_phy0_OFS1_PHYB(unsigned int sel, unsigned int data);

void cmb_port1_out_num(unsigned int sel, unsigned int num);
void cmb_port1_channel_num(unsigned int sel, unsigned int num);
void cmb_port1_lane_num(unsigned int sel, unsigned int num);
void cmb_port1_lane_map(unsigned int sel);
void cmb_port1_mipi_enpack_enable(unsigned int sel);
void cmb_port1_mipi_enpack_disable(unsigned int sel);
void cmb_port1_mipi_yuv_seq(unsigned int sel, unsigned int seq);
void cmb_port1_mipi_ch0_dt(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch1_dt(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch2_dt(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch3_dt(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch0_vc(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch1_vc(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch2_vc(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch3_vc(unsigned int sel, unsigned int type);
void cmb_port1_mipi_ch_trig_en(unsigned int sel, unsigned int en);
void cmb_port1_enable(unsigned int sel);
void cmb_port1_disable(unsigned int sel);

#endif /*__COMBO_CSI_REG__H__*/
