///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_EDID.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/

#ifndef _ITE6805_EDID_H_
#define _ITE6805_EDID_H_

void	iTE6805_EDID_Init(void);
void 	iTE6805_EDID_RAMInitial(void);
iTE_u8	iTE6805_EDID_UpdateRAM(iTE_u8 *pEDID, iTE_u8 BlockNUM);
#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_)
iTE_u8	iTE6805_EDID_UpdateBloack0(iTE_u8 *pEDID);
iTE_u8	iTE6805_EDID_UpdateBloack1(iTE_u8 *pEDID);
#endif
iTE_u8	iTE6805_EDID_Find_Phyaddress(iTE_u8 *pEDID);
void	iTE6805_EDID_ParseVSDB_3Dblock(void);

#ifdef _HDMI_SWITCH_
void	iTE6805_EDID_Set_Phyaddress(void);
#endif

#endif // _ITE6805_EDID_H_
