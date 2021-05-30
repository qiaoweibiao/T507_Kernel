///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_HDCP_Repeater_Callback.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _iTE6805_HDCP_REPEATER_CALLBACK_H_
#define _iTE6805_HDCP_REPEATER_CALLBACK_H_


#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
void iTE6805_Inform_HPD_Unplug_CB(void);
void iTE6805_Inform_HDCP1_KSVListReady_CB(unsigned char *BKSV, unsigned char *BStatus, unsigned char *KSV_List);
void iTE6805_Inform_HDCP2_KSVListReady_CB(unsigned char *RxID, unsigned char *RxInfo,  unsigned char *RxID_List);
void iTE6805_Inform_HDCPDone_CB(unsigned char *BKSV_RxID);
void iTE6805_Inform_EDIDReadyBlock0_CB(unsigned char *EDID);
void iTE6805_Inform_EDIDReadyBlock1_CB(unsigned char *EDID);
void iTE6805_Inform_EDIDReadyNoNeedUpdate_CB(void);
void iTE6805_Inform_HDCP_EnableRepeaterMode(unsigned char Enable);
#endif

#endif
