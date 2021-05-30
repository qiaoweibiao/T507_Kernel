///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_CEC_SYS.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _ITE6805_CEC_SYS_H_
#define _ITE6805_CEC_SYS_H_

#if (_ENABLE_IT6805_CEC_ == TRUE)
void iTE6805_hdmirx_CEC_irq(void);

void	iTE6805_CEC_fsm(void);
void	iTE6805_CEC_chg(STATECEC_Type NewState);

iTE_u8	iTE6805_CEC_CMD_Check(pCEC_FRAME CEC_FRAME);
void	iTE6805_CEC_CMD_Print(pCEC_FRAME CEC_FRAME);
void	iTE6805_CEC_CMD_Push_To_Queue_Handler(void);
void	iTE6805_CEC_CMD_Ready_To_Fire(void);

void iTE6805_CEC_INIT(void);

#endif
#endif

