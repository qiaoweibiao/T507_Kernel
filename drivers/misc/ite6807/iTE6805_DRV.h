///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_DRV.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _ITE6805_DRV_H_
#define _ITE6805_DRV_H_

void iTE6805_Set_LVDS_Video_Path(iTE_u8 LaneNum);
void iTE6805_Set_TTL_Video_Path(void);
void iTE6805_Set_AVMute(iTE_u8 AVMUTE_STATE);
void iTE6805_Set_ColorDepth(void);
void iTE6805_Set_DNScale(void);
void iTE6805_Set_HPD_Ctrl(iTE_u16 PORT_NUM, iTE_u16 HPD_State);
void iTE6805_Set_Video_Tristate(iTE_u8 VIDEO_STATE);
void iTE6805_Set_Audio_Tristate(iTE_u8 AUDIO_STATE);
void iTE6805_Set_1B0_By_PixelClock(void);
void iTE6805_Set_47_By_TMDS(void);

#if (iTE6807 == 1) && (iTE6807_EnSSC == 1)
void iTE6807_Set_EnSSC(void);
#endif

#if defined(_ENABLE_6805_INT_MODE_FUNCTION_) && (_ENABLE_6805_INT_MODE_FUNCTION_ == TRUE)
void iTE6805_Set_INT_Port(void);
#endif

#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
void iTE6805_Set_Power_Mode(iTE_u8 Mode);
#endif

#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
void iTE6805_Set_HDCP(iTE_u8 HDCP_STATE);
#endif

void iTE6805_Reset_ALL_Logic(iTE_u8 PORT_NUM);
void iTE6805_Reset_Video_Logic(void);
void iTE6805_Reset_Audio_Logic(void);
void iTE6805_Reset_Video_FIFO(void);

iTE_u8 iTE6805_Check_Support4KTiming(void);
iTE_u8 iTE6805_Check_PORT0_IS_MHL_Mode(iTE_u8 PORT_NUM);
iTE_u8 iTE6805_Check_HDMI_OR_DVI_Mode(iTE_u8 PORT_NUM);
iTE_u8 iTE6805_Check_CLK_Vaild(void);
iTE_u8 iTE6805_Check_CLK_Stable(void);
iTE_u8 iTE6805_Check_SCDT(void);
iTE_u8 iTE6805_Check_AUDT(void);
iTE_u8 iTE6805_Check_AVMute(void);
iTE_u8 iTE6805_Check_5V_State(iTE_u8 PORT_NUM);
iTE_u8 iTE6805_Check_Single_Dual_Mode(void);
iTE_u8 iTE6805_Check_4K_Resolution(void);
iTE_u8 iTE6805_Check_HDMI2(void);
iTE_u8 iTE6805_Check_EQ_Busy(void);
iTE_u8 iTE6805_Check_TMDS_Bigger_Than_1G(void);
iTE_u8 iTE6805_Check_Scramble_State(void);

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
void iTE6805_Check_MHL_Mode_Change_Need_Power_On(void);
#endif


void	iTE6805_Get_AVIInfoFrame_Info(void);
void	iTE6805_Get_VID_Info(void);
void	iTE6805_Get_PCLK(void);
void	iTE6805_Get_TMDS(iTE_u8 i);
void	iTE6805_GET_InputType(void);
//iTE_u16	iTE6805_Get_VIC_Number();

void	iTE6805_Show_VID_Info(void);
void	iTE6805_Show_AUD_Info(void);
void	iTE6805_Show_AVIInfoFrame_Info(void);
void	chgbank(iTE_u16 bank);

void 	iTE6805_OCLK_Cal(void);
#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
void	iTE6805_OCLK_Set(iTE_u8 MODE);
#endif
iTE_u32 iTE6805_OCLK_Load(void);

void iTE6805_Init_fsm(void);
void iTE6805_Init_TTL_VideoOutputConfigure(void);
void iTE6805_Init_CAOF(void);
void iTE6805_Init_6028LVDS(iTE_u8 chip);


#endif


