///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_EQ.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
void iTE6805_EQ_fsm(void);
void iTE6805_EQ_chg(STATEEQ_Type NewState);
void iTE6805_hdmirx_port0_EQ_irq(void);
void iTE6805_hdmirx_port1_EQ_irq(void);

void iTE6805_Trigger_SAREQ(void);		// for HDMI2.x
void iTE6805_Trigger_EQ(void);			// for HDMI1.x
void iTE6805_Trigger_RSSKEW_EQ(void);	// for HDMI2.x and SAREQ Fail

void	iTE6805_BitErr_Get(void);
iTE_u8	iTE6805_BitErr_Check(void);
iTE_u8	iTE6805_BitErr_Check_Again(void);
iTE_u8	iTE6805_BitErr_Check_For_Parity_Error(void);

void iTE6805_Report_Skew(void);
void iTE6805_Report_EQ(void);

void iTE6805_Reset_EQ(void);

void iTE6805_Get_DFE(iTE_u8 Channel_Color);
void iTE6805_Set_DFE(iTE_u8 EQ_Value, iTE_u8 Type_Channel);

iTE_u8 iTE6805_Find_indexof_DEF(iTE_u8 EQ_Value);

void iTE6805_Set_EQResult_Flag(void);
