///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_SYS.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#ifndef _ITE6805_SYS_H_
#define _ITE6805_SYS_H_

// Main Loop
void iTE6805_FSM(void);
void iTE6805_MainBody(void);

// IRQ
void iTE6805_hdmirx_port0_SYS_irq(void);
void iTE6805_hdmirx_port1_SYS_irq(void);
void iTE6805_hdmirx_common_irq(void);

// finite state machine
void iTE6805_vid_fsm(void);
void iTE6805_aud_fsm(void);
void iTE6805_vid_chg(STATEV_Type NewState);
void iTE6805_aud_chg(STATEA_Type NewState);

// Customer need to call functions
void iTE6805_Port_Select(iTE_u8 ucPortSel);
void iTE6805_Port_Select_Body(iTE_u8 ucPortSel);

#if (_ENABLE_EDID_RAM_ == TRUE)
void iTE6805_Port_SetEDID(iTE_u8 SET_PORT, iTE_u8 EDID_INDEX);
void iTE6805_Port_SetEDID_Body(iTE_u8 SET_PORT, iTE_u8 EDID_INDEX);
#endif

void iTE6805_Port_Reset(iTE_u8 ucPortSel);
void iTE6805_Port_Reset_Body(iTE_u8 ucPortSel);


#if (iTE68051 == 1)
void iTE68051_Video_Output_Setting(void);
#endif

#if (iTE68052 == 1)
void iTE68052_Video_Output_Setting(void);
#endif

#if (iTE6807 == 1)
void iTE6807_Video_Output_Setting(void);
#endif

void iTE6805_Enable_Video_Output(void);
void iTE6805_Enable_Audio_Output(void);

// INT function
void iTE6805_INT_5VPWR_Chg(iTE_u8 ucport);
void iTE6805_INT_HDMI_DVI_Mode_Chg(iTE_u8 ucport);
void iTE6805_INT_ECC_ERROR(void);
void iTE6805_INT_SCDT_Chg(void);

// Identify chip
iTE_u8 iTE6805_Identify_Chip(void);

#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
void iTE6805_HDCP_Detect(void);
#endif

#if (ENABLE_DETECT_DRM_PKT == TRUE)
void iTE6805_DRM_Detect(void);
void iTE6805_GET_DRMInfoframe(iTE_u8 *pInfoframe);
void iTE6805_CHECK_DRMInfoframe(iTE_u8 *pFlag_HAVE_DRM_PKT, iTE_u8 *pFlag_NEW_DRM_PKT_RECEIVE);
#endif

/* do not using this anymore and multi-thread may occur error
void iTE6805_GET_AVIInfoframe(iTE_u8 *pInfoframe);
void iTE6805_GET_AudioInfoframe(iTE_u8 *pInfoframe);
void iTE6805_GET_SPDInfoframe(iTE_u8 *pInfoframe);
void iTE6805_GET_VendorSpecificInfoframe(iTE_u8 *pInfoframe);
*/

void iTE6805_GET_DownScale_Flag(iTE_u8 *pFlag_DownScale);
void iTE6805_GET_HDMI_ColorType(iTE_u8 *pFlag_ColorType);
void iTE6805_GET_Pixel_Mode(iTE_u8 *pFlag_Pixel_Mode);
void iTE6805_GET_Input_Type(iTE_u8 *pFlag_Input_Type);
void iTE6805_GET_Input_Color(iTE_u8 *pFlag_Input_Color);
void iTE6805_GET_Input_HActive(iTE_u32 *pFlag_Input_HActive);
void iTE6805_GET_EQ_Result(iTE_u8 *pEQ_B_Channel, iTE_u8 *pEQ_G_Channel, iTE_u8 *pEQ_R_Channel);
iTE_u8 iTE6805_GET_Input_Is_LimitRange(void);

void	iTE6805_Detect3DFrame(void);
iTE_u8	iTE6805_Check_ColorChange(void);
void	iTE6805_Check_ColorChange_Update(void);

#if (_ENABLE_EXTERN_EQ_CTRL_ == TRUE)
void iTE6805_Set_EQ_LEVEL(iTE_u8 PORT, EQ_LEVEL EQ);
void iTE6805_Set_EQ_LEVEL_Body(iTE_u8 PORT, EQ_LEVEL EQ);
#endif
#endif


void printf_regall(void);
void printf_reg(void);
