///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_CEC_FETURE.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _ITE6805_CEC_FETURE_H_
#define _ITE6805_CEC_FETURE_H_

#if (_ENABLE_IT6805_CEC_ == TRUE)
void iTE6805_CEC_CMD_Feature_decode(pCEC_FRAME CEC_FRAME);

void iTE6805_CEC_MSG_Reply_CEC_Version(iTE_u8 TARGET_LA, iTE_u8 CEC_Version);
void iTE6805_CEC_MSG_Feture_CEC_Version(iTE_u8 TARGET_LA);
void iTE6805_CEC_MSG_Future_Report_PA(iTE_u8 PA_LOW, iTE_u8 PA_HIGH, iTE_u8 prim_devtype);
void iTE6805_CEC_MSG_Future_Give_PA(void);

void iTE6805_CEC_MSG_Future_Polling(iTE_u8 TARGET_LA);
void iTE6805_CEC_MSG_Feture_Abort(iTE_u8 TARGET_LA, iTE_u8 CEC_RXCMD, iTE_u8 Abort_Reason);
void iTE6805_CEC_MSG_Reply_Feture_Abort(pCEC_FRAME RX_CEC_FRAME, iTE_u8 Abort_Reason);
void iTE6805_CEC_MSG_Abort(iTE_u8 TARGET_LA);

void iTE6805_CEC_MSG_Device_VenderID(iTE_u32 vendor_id);
void iTE6805_CEC_MSG_Give_Deive_VendorID(void);

void iTE6805_CEC_MSG_STANDBY(void);
void iTE6805_CEC_MSG_REPORT_POWER_STATUS(iTE_u8 CASTING_MODE, iTE_u8 TARGET_LA);
void iTE6805_CEC_MSG_SET_OSD_NAME(iTE_u8 TARGET_LA);
void iTE6805_CEC_MSG_ACTIVE_SOURCE(iTE_u8 PA_LOW, iTE_u8 PA_HIGH);
#endif
#endif
