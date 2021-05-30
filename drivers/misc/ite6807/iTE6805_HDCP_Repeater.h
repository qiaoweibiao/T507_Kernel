///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_HDCP_Repeater.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _iTE6805_HDCP_REPEATER_H_
#define _iTE6805_HDCP_REPEATER_H_

#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
void iTE6805_HDCP_Repeater_INIT(void);
void iTE6805_HDCP_Repeater_fsm(void);
void iTE6805_HDCP_Repeater_chg(STATER_Type New_STATE);
void iTE6805_EDID_Set_RepeaterCECPhyaddress(void);
void iTE6805_HDCP_EnableRepeaterMode(iTE_u8 Enable);
#endif

#endif
