///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_CEC_DRV.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _ITE6805_CEC_DRV_H_
#define _ITE6805_CEC_DRV_H_

void	iTE6805_CEC_Fire_Tx(void);

void	iTE6805_CEC_INIT_CMD(iTE_u8 Initiator, iTE_u8 Follower);

iTE_u8	iTE6805_CEC_Check_IsPollingMsg(void);
iTE_u8	iTE6805_CEC_Check_Fire_Ready(void);

iTE_u8	iTE6805_CEC_Get_TxFailStatus(void);

void	iTE6805_CEC_Reset_RX_FIFO(void);

void	iTE6805_CEC_Clear_INT(void);

// Queue
iTE_u8 iTE6805_CEC_RX_CMD_Push(piTE_u8 pHeader);
iTE_u8 iTE6805_CEC_RX_CMD_Pull(void);

#endif

