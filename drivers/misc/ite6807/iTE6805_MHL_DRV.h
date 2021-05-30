///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_MHL_DRV.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _iTE6805_MHL_DRV_H_
#define _iTE6805_MHL_DRV_H_

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
// MSC Function
iTE_u8		iTE6805_MHL_MSC_Get_Response_State(void);
void		iTE6805_MHL_MSC_Decoder(void);
iTE_u8		iTE6805_MHL_MSC_Fire(MSC_PKT_ID MSC_PKT);
iTE_u8		iTE6805_MHL_MSC_Fire_With_Retry(MSC_PKT_ID MSC_PKT);
void		iTE6805_MHL_MSC_DevCap_Parse(iTE_u8 ucOffset, iTE_u8 ucData);
iTE_u8		iTE6805_MHL_MSC_WriteBurstDataFill(iTE_u8 Offset, iTE_u8 ucByteNo, iTE_u8 *pucData);
void		iTE6805_MHL_3D_REQ_fsm(MHL3D_STATE *e3DReqState);

void		iTE6805_MHL_Set_RAP_Content(iTE_u8 RCP_CONTENT);

#endif
#endif

