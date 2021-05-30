///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_DRV.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#include "iTE6805_Global.h"
#include "iTE6805_VM_TABLE.h"
#include "iTE6805_INI_Table.h"

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
#include "iTE6805_MHL_INI_Table.h"
#endif

struct i2c_client *edid_client;
struct i2c_adapter *edid_adap;
struct i2c_client *mhl_client;
struct i2c_adapter *mhl_adap;
struct i2c_client *cec_client;
struct i2c_adapter *cec_adap;
extern IT6807_CONFIG_INFO it6807_config_info;

extern _iTE6805_DATA	iTE6805_DATA;
extern _iTE6805_VTiming iTE6805_CurVTiming;
void delay1ms(iTE_u16 ms);

iTE_u32 RCLKVALUE;
iTE_u8	CEC_timeunit;

iTE_u8 MHL_AA;

iTE_u32 RCLKVALUE_HDMI;
iTE_u8 HDMI_0x91;
iTE_u8 HDMI_0x92;
iTE_u8 HDMI_0x44;
iTE_u8 HDMI_0x45;
iTE_u8 HDMI_0x46;
iTE_u8 HDMI_0x47;
iTE_u8 HDMI_CEC_timeunit;

typedef struct {
    iTE_u16 HActive ;
    iTE_u16 VActive ;
    iTE_u16 HTotal ;
    iTE_u16 VTotal ;
    iTE_u32 PCLK ;
    iTE_u16 HFrontPorch ;
    iTE_u16 HSyncWidth ;
    iTE_u16 HBackPorch ;
    iTE_u16 VFrontPorch ;
    iTE_u16 VSyncWidth ;
    iTE_u16 VBackPorch ;
    iTE_u32 FrameRate ;
    iTE_u8 ScanMode ;
    iTE_u8 VPolarity ;
    iTE_u8 HPolarity ;
    iTE_u8 PixRpt;
    char *format;
    iTE_u16 VIC;
} iTE6805_SupportVT;



// Add code for check 4K downscale supported timing , If support then do downscale or > 3.2G timing setting downscale will be output unnormal case
// Device need to show out of range is better then received a abnormal video
_CODE iTE6805_SupportVT iTE6805_SupportedDSTiming[] = {

    {3840, 2160, 5280, 2250, 297000, 1056, 88, 296, 8, 10, 72, 25000, PROG, Vpos, Hpos, 1, "CEA Fmt94 3840x2160P@25Hz/50Hz", 94},
    //{3840,2160,5280,2250,594000,1056,88,296,8,10,72,50000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt96 3840x2160P@50Hz",96},
    //{3840,2160,5280,2250,297000,1056,88,296,8,10,72,25000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt104 3840x2160P@25Hz",104},
    //{3840,2160,5280,2250,594000,1056,88,296,8,10,72,50000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt106 3840x2160P@50Hz",106},

    {4096, 2160, 5280, 2250, 297000, 968, 88, 128, 8, 10, 72, 25000, PROG, Vpos, Hpos, 1, "CEA Fmt99 4096x2160P@25Hz/50Hz", 99},
    //{4096,2160,5280,2250,594000,968,88,128,8,10,72,50000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt101 4096x2160P@50Hz",101},

    {3840, 2160, 4400, 2250, 297000, 176, 88, 296, 8, 10, 72, 30000, PROG, Vpos, Hpos, 1, "CEA Fmt95 3840x2160P@30Hz/60Hz", 95},
    //{3840,2160,4400,2250,297000,176,88,296,8,10,72,30000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt105 3840x2160P@30Hz",105},
    //{3840,2160,4400,2250,594000,176,88,296,8,10,72,60000,PROG,Vpos,Hpos,1,AR_16_9,"CEA Fmt97 3840x2160P@60Hz",97},
    //{3840,2160,4400,2250,594000,176,88,296,8,10,72,60000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt107 3840x2160P@60Hz",107},

    {4096, 2160, 5500, 2250, 297000, 1020, 88, 296, 8, 10, 72, 24000, PROG, Vpos, Hpos, 1, "CEA Fmt98 4096x2160P@24Hz", 98},

    {3840, 2160, 5500, 2250, 297000, 1276, 88, 296, 8, 10, 72, 24000, PROG, Vpos, Hpos, 1, "CEA Fmt93 3840x2160P@24Hz", 93},
    //{3840,2160,5500,2250,297000,1276,88,296,8,10,72,24000,PROG,Vpos,Hpos,1,AR_64_27,"CEA Fmt103 3840x2160P@24Hz",103},

    {4096, 2160, 4400, 2250, 297000, 88, 88, 128, 8, 10, 72, 30000, PROG, Vpos, Hpos, 1, "CEA Fmt100 4096x2160P@30Hz/60Hz", 100},
    //{4096,2160,4400,2250,594000,88,88,128,8,10,72,60000,PROG,Vpos,Hpos,1,AR_256_135,"CEA Fmt102 4096x2160P@60Hz",102},
};
#define     SupportVTCount    (sizeof(iTE6805_SupportedDSTiming)/sizeof(iTE6805_SupportVT))


// Block Set
void iTE6805_Set_AVMute(iTE_u8 AVMUTE_STATE)
{
	//	1C5
	//		Reg_TriSVDIO	4:0	1: Tri-state Video data IO when single pixel mode	R/W	11111
	//		Reg_TriSVDLLIO	5	1: Tri-state Video data IO when single pixel mode	R/W	1
	//		Reg_TriSVDLHIO	6	1: Tri-state Video data IO when single pixel mode	R/W	1
	//		Reg_VIOSel	7	1: VIO Tri-state controlled by VIO setting
	//						0: VIO Tri-state controlled by VDIO setting	R/W	1
	//	1C6	Reg_TriDVDIO	5:0	1: Tri-state Video data IO when dual pixel mode	R/W	111111
	//		Reg_TriDVDLLIO	6	1: Tri-state Video data IO when dual pixel mode	R/W	1
	//		Reg_TriVIO	7	1: Tri-state Video control IO	R/W	1

	//	4F	RegDisVAutoMute	7	1: disable video auto mute
	//		RegVDGatting	5	Enable output data gating to zero when no Video display

	// If AVMUTE ON, VIOSel should set 1
	// If VIOSel = 1, If no data, sync still on.
	// If VIOSel = 0, If no data, sync will stop.
	// Therefore AVMute represent sync still on but should disable video.
	// so set 3 set the output black clock (no data), and set VIOSel (sync still go on) or it will reSync(Check Stable) again.
	// If AV Mute On, hardware auto AV mute mute go before SW or it will be a short delay for shut the screen block
	// so Hardware auto AVMute on -> disable AVMute for triiger AVMute again.
	if (AVMUTE_STATE == AVMUTE_ON) {
		chgbank(0);
		hdmirxset(0x4F, BIT5 + BIT7, BIT5 + BIT7); // enable VD Gatting , RegDisVAutoMute : 1: Disable video auto mute
		chgbank(1);
		hdmirxset(0xC5, BIT7, BIT7); // Reg_VIOSel  :  1: VIO Tri-state controlled by VIO setting, 0: VIO Tri-state controlled by VDIO setting
		chgbank(0);
		HDMIRX_VIDEO_PRINTF("+++++++++++ iTE6805_Set_AVMute -> On\n");
	} else {
		HDMIRX_VIDEO_PRINTF("+++++++++++  iTE6805_Set_AVMute -> Off\n");
		if (iTE6805_DATA.STATEV == STATEV_VidStable) {
			if (iTE6805_Check_AVMute()) {
				chgbank(0);
				hdmirxset(0x4F, BIT7, BIT7); // RegDisVAutoMute : 1: Disable video auto mute
			} else {
				// clear AV Mute
				chgbank(1);
				hdmirxset(0xC5, BIT0, BIT0);
				hdmirxset(0xC5, BIT0, 0);
				if (iTE6805_Check_Single_Dual_Mode() == MODE_DUAL) {
					chgbank(1);
					hdmirxset(0xC5, BIT0, BIT0);
				}
				chgbank(0);

				hdmirxset(0x4F, BIT5|BIT7, BIT5|BIT7);	// enable VD Gatting
				hdmirxset(0x4F, BIT5|BIT7, BIT7);		// disable VD Gatting
			}
		}
		/***************wanpeng add******************/
		if ((gpio_get_value(it6807_config_info.backlight_gpio) == 0) && (iTE6805_CurVTiming.TMDSCLK == 594816)) {
			gpio_set_value(it6807_config_info.backlight_gpio, 1);
			printinfo("wanpeng>>>>set vbyone interface lcd backlight\n");
		}
		/***************wanpeng add******************/
	}
}

void iTE6805_Set_ColorDepth(void)
{
#if (iTE6807 == TRUE) && (iTE6807_Force_ByteMode == 0)
	iTE_u8 Reg_IP_Status;
	iTE_u8 Reg_XP_Status;
	iTE_u8 Reg_DRVPWD;
	iTE_u8 Reg5D8;
	iTE_u8 Reg5D9;
	iTE_u8 Reg5DA;
	iTE_u8 Reg5DB = 0;
	iTE_u8 VBOTX_CLKStable;
	iTE_u8 VBOCLKSpd;
    iTE_u8 i;
#endif

	chgbank(0);

#if (iTE68051 == TRUE) || (iTE68052 == TRUE)
	#if (COLOR_DEPTH_BY_GCPINFOFRAME == TRUE)
	iTE6805_CurVTiming.ColorDepth = (hdmirxrd(0x98) & 0xF0);
	#endif

	#if (COLOR_DEPTH_BY_GCPINFOFRAME == TRUE)
	VIDEOTIMNG_DEBUG_PRINTF("\n Input ColorDepth = ");
	switch (iTE6805_CurVTiming.ColorDepth) {
	case 0x00:
	case 0x40:
		VIDEOTIMNG_DEBUG_PRINTF("8 b\n");
		hdmirxset(0x6B, 0x03, 0x00);
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x04);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set RegLVColDep = 01 \n");
		#endif
		break;
	case 0x50:
		VIDEOTIMNG_DEBUG_PRINTF("10 b\n");
		hdmirxset(0x6B, 0x03, 0x01);
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x08);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("10/12 b, set RegLVColDep = 10 \n");
		#endif
		break;
	case 0x60:
		VIDEOTIMNG_DEBUG_PRINTF("12 b\n");
		hdmirxset(0x6B, 0x03, 0x02);
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x08);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("10/12 b, set RegLVColDep = 10 \n");
		#endif
		break;
	default:
		hdmirxset(0x6B, 0x03, 0x00);
		VIDEOTIMNG_DEBUG_PRINTF("8 b\n");
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x04);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set RegLVColDep = 01 \n");
		#endif
		break;
	}
	#else
	VIDEOTIMNG_DEBUG_PRINTF("\n Force set Output ColorDepth = ");
	switch (Output_Color_Depth) {
	case 8:
		VIDEOTIMNG_DEBUG_PRINTF("8 b\n");
		hdmirxset(0x6B, 0x03, 0x00);
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x04);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set RegLVColDep = 01 \n");
		#endif
		break;
	case 10:
		VIDEOTIMNG_DEBUG_PRINTF("10 b\n");
		hdmirxset(0x6B, 0x03, 0x01);
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x08);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("10/12 b, set RegLVColDep = 10 \n");
		#endif
		break;
	case 12:
		VIDEOTIMNG_DEBUG_PRINTF("12 b\n");
		hdmirxset(0x6B, 0x03, 0x02);
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x08);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("10/12 b, set RegLVColDep = 10 \n");
		#endif
		break;
	default:
		hdmirxset(0x6B, 0x03, 0x00);
		VIDEOTIMNG_DEBUG_PRINTF("8 b\n");
		#if (iTE68052 == TRUE)
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x04);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set RegLVColDep = 01 \n");
		#endif
		break;
	}
	#endif
#endif

#if (iTE6807 == TRUE)
	// 20190917 add by mail 'RE: 6265 HDMI1.4 8-7 test 480p jitter'
	chgbank(5);
	if (iTE6805_CurVTiming.PCLK < 30000) {
		hdmirxwr(0xC7, 0x49);
	} else {
		hdmirxwr(0xC7, 0x00);
	}
	chgbank(0);

	#if (iTE6807_Force_ByteMode == 0)
	iTE6805_CurVTiming.ColorDepth = (hdmirxrd(0x98) & 0xF0);
	VIDEOTIMNG_DEBUG_PRINTF("\n Input ColorDepth = ");
	switch (iTE6805_CurVTiming.ColorDepth) {
	case 0x00:
	case 0x40:
		hdmirxset(0x6B, 0x03, 0x00);
		iTE6805_DATA.VBO_ByteMode = 0;
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set ByteMode = 3 (variable iTE6805_DATA.VBO_ByteMode = 0)\n");
		break;
	case 0x50:
		hdmirxset(0x6B, 0x03, 0x01);
		iTE6805_DATA.VBO_ByteMode = 1;
		VIDEOTIMNG_DEBUG_PRINTF("10 b, set ByteMode = 4 (variable iTE6805_DATA.VBO_ByteMode = 1)\n");
		break;
	case 0x60:
		hdmirxset(0x6B, 0x03, 0x02);
		iTE6805_DATA.VBO_ByteMode = 2;
		VIDEOTIMNG_DEBUG_PRINTF("12 b, set ByteMode = 5 (variable iTE6805_DATA.VBO_ByteMode = 2)\n");
		break;
	default:
		hdmirxset(0x6B, 0x03, 0x00);
		iTE6805_DATA.VBO_ByteMode = 0;
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set ByteMode = 3 (variable iTE6805_DATA.VBO_ByteMode = 0)\n");
		break;
	}
	chgbank(5);
	hdmirxset(0xC0, 0xC0, (iTE6805_DATA.VBO_ByteMode<<6));

	i = 0;

	do {
		// add 5D3 bit7 or
		// add 5D2 bit4
		hdmirxset(0xD2, BIT4, 0);
		hdmirxset(0xD2, BIT4, BIT4);

		delay1ms(10);
		Reg5DB = hdmirxrd(0xDB);
		VBOTX_CLKStable = (hdmirxrd(0xDB)&0x10)>>4;
		HDMIRX_DEBUG_PRINT("Wait VBOTX Clock Stable! i = %d\n", (int) i);
		i++;
		if (i > 20)
			break;
	} while (!(Reg5DB & 0x10));

	Reg5D8 = hdmirxrd(0xD8);
	Reg5D9 = hdmirxrd(0xD9);
	Reg5DA = hdmirxrd(0xDA);

	//HDMIRX_DEBUG_PRINT("VBOTX Reg5D8=%2X , Reg5D9 =%2X\n",(int) Reg5D8,(int) Reg5D9);
	//HDMIRX_DEBUG_PRINT("VBOTX StateA=%2X , StateB=%2X\n",(int) (Reg5DA&0x0F),(int) (Reg5DA&0xF0)>>4);
	//HDMIRX_DEBUG_PRINT("VBOTX Reg5DB=%2X \n",(int) Reg5DB);

	VBOCLKSpd = (Reg5DB & 0x0C)>>2;

	if (VBOCLKSpd == 3) {
		hdmirxset(0xC6, 0x07, 0x04);   // 20180822 emily , don't use autoXPHSDet
	} else {
		hdmirxset(0xC6, 0x07, 0x03);  // 20180822 emily , don't use autoXPHSDet
	}

	hdmirxset(0xC1, 0x04, 0x00);       // 20180822 emily , don't use autoXPHSDet

	Reg_IP_Status = hdmirxrd(0xC5);
	Reg_XP_Status = hdmirxrd(0xC6);
	Reg_DRVPWD = hdmirxrd(0xCA);
	//HDMIRX_DEBUG_PRINT("VBOTX_IP_Status=%2X , VBOTX_XP_Status=%2X, VBOTX_DRVPWD=%2X\n",(int) Reg_IP_Status,(int) Reg_XP_Status,(int) Reg_DRVPWD);
	hdmirxset(0xC9, 0x01, 0x00); // de-assert DR_RST
	chgbank(0);
	#else
		#if (Force_ByteMode == 3)
		hdmirxset(0x6B, 0x03, 0x00);
		iTE6805_DATA.VBO_ByteMode = 0;
		VIDEOTIMNG_DEBUG_PRINTF("Force ByteMode = 3B (variable iTE6805_DATA.VBO_ByteMode = 0)\n");
		#elif (Force_ByteMode == 4)
		hdmirxset(0x6B, 0x03, 0x01);
		iTE6805_DATA.VBO_ByteMode = 1;
		VIDEOTIMNG_DEBUG_PRINTF("Force ByteMode = 4B (variable iTE6805_DATA.VBO_ByteMode = 1)\n");
		#elif (Force_ByteMode == 5)
		hdmirxset(0x6B, 0x03, 0x02);
		iTE6805_DATA.VBO_ByteMode = 2;
		VIDEOTIMNG_DEBUG_PRINTF("Force ByteMode = 5B (variable iTE6805_DATA.VBO_ByteMode = 2)\n");
		#endif
	#endif

	#if (iTE6807 == 1) && (iTE6807_EnSSC == 1)
	iTE6807_Set_EnSSC();
	#endif
#endif

}

void iTE6805_Set_LVDS_Video_Path(iTE_u8 LaneNum)
{
	#if (iTE68051 == TRUE)
	chgbank(1);
	if (LaneNum == 2) {
		hdmirxset(0xC0, BIT0, BIT0); // setting to dual pixel mode
	}
	#endif
	chgbank(5);
	hdmirxset(0xD1, BIT0, LVDSMode);	// RegPixMap
	hdmirxset(0xD1, 0x0C, LVDSColDep);	// RegLVColDep
	hdmirxset(0xDA, BIT4, LVDSSSC << 4);// RegSPENSSC
	hdmirxwr(0xD0, 0xF3); // RegEnLVDS = 1
	chgbank(1);

	switch (LaneNum) {
	case 1:
		hdmirxset(0xBD, 0x30, 0x00); // [5:4] LaneNum
		break;
	case 2:
		hdmirxset(0xBD, 0x30, 0x10); // [5:4] LaneNum
		break;
	case 4:
		hdmirxset(0xBD, 0x30, 0x20); // [5:4] LaneNum
		break;
	default:
		break;
	}

	#if LVDSSwap
	hdmirxwr(0xBE, 0x02);
	hdmirxset(0xFE, 0x10, 0x10);// inverse VGA out clk
	#else
	hdmirxwr(0xBE, 0x00);
	hdmirxset(0xFE, 0x10, 0x10);// inverse VGA out clk
	#endif
	chgbank(0);

}

void iTE6805_Set_TTL_Video_Path(void)
{
	#if (iTE68051 == TRUE)
	chgbank(1);
	hdmirxset(0xC0, 0x06, 0x02);
	// Configure TTL Video Output mode
	// Reg1C1 [0] TTL half bus DDR mode , [1] TTL full bus DDR mode , [5] Enable sync embedded , [6] Enable BTA1004 mode
	// BTA1004 mode must SyncEmb
	switch (iTE6805_DATA.US_Video_Out_Data_Path) {
	case eTTL_SDR:
		hdmirxset(0xC1, BIT1, 0x00);
		break;
	case eTTL_DDR:
		hdmirxset(0xC1, BIT1, BIT1);
		break;
	case eTTL_HalfBusDDR:
		hdmirxset(0xC1, BIT0, BIT0);
		break;
	case eTTL_SDR_BTA1004:
		hdmirxset(0xC1, BIT5|BIT6, BIT5|BIT6);
		break;
	case eTTL_DDR_BTA1004:
		hdmirxset(0xC1, BIT1|BIT5|BIT6, BIT1|BIT5|BIT6);
		break;

    }

	switch (iTE6805_DATA.US_Video_Sync_Mode) {
	case eTTL_SepSync:
		hdmirxset(0xC1, BIT5, 0x00);
		break;
	case eTTL_EmbSync:
		hdmirxset(0xC1, BIT5, BIT5);
		break;
    }
	#endif

	chgbank(0);
	//Set Video Output Color Format
	switch (iTE6805_DATA.US_Output_ColorFormat) {
	case Color_Format_RGB:
		hdmirxset(0x6B, 0x0C, 0x00);
		break;
	case Color_Format_YUV422:
		hdmirxset(0x6B, 0x0C, 0x04);
		break;
	case Color_Format_YUV444:
		hdmirxset(0x6B, 0x0C, 0x08);
		break;
	case Color_Format_YUV420:
		hdmirxset(0x6B, 0x0C, 0x0C);
		break;
	case Color_Format_BYPASS_CSC:
		break;
    }
	// Color Depth Setting depend on input color depth , do not need setting here.
}

void iTE6805_Set_DNScale(void)
{
	iTE_u16 Max_YRGB;
	iTE_u16 Min_YRGB;
	iTE_u16 Max_CRCB;
	iTE_u16 Min_CRCB;

	iTE_u16 Src_Width;
	iTE_u16 Src_Height;
	iTE_u16 Ratio_Denominator;
	iTE_u16 Ratio_Numerator;
	iTE_u16 Ratio_Offset;

	iTE_u16 TG_HSPOL;
	iTE_u16 TG_VSPOL;
	iTE_u16	TG_HFP;
	iTE_u16	TG_HSW;
	iTE_u16	TG_HBP;
	iTE_u16	TG_HDEW;
	iTE_u16	TG_VFP;
	iTE_u16	TG_VSW;
	iTE_u16	TG_VBP;
	iTE_u16	TG_VDEW;
	iTE_u16 Double;
	iTE_u8	GCP_CD;

	//iTE6805_DATA.DumpREG = TRUE;
	// 20160926 reset downscale, and set register value,
	// release reset after register setting
	VIDEOTIMNG_DEBUG_PRINTF("---- Video DownScale Start! ----\n");
	chgbank(0);
	hdmirxset(0x33, 0x08, 0x08);

	if (iTE6805_DATA.Flag_IS_YUV420) {
		Double = 1;
	} else {
		Double = 0;
	}
	Src_Width = (iTE6805_CurVTiming.HActive) << Double;
	Src_Height = iTE6805_CurVTiming.VActive;
	Ratio_Denominator = Src_Width - 2;
	Ratio_Numerator = (Src_Width / 2) - 1;
	Ratio_Offset = (Src_Width / 2) - 1;

	if (iTE6805_DATA.US_Output_ColorFormat == Color_Format_RGB) {
		Max_YRGB = 0xFFF;
		Min_YRGB = 0x000;
		Max_CRCB = 0xFFF;
		Min_CRCB = 0x000;
	} else {
#if (DownScale_YCbCr_Color_Range == 1)
		Max_YRGB = 0xFFF;
		Min_YRGB = 0x000;
		Max_CRCB = 0xFFF;
		Min_CRCB = 0x000;
#else
		Max_YRGB = 940;	// 235*4
		Min_YRGB = 64;	// 16*4
		Max_CRCB = 940;	// 235*4
		Min_CRCB = 64;	// 16*4
#endif
	}

	GCP_CD = ((hdmirxrd(0x98)&0xF0)>>4);

	TG_HSPOL = iTE6805_CurVTiming.HPolarity;
	TG_VSPOL = iTE6805_CurVTiming.HPolarity;
	TG_HFP = ((iTE6805_CurVTiming.HFrontPorch) / 2) << Double;
	TG_HSW = ((iTE6805_CurVTiming.HSyncWidth) / 2) << Double;
	TG_HBP = ((iTE6805_CurVTiming.HBackPorch) / 2) << Double;
	TG_HDEW = ((iTE6805_CurVTiming.HActive) / 2) << Double;

	// for VIC 100 : HDE 4096 to 3840 only
	//	TG_HDEW = 1920;
	//  TG_HBP  = 2200 -  TG_HFP - TG_HSW -1920;
	// end  for HDE 4096 to 3840 only

	TG_VFP = (iTE6805_CurVTiming.VFrontPorch) / 2;
	TG_VSW = (iTE6805_CurVTiming.VSyncWidth) / 2;
	TG_VBP = (iTE6805_CurVTiming.VBackPorch) / 2;
	TG_VDEW = (iTE6805_CurVTiming.VActive) / 2;

#if (ENABLE_4K_MODE_4096x2048_DownScale_To_1920x1080p == TRUE)
		if (TG_HDEW == 2048) {
			TG_HSPOL = 1;
			TG_VSPOL = 1;
			TG_HDEW = 1920 ;
			TG_HSW = 44 ;
			TG_HBP = 148 ;

			TG_HFP = ((iTE6805_CurVTiming.HTotal/2) << Double) - (TG_HBP) - (TG_HSW) - 1920 ;
			Ratio_Numerator = TG_HDEW - 1;
			Ratio_Offset = TG_HDEW - 1;
			VIDEOTIMNG_DEBUG_PRINTF("!!!! New Double = %d !!!!\n", Double);
			VIDEOTIMNG_DEBUG_PRINTF("!!!! TG_HFP = %d !!!!\n", TG_HFP);
			VIDEOTIMNG_DEBUG_PRINTF("!!!! DownScale 2048x1080 to 1920x1080 !!!!\n");
		}
#endif

	VIDEOTIMNG_DEBUG_PRINTF("Target HActive= %d , HFrontPorch=%d, HSync Width=%d, HBackPorch=%d \n", TG_HDEW, TG_HFP, TG_HSW, TG_HBP);
	VIDEOTIMNG_DEBUG_PRINTF("Target VActive= %d , VFrontPorch=%d, VSync Width=%d, VBackPorch=%d \n", TG_VDEW, TG_VFP, TG_VSW, TG_VBP);
	chgbank(5);
	hdmirxwr(0x21, Src_Width & 0xFF);
	hdmirxwr(0x22, (Src_Width & 0xFF00) >> 8);
	hdmirxwr(0x23, Src_Height & 0xFF);
	hdmirxwr(0x24, (Src_Height & 0xFF00) >> 8);
	hdmirxwr(0x25, Ratio_Denominator & 0xFF);
	hdmirxwr(0x26, (Ratio_Denominator & 0xFF00) >> 8);
	hdmirxwr(0x27, Ratio_Numerator & 0xFF);
	hdmirxwr(0x28, (Ratio_Numerator & 0xFF00) >> 8);
	hdmirxwr(0x29, Ratio_Offset & 0xFF);
	hdmirxwr(0x2A, (Ratio_Offset & 0xFF00) >> 8);
	hdmirxwr(0x2B, Max_YRGB & 0xFF);
	hdmirxwr(0x2C, Min_YRGB & 0xFF);
	hdmirxwr(0x2D, ((Min_YRGB & 0xF00) >> 4) + ((Max_YRGB & 0xF00) >> 8));

	hdmirxwr(0x2E, Max_CRCB & 0xFF);
	hdmirxwr(0x2F, Min_CRCB & 0xFF);
	hdmirxwr(0x30, ((Min_CRCB & 0xF00) >> 4) + ((Max_CRCB & 0xF00) >> 8));

	hdmirxwr(0x31, 0x10 + (0x02 << 2) + (TG_HSPOL << 1) + TG_VSPOL); // this color depth force set to 12bits Reg31 2:3
	hdmirxwr(0x32, TG_HFP & 0xFF);
	hdmirxwr(0x33, (TG_HFP & 0x3F00) >> 8);
	hdmirxwr(0x34, TG_HSW & 0xFF);
	hdmirxwr(0x35, (TG_HSW & 0x3F00) >> 8);
	hdmirxwr(0x36, TG_HBP & 0xFF);
	hdmirxwr(0x37, (TG_HBP & 0x3F00) >> 8);
	hdmirxwr(0x38, TG_HDEW & 0xFF);
	hdmirxwr(0x39, (TG_HDEW & 0x3F00) >> 8);
	hdmirxwr(0x3A, TG_VFP & 0xFF);
	hdmirxwr(0x3B, (TG_VFP & 0x3F00) >> 8);
	hdmirxwr(0x3C, TG_VSW & 0xFF);
	hdmirxwr(0x3D, (TG_VSW & 0x3F00) >> 8);
	hdmirxwr(0x3E, TG_VBP & 0xFF);
	hdmirxwr(0x3F, (TG_VBP & 0x3F00) >> 8);
	hdmirxwr(0x40, TG_VDEW & 0xFF);
	hdmirxwr(0x41, (TG_VDEW & 0x3F00) >> 8);
	hdmirxset(0x20, 0x40, 0x00);
	VIDEOTIMNG_DEBUG_PRINTF("---- Video DownScale Enable! Video DownScale Enable! ----\n");
	VIDEOTIMNG_DEBUG_PRINTF("---- Video DownScale End! ----\n");
	chgbank(0);
	hdmirxset(0x33, 0x08, 0x00);
	//iTE6805_DATA.DumpREG = FALSE;
}
// Block Set End


// Blcok Get
void iTE6805_Get_AVIInfoFrame_Info(void)
{
    chgbank(2);
    iTE6805_DATA.AVIInfoFrame_Input_ColorFormat = (AVI_Color_Format) ((hdmirxrd(REG_RX_AVI_DB1)&0x60)>>5);   			// CEA page 43 RGB/YCbCr
    iTE6805_DATA.AVIInfoFrame_Colorimetry = (AVI_Colormetry) ((hdmirxrd(REG_RX_AVI_DB2)&0xC0)>>6); 			// 10=IT709 11=ExtendedColorimetry
    iTE6805_DATA.AVIInfoFrame_ExtendedColorimetry = ((hdmirxrd(REG_RX_AVI_DB3)&0x70)>>4);	// 000=xvYUV601, 001=xvYUV709, 010=sYUV601 ....
    iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange = (AVI_RANGE_Type_RGB) ((hdmirxrd(REG_RX_AVI_DB3)&0x0C)>>2);	// 00 = Depend on Video Format, 01 = Limit, 10 = Full
    iTE6805_DATA.AVIInfoFrame_VIC = (hdmirxrd(REG_RX_AVI_DB4)&0x7F);
    iTE6805_DATA.AVIInfoFrame_YUVQuantizationRange = (AVI_RANGE_Type_YUV) ((hdmirxrd(REG_RX_AVI_DB5)&0xC0)>>6);	// 00 = Limit Range, 01 = Full Range
    iTE6805_DATA.AVIInfoFrame_ScanInfo = (hdmirxrd(REG_RX_AVI_DB1)&0x03);
    chgbank(0);

	if (iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange == RGB_RANGE_DEPENDED_ON_VIC) {
		if (iTE6805_DATA.AVIInfoFrame_VIC >= 2) {
			iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange = RGB_RANGE_LIMIT; // CE Mode
		} else {
			iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange = RGB_RANGE_FULL; // IT mode
		}
	}

	if (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420) {
		iTE6805_DATA.Flag_IS_YUV420 = TRUE;
	} else {
		iTE6805_DATA.Flag_IS_YUV420 = FALSE;
	}

}
// Block Get End


// Block Reset
//hdmirx_ECCTimingOut
void iTE6805_Reset_ALL_Logic(iTE_u8 PORT_NUM)
{
	// REG23	Reg_P0_SWRst		0	1: Port 0 all logic reset	R/W	0
	//			Reg_P0_HDCPRst		1	1: Port 0 HDCP logic reset	R/W	0
	//			Reg_P0_CDRDataRst	2	1: Port 0 CDR Data logic reset	R/W	0
	//			Reg_P0_DCLKRst		3	1: Port 0 CLKD10 domain logic reset	R/W	0


	HDMIRX_DEBUG_PRINT("******* iTE6805_Reset_ALL_Logic *******\n");
	if (PORT_NUM == 0) {
		iTE6805_Set_HPD_Ctrl(PORT0, HPD_LOW);

		chgbank(0);
		hdmirxwr(0x08, 0x04); // port0
		hdmirxwr(0x22, 0x12);
		hdmirxwr(0x22, 0x10);
		hdmirxset(0x23, 0xFD, 0xAC);
		hdmirxset(0x23, 0xFD, 0xA0);

		// Auto Reset when B0
		#if (iTE68051 == TRUE || iTE68052 == TRUE)
		if (iTE6805_DATA.ChipID == 0xA0 || iTE6805_DATA.ChipID == 0xA1) {
			hdmirxset(0xC5, BIT4, BIT4); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
			delay1ms(1);
			hdmirxset(0xC5, BIT4, 0x00); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
		}
		#endif

		iTE6805_INT_5VPWR_Chg(PORT0);
	} else {
		iTE6805_Set_HPD_Ctrl(PORT1, HPD_LOW);

		chgbank(0);
		hdmirxwr(0x0D, 0x04); // port1
		hdmirxwr(0x22, 0x12);
		hdmirxwr(0x22, 0x10);
		hdmirxset(0x2B, 0xFD, 0xAC);
		hdmirxset(0x2B, 0xFD, 0xA0);

		// Auto Reset when B0
		#if (iTE68051 == TRUE || iTE68052 == TRUE)
		if (iTE6805_DATA.ChipID == 0xA0 || iTE6805_DATA.ChipID == 0xA1) {
			chgbank(4);
			hdmirxset(0xC5, BIT4, BIT4); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
			delay1ms(1);
			hdmirxset(0xC5, BIT4, 0x00); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
			chgbank(0);
		}
		#endif
		iTE6805_INT_5VPWR_Chg(PORT1);
    }
	chgbank(0);
}

void iTE6805_Reset_Video_Logic(void)
{
	chgbank(0);
	hdmirxset(0x22, BIT0, BIT0);
	delay1ms(1);
	hdmirxset(0x22, BIT0, 0x00);
	hdmirxset(0x10, BIT1, BIT1); // clear vidstable change INT
	hdmirxset(0x12, BIT7, BIT7); // clear vidstable change INT
}
void iTE6805_Reset_Audio_Logic(void)
{
	iTE_u8 temp;
	chgbank(0);
	// ALL MARK FROM 6802
	// hdmirxset(0x81,0x0c,0x0c);	// enable  Mute i2s and ws and s/pdif
	// delay1ms(100);
	// hdmirxset(0x81,0x0c,0x00);	// disable Mute i2s and ws and s/pdif

	hdmirxset(0x22, BIT1, BIT1); // audio reset
	//delay1ms(1000); // HW ADD , SW MARK
	hdmirxset(0x22, BIT1, 0x00);
	// RegFS_Set[5:0] : Software set sampling frequency R/W
	temp = hdmirxrd(0x8A);
	hdmirxwr(0x8A, temp);
	hdmirxwr(0x8A, temp);
	hdmirxwr(0x8A, temp);
	hdmirxwr(0x8A, temp);
}
void iTE6805_Reset_Video_FIFO(void)
{
	// from 6802 can't find in 6805
	chgbank(1);
	hdmirxset(0x64, 0x80, 0x80); 		// reset Video FIFO's pointer
	hdmirxset(0x64, 0x80, 0x00);
	chgbank(0);
}


// Block Reset End

// Block Check
iTE_u8 iTE6805_Check_Support4KTiming(void)
{
    const iTE6805_SupportVT *vt;
    iTE_u8 i, Double, retry_time = 3;

    if (iTE6805_DATA.Flag_IS_YUV420) {
		Double = 1;
    } else {
		Double = 0;
    }

retry_loop:

    for (i = 0 ; i < (int)SupportVTCount ; i++) {
		vt = &iTE6805_SupportedDSTiming[i];
		if ((vt->HTotal == (iTE6805_CurVTiming.HTotal << Double)) &&
				(vt->HActive == (iTE6805_CurVTiming.HActive << Double)) &&
				(vt->VTotal == iTE6805_CurVTiming.VTotal) &&
				(vt->VActive == iTE6805_CurVTiming.VActive) &&
				(vt->HSyncWidth == (iTE6805_CurVTiming.HSyncWidth << Double)) &&
				(vt->VSyncWidth == iTE6805_CurVTiming.VSyncWidth) &&
				(vt->HFrontPorch == (iTE6805_CurVTiming.HFrontPorch << Double)) &&
				(vt->VFrontPorch == iTE6805_CurVTiming.VFrontPorch) &&
				(vt->HBackPorch  == (iTE6805_CurVTiming.HBackPorch << Double)) &&
				(vt->VBackPorch == iTE6805_CurVTiming.VBackPorch)) {
			return 1;
		}
	}

	if (retry_time && ((iTE6805_CurVTiming.HTotal << Double) > 3000)) {
		retry_time--;
		iTE6805_Get_VID_Info();
		goto retry_loop;
	}

    return 0;
}


iTE_u8 iTE6805_Check_PORT0_IS_MHL_Mode(iTE_u8 PORT_NUM)
{
	chgbank(0);
	//HDMIRX_DEBUG_PRINT("\n\niTE6805_Check_PORT0_IS_MHL_Mode = MODE_HDMI hdmirxrd(0x13)=0x%02X !!!\n", (int) hdmirxrd(0x13));
	if (PORT_NUM == PORT0) {
		return hdmirxrd(0x13)&BIT6;
	} else {
		return MODE_HDMI;
	}

}

iTE_u8 iTE6805_Check_HDMI_OR_DVI_Mode(iTE_u8 PORT_NUM)
{
	chgbank(0);
	if ((PORT_NUM == PORT0 && (hdmirxrd(0x13)&BIT1)) || (PORT_NUM == PORT1 && (hdmirxrd(0x16)&BIT1))) {
		return MODE_HDMI;
	} else {
		return MODE_DVI;
	}
}

iTE_u8 iTE6805_Check_CLK_Vaild(void)
{
	chgbank(0);
	if (iTE6805_DATA.CurrentPort == PORT0) {
		return hdmirxrd(0x13)&BIT3;
	} else {
		return hdmirxrd(0x16)&BIT3;
	}
}

iTE_u8 iTE6805_Check_CLK_Stable(void)
{
	chgbank(0);
	if (iTE6805_DATA.CurrentPort == PORT0) {
		return hdmirxrd(0x13) & BIT4;
	} else {
		return hdmirxrd(0x16) & BIT4;
	}
}

iTE_u8 iTE6805_Check_SCDT(void)
{
	chgbank(0);
	return hdmirxrd(0x19) & BIT7;
}

iTE_u8 iTE6805_Check_AUDT(void)
{
	// REG10[7] : Audio FIFO Error Interrupt
	// REGB1[7] : Audio is On Flag
	chgbank(0);
	return ((!((hdmirxrd(0x10) & 0x80)>>7)) & ((hdmirxrd(0xB1) & 0x80)>>7));	// fix for cross complier warning
}
iTE_u8 iTE6805_Check_AVMute(void)
{
	iTE_u8 temp;
	// REG 0AA[3] Port 0 is in AVMute state
	// REG 4AA[3] Port 1 is in AVMute state
	if (iTE6805_DATA.CurrentPort == PORT0) {
		chgbank(0);
		return hdmirxrd(0xAA) & BIT3;
	} else {
		chgbank(4);
		temp = hdmirxrd(0xAA) & BIT3;
		chgbank(0);
		return temp;
	}
}

iTE_u8 iTE6805_Check_5V_State(iTE_u8 PORT_NUM)
{
	if (PORT_NUM == 0) {
		return hdmirxrd(0x13) & BIT0;
	} else {
		return hdmirxrd(0x16)&BIT0;
	}
}

iTE_u8 iTE6805_Check_Single_Dual_Mode(void)
{
	chgbank(1);
	if (hdmirxrd(0xC0) & BIT0) {
		chgbank(0);
		return MODE_DUAL;
	} else {
		chgbank(0);
		return MODE_SINGLE;
	}
}
iTE_u8 iTE6805_Check_4K_Resolution(void)
{
	iTE6805_Get_PCLK();

	HDMIRX_DEBUG_PRINT("PCLK = %ld.%03ldMHz\n", (long int)(iTE6805_CurVTiming.PCLK/1000), (long int)iTE6805_CurVTiming.PCLK%1000);
	#if (TMDS_162M_LESS_SDR_BIGGER_DDR == TRUE)
	if (iTE6805_CurVTiming.PCLK < 162000) {
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR;
		HDMIRX_DEBUG_PRINT("\n\n---------------------------------PCLK < 162000 , set to SDR Mode\n\n");
	} else {
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		HDMIRX_DEBUG_PRINT("\n\n---------------------------------PCLK >= 162000 , set to DDR Mode\n\n");
	}
	#endif


	if (iTE6805_CurVTiming.PCLK > 320000 ||
			(iTE6805_CurVTiming.PCLK > 160000 && iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420)) {
		HDMIRX_DEBUG_PRINT("iTE6805_Check_4K_Resolution = TRUE!!!\n");
		return TRUE;
	}
	HDMIRX_DEBUG_PRINT("iTE6805_Check_4K_Resolution = FALSE!!!\n");
	return FALSE;
}

iTE_u8 iTE6805_Check_HDMI2(void)
{
	// using clock ratio 1/10 or 1/40 judge is HDMI2.0 or not
	// ADD BIT7 for really HDMI2.0
	iTE_u8 temp ;
	chgbank(0);

	if (iTE6805_DATA.CurrentPort == PORT0) {
		// Edit judge the hdmi2.x from 0xC0 to BIT6 when present 6805 EQ flow
		temp = hdmirxrd(0x14) & BIT6;
		if (temp == BIT6) {
			return TRUE;
		} else {
			return FALSE;
		}

	} else if (iTE6805_DATA.CurrentPort == PORT1) {
		temp = hdmirxrd(0x17) & BIT6;
		if (temp == BIT6) {
			return TRUE;
		} else {
			return FALSE;
		}
	}
	return FALSE ;
}

iTE_u8 iTE6805_Check_EQ_Busy(void)
{
	// using Dbg_AutoStat judge EQ is on trigger now or not (0 mean not busy)
	iTE_u8 EQ_Busy = 0;
	if (iTE6805_DATA.CurrentPort == PORT0)
		chgbank(3);
	if (iTE6805_DATA.CurrentPort == PORT1)
		chgbank(7);
	EQ_Busy = hdmirxrd(0xD4);
	chgbank(0);
	return EQ_Busy;
}

iTE_u8 iTE6805_Check_TMDS_Bigger_Than_1G(void)
{
	chgbank(0);
	if (iTE6805_DATA.CurrentPort == PORT0) {
		return hdmirxrd(0x14) & BIT0;
	} else if (iTE6805_DATA.CurrentPort == PORT1) {
		return hdmirxrd(0x17) & BIT0;
	}
	return FALSE ;
}

iTE_u8 iTE6805_Check_Scramble_State(void)
{
	chgbank(0);
	if (iTE6805_DATA.CurrentPort == PORT0) {
		return hdmirxrd(0x14) & BIT7;
	} else if (iTE6805_DATA.CurrentPort == PORT1) {
		return hdmirxrd(0x17) & BIT7;
	}
	return FALSE ;
}

// Block Check End

#if defined(_6350_EVB_) && _6350_EVB_ == 1
extern void iTE6805_Set_PORT1_HPD(unsigned char temp);
#endif

void iTE6805_Set_HPD_Ctrl(iTE_u16 PORT_NUM, iTE_u16 HPD_State)
{

    HDMIRX_DEBUG_PRINT("iTE6805_Set_HPD_Ctrl PORT%d_HPD = %d \r\n", (int)PORT_NUM, (int)HPD_State);
	// 5V DETECT OFF
	if (PORT_NUM == PORT0 && iTE6805_Check_5V_State(PORT0) == MODE_5V_OFF) {
		chgbank(3);
		hdmirxset(0xAB, 0xC0, 0x00); //SET PORT0 Tri-State
		chgbank(0);
		return;
	}
	// PORT1 is using gpio for HPD low/high
	if (PORT_NUM == PORT1) {
		iTE6805_DATA.Flag_InputMode = MODE_HDMI;
		#if _MCU_8051_EVB_
		if (HPD_State == HPD_LOW) {
			gpHPD1 = PORT1_HPD_OFF;
		} else {
			gpHPD1 = PORT1_HPD_ON;
		}
		#else

		#if defined(_6350_EVB_) && _6350_EVB_ == 1
		if (HPD_State == HPD_LOW) {
			iTE6805_Set_PORT1_HPD(0);
		} else {
			iTE6805_Set_PORT1_HPD(1);
		}
		#endif
		// no MCU, no return PORT1 HPD status.
		#endif
		return;
	}

	// 5V DETECT ON + HDMI MODE
	if (PORT_NUM == PORT0 && iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) == MODE_HDMI) {
		iTE6805_DATA.Flag_InputMode = MODE_HDMI;
		chgbank(3);
		if (HPD_State == HPD_LOW) {
			hdmirxset(0xAB, 0xC0, 0x40); // SET PORT0 HPD LOW
		} else {
			hdmirxset(0xAB, 0xC0, 0xC0); // SET PORT0 HPD HIGH
		}
		hdmirxset(0xAC, 0x7C, 0x40); // Reg_P0_ForceCBUS=1
		chgbank(0);
	}

	// 5V DETECT ON + MHL MODE
	#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
	if (PORT_NUM == PORT0 && iTE6805_Check_PORT0_IS_MHL_Mode(PORT0)) {
		iTE6805_DATA.Flag_InputMode = MODE_MHL;
		if (iTE6805_DATA.MHL_DiscoveryDone == 0) {
			chgbank(3);
			hdmirxset(0xAB, 0xC0, 0x00);
			hdmirxset(0xAC, 0x7C, 0x00); // Reg_P0_ForceCBUS=0
			chgbank(0);
			return;
		}

		HDMIRX_DEBUG_PRINT("MHL HPD Setting\n");
		if (HPD_State == HPD_LOW) {
			iTE6805_MHL_MSC_Fire_With_Retry(MSC_PKT_CLR_HPD);
			HDMIRX_DEBUG_PRINT("MHL_LOW\n");
		} else {
			iTE6805_MHL_MSC_Fire_With_Retry(MSC_PKT_SET_HPD);
			HDMIRX_DEBUG_PRINT("MHL_HIGH\n");
		}
	}
	#endif
	chgbank(0);

}

//IT6802VideoOutputEnable
void iTE6805_Set_Video_Tristate(iTE_u8 TRISTATE_STATE) // TRISTATE_ON, TRISTATE_OFF
{
	chgbank(0);
	#if (COLOR_DEPTH_BY_GCPINFOFRAME == TRUE)
	iTE6805_CurVTiming.ColorDepth = (hdmirxrd(0x98) & 0xF0);
	#endif
	//  6805 no RegTriVDIO 3:1 1: enable tri-state Video IO
    if (TRISTATE_STATE == TRISTATE_OFF) {
		chgbank(1);
		hdmirxset(0xC5, 0x80, 0x00);
		hdmirxset(0xC6, 0x80, 0x00);
		if (iTE6805_Check_Single_Dual_Mode() == MODE_SINGLE) {
			chgbank(1);
			switch (hdmirxrd(0xC0) & 0xC0) {
			case 0x00:
			case 0x40:
			// setting Tristate IO by IO/Mode & Color Depth
#if (COLOR_DEPTH_BY_GCPINFOFRAME == TRUE)
				switch (iTE6805_CurVTiming.ColorDepth) {
				case 0x10:	// 8b
					hdmirxwr(0xC5, 0x78); // turn off tristate If IO mode do not need it, only for single mode
					break;
				case 0x20:	// 10b
					hdmirxwr(0xC5, 0x38); // turn off tristate If IO mode do not need it, only for single mode
					break;
				default:	// 12b+
					hdmirxwr(0xC5, 0x18); // turn off tristate If IO mode do not need it, only for single mode
					break;
				}
#else
				#if (Output_Color_Depth == 8)
				hdmirxwr(0xC5, 0x78); // turn off tristate If IO mode do not need it, only for single mode
				#elif (Output_Color_Depth == 10)
				hdmirxwr(0xC5, 0x38); // turn off tristate If IO mode do not need it, only for single mode
				#elif (Output_Color_Depth == 12)
				hdmirxwr(0xC5, 0x18); // turn off tristate If IO mode do not need it, only for single mode
				#endif
#endif

				break;
			case 0x80:


#if (COLOR_DEPTH_BY_GCPINFOFRAME == TRUE)
				switch (iTE6805_CurVTiming.ColorDepth) {
				case 0x10:	// 8b
					hdmirxwr(0xC5, 0x63); // turn off tristate If IO mode do not need it, only for single mode
					break;
				case 0x20:	// 10b
					hdmirxwr(0xC5, 0x23); // turn off tristate If IO mode do not need it, only for single mode
					break;
				default:	// 12b+
					hdmirxwr(0xC5, 0x03); // turn off tristate If IO mode do not need it, only for single mode
					break;
				}
#else
				// setting Tristate IO by IO/Mode & Color Depth
				#if (Output_Color_Depth == 8)
				hdmirxwr(0xC5, 0x63); // turn off tristate If IO mode do not need it, only for single mode
				#elif (Output_Color_Depth == 10)
				hdmirxwr(0xC5, 0x23); // turn off tristate If IO mode do not need it, only for single mode
				#elif (Output_Color_Depth == 12)
				hdmirxwr(0xC5, 0x03); // turn off tristate If IO mode do not need it, only for single mode
				#endif
#endif

				break;
			}
			//hdmirxwr(0xC5, 0x00);
		} else {
			chgbank(1);
#if (COLOR_DEPTH_BY_GCPINFOFRAME == TRUE)
			switch (iTE6805_CurVTiming.ColorDepth) {
			case 0x10:	// 8b
				hdmirxwr(0xC6, 0x40); // turn off tristate If IO mode do not need it, only for single mode
				break;
			default:	// 10b+
				hdmirxwr(0xC6, 0x00); // turn off tristate If IO mode do not need it, only for single mode
				break;
			}

#else
			#if (Output_Color_Depth == 8)
			hdmirxwr(0xC6, 0x40); // turn off tristate If IO mode do not need it, only for single mode
			#elif (Output_Color_Depth == 10 || Output_Color_Depth == 12)
			hdmirxwr(0xC6, 0x00); // turn off tristate If IO mode do not need it, only for single mode
			#endif
#endif

		}
		HDMIRX_VIDEO_PRINTF("---------------- iTE6805_Set_Video_Tristate -> VIDEO_ON = Tristate off\n");
	} else {
		chgbank(1);
		hdmirxwr(0xC5, 0xFF);
		hdmirxwr(0xC6, 0xFF);
		HDMIRX_VIDEO_PRINTF("---------------- iTE6805_Set_Video_Tristate -> VIDEO_OFF = Tristate on\n");
	}
	chgbank(0);
}

void iTE6805_Set_Audio_Tristate(iTE_u8 TRISTATE_STATE)
{
	chgbank(1);
	if (TRISTATE_STATE == TRISTATE_OFF) {
		hdmirxwr(0xC7, 0x00); // SPDIF/I2S tri-state off
	} else {
		hdmirxwr(0xC7, 0x7F); // SPDIF/I2S tri-state on
	}
	chgbank(0);
}



// ***************************************************************************
// VIC Related Function
// ***************************************************************************
/*
iTE_u16 iTE6805_Get_VIC_Number()
{
	iTE_u16 i;
	for (i = 0; i<(SizeofVMTable - 1); i++) {
		if ((abs(iTE6805_CurVTiming.PCLK - s_VMTable[i].PCLK)>(s_VMTable[i].PCLK * 5 / 100)) ||
			(iTE6805_CurVTiming.HActive != s_VMTable[i].HActive) ||
			//         (iTE6805_CurVTiming.HBackPorch != s_VMTable[i].HBackPorch ) ||
			//         (iTE6805_CurVTiming.HFrontPorch != s_VMTable[i].HFrontPorch ) ||
			//         (iTE6805_CurVTiming.HSyncWidth != s_VMTable[i].HSyncWidth ) ||
			(iTE6805_CurVTiming.HTotal != s_VMTable[i].HTotal) ||
			//         (iTE6805_CurVTiming.HPolarity != s_VMTable[i].HPolarity ) ||
			(iTE6805_CurVTiming.VActive != s_VMTable[i].VActive) ||
			//         (iTE6805_CurVTiming.VBackPorch != s_VMTable[i].VBackPorch ) ||
			//         (iTE6805_CurVTiming.VFrontPorch != s_VMTable[i].VFrontPorch ) ||
			//         (iTE6805_CurVTiming.VSyncWidth != s_VMTable[i].VSyncWidth ) ||
			(iTE6805_CurVTiming.VTotal != s_VMTable[i].VTotal))
			//         (iTE6805_CurVTiming.VPolarity != s_VMTable[i].VPolarity ) ||
			//         (iTE6805_CurVTiming.ScanMode != s_VMTable[i].ScanMode ) )
			continue;
		else
			break;
	}
	return i;
}
*/

void iTE6805_Get_VID_Info(void)
{
	iTE_u16 HSyncPol, VSyncPol, InterLaced;
	iTE_u16 HTotal, HActive, HFP, HSYNCW;
	iTE_u16 VTotal, VActive, VFP, VSYNCW;

	iTE_u32 FrameRate;

	// 0x43 bit5 : P0_WDog RxCLK PreDiv2
	// 0x43 bit6 : P0_WDog RxCLK PreDiv4
	// 0x43 bit7 : P0_WDog RxCLK PreDiv8
	// 0x443 bit5 : P1_WDog RxCLK PreDiv2
	// 0x443 bit6 : P1_WDog RxCLK PreDiv4
	// 0x443 bit7 : P1_WDog RxCLK PreDiv8
	// 0x48	 P0_TMDSCLKSpeed	7:0	TMDSCLK = RCLKVALUE*256/ TMDSCLKSpeed
	// 0x448 P1_TMDSCLKSpeed	7:0	TMDSCLK = RCLKVALUE*256/ TMDSCLKSpeed

	if (iTE6805_DATA.CurrentPort == PORT0) {
		chgbank(0);
	} else {
		chgbank(4);
	}
	iTE6805_Get_TMDS(1);

	iTE6805_Get_PCLK();

	InterLaced = (hdmirxrd(0x98) & 0x02) >> 1;
	HTotal = ((hdmirxrd(0x9C) & 0x3F) << 8) + hdmirxrd(0x9B);
	HActive = ((hdmirxrd(0x9E) & 0x3F) << 8) + hdmirxrd(0x9D);
	HFP = ((hdmirxrd(0xA1) & 0xF0) << 4) + hdmirxrd(0xA0);
	HSYNCW = ((hdmirxrd(0xA1) & 0x01) << 8) + hdmirxrd(0x9F);


	VTotal = ((hdmirxrd(0xA3) & 0x3F) << 8) + hdmirxrd(0xA2);
	VActive = ((hdmirxrd(0xA5) & 0x3F) << 8) + hdmirxrd(0xA4);
	VFP = ((hdmirxrd(0xA8) & 0xF0) << 4) + hdmirxrd(0xA7);
	VSYNCW = ((hdmirxrd(0xA8) & 0x01) << 8) + hdmirxrd(0xA6);

	if (iTE6805_DATA.CurrentPort == PORT1) {
		chgbank(4);
		HDMIRX_VIDEO_PRINTF("hdmirxrd(0xAA) = 0x%02X \n", (int) hdmirxrd(0xAA));
	}

	HSyncPol = (hdmirxrd(0xAA)&BIT5) >> 5;
	VSyncPol = (hdmirxrd(0xAA)&BIT6) >> 6;
	chgbank(0);

	iTE6805_CurVTiming.HActive = HActive;
	iTE6805_CurVTiming.HTotal = HTotal;
	iTE6805_CurVTiming.HFrontPorch = HFP;
	iTE6805_CurVTiming.HSyncWidth = HSYNCW;
	iTE6805_CurVTiming.HBackPorch = (HTotal - HActive - HFP - HSYNCW);
	iTE6805_CurVTiming.VActive = VActive;
	iTE6805_CurVTiming.VTotal = VTotal;
	iTE6805_CurVTiming.VFrontPorch = VFP;
	iTE6805_CurVTiming.VSyncWidth = VSYNCW;
	iTE6805_CurVTiming.VBackPorch = VTotal - VActive - VFP - VSYNCW;
	iTE6805_CurVTiming.ScanMode = (InterLaced) & 0x01;
	iTE6805_CurVTiming.VPolarity = (VSyncPol) & 0x01;
	iTE6805_CurVTiming.HPolarity = (HSyncPol) & 0x01;

	FrameRate = (iTE_u32)(iTE6805_CurVTiming.PCLK) * 1000;
	FrameRate /= iTE6805_CurVTiming.HTotal;
	FrameRate /= iTE6805_CurVTiming.VTotal;
	iTE6805_CurVTiming.FrameRate = FrameRate;

}

void iTE6805_Get_PCLK(void)
{
	iTE_u32 rddata, i;
	iTE_u32 PCLK, sump;
	sump = 0;
	chgbank(0);
	for (i = 0; i < 5; i++) {
		delay1ms(3);
		hdmirxset(0x9A, BIT7, 0x00);
		rddata = ((iTE_u32)(hdmirxrd(0x9A) & 0x03) << 8) + hdmirxrd(0x99);
		hdmirxset(0x9A, BIT7, BIT7);
		sump += rddata;
	}
	PCLK = RCLKVALUE * 512 * 5 / sump; // 512=2*256 because of 1T 2 pixel, *5 = for repeat times
	iTE6805_CurVTiming.PCLK = PCLK;
}

void iTE6805_Get_TMDS(iTE_u8 count)
{
	iTE_u32	sumt = 0;
	iTE_u8 rddata = 0, i;

	if (iTE6805_DATA.CurrentPort == PORT0) {
		chgbank(0);
	} else {
		chgbank(4);
	}

	for (i = 0; i < count; i++) {	// saving time for output
		delay1ms(3);
		rddata = hdmirxrd(0x48) + 1;
		//VIDEOTIMNG_DEBUG_PRINTF("0x48 TMDSCLKSpeed = 0x%02x \n",(int) rddata);
		sumt += rddata;
	}
	//VIDEOTIMNG_DEBUG_PRINTF("sumt = %ld \n", sumt);
	rddata = hdmirxrd(0x43) & 0xE0;
	chgbank(0);

	//HDMIRX_DEBUG_PRINT("RCLKVALUE=%ld.%02ldMHz\n", RCLKVALUE / 1000, (RCLKVALUE % 1000) / 10);
	//HDMIRX_DEBUG_PRINT("rddata=%d \n", (int) rddata);

	if (rddata&BIT7)
		iTE6805_CurVTiming.TMDSCLK = (RCLKVALUE * (iTE_u32)1024 * i) / sumt;
	else if (rddata&BIT6)
		iTE6805_CurVTiming.TMDSCLK = (RCLKVALUE * (iTE_u32)512  * i) / sumt;
	else if (rddata&BIT5)
		iTE6805_CurVTiming.TMDSCLK = (RCLKVALUE * (iTE_u32)256  * i) / sumt;
	if (rddata == 0x00)
		iTE6805_CurVTiming.TMDSCLK = (RCLKVALUE * (iTE_u32)128  * i) / (sumt);

	VIDEOTIMNG_DEBUG_PRINTF("TMDSCLK = %lu.%03luMHz\n", (long unsigned int)iTE6805_CurVTiming.TMDSCLK/1000, (long unsigned int)iTE6805_CurVTiming.TMDSCLK%1000);
}

void iTE6805_GET_InputType(void)
{
	//MODE_HDMI	0x00
	//MODE_MHL	0x01
	//MODE_DVI	0x02
	if (iTE6805_DATA.CurrentPort == PORT0) {
		if (iTE6805_Check_PORT0_IS_MHL_Mode(PORT0)) {
			iTE6805_DATA.Flag_InputMode = MODE_MHL;
		} else {
			if (iTE6805_Check_HDMI_OR_DVI_Mode(iTE6805_DATA.CurrentPort) == MODE_HDMI) {
				iTE6805_DATA.Flag_InputMode = MODE_HDMI;
			} else {
				iTE6805_DATA.Flag_InputMode = MODE_DVI;
			}
		}
	} else {
		if (iTE6805_Check_HDMI_OR_DVI_Mode(iTE6805_DATA.CurrentPort) == MODE_HDMI) {
			iTE6805_DATA.Flag_InputMode = MODE_HDMI;
		} else {
			iTE6805_DATA.Flag_InputMode = MODE_DVI;
		}
	}
}

void iTE6805_Show_VID_Info(void)
{
    int voutstb, lvpclk_hs;

	chgbank(0);
	VIDEOTIMNG_DEBUG_PRINTF("\n\n -------Video Timing-------\n");

	if (iTE6805_DATA.CurrentPort == PORT0) {
		if (hdmirxrd(0x15) & 0x02) {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI2 Scramble Enable !! \n");
		} else {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI2 Scramble Disable !! \n");
		}

		if (hdmirxrd(0x14) & 0x40) {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI2 CLK Ratio 1/40 !! \n");
		} else {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI CLK Ratio 1/10  !! \n");
		}
	} else {
		if (hdmirxrd(0x18) & 0x02) {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI2 Scramble Enable !! \n");
		} else {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI2 Scramble Disable !! \n");
		}

		if (hdmirxrd(0x17) & 0x40) {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI2 CLK Ratio 1/40 !! \n");
		} else {
			VIDEOTIMNG_DEBUG_PRINTF("HDMI CLK Ratio 1/10  !! \n");
		}
	}

    voutstb   = ((hdmirxrd(0x19) & 0x02)>>1);
	lvpclk_hs = ((hdmirxrd(0x36) & 0x0C)>>2);

    //VIDEOTIMNG_DEBUG_PRINTF("Video Input Timing: %s\n", s_VMTable[iTE6805_CurVTiming.VIC].format); // mark because all @#&@)!!$@! can't read ..
    VIDEOTIMNG_DEBUG_PRINTF("TMDSCLK = %lu.%03luMHz\n", (long unsigned int)iTE6805_CurVTiming.TMDSCLK/1000, (long unsigned int)iTE6805_CurVTiming.TMDSCLK%1000);
    VIDEOTIMNG_DEBUG_PRINTF("PCLK = %ld.%03ldMHz\n", (long int)iTE6805_CurVTiming.PCLK/1000, (long int)iTE6805_CurVTiming.PCLK%1000);

	//HSyncWidth,HBackPorch,HActive,HFrontPorch
	//If return to upper layer or show log, need to x = x*2 for YUV420 condition
    if (iTE6805_DATA.Flag_IS_YUV420) {
		VIDEOTIMNG_DEBUG_PRINTF("HActive = %d\n", iTE6805_CurVTiming.HActive*2);
		VIDEOTIMNG_DEBUG_PRINTF("HTotal = %d\n", iTE6805_CurVTiming.HTotal*2);
	} else {
		VIDEOTIMNG_DEBUG_PRINTF("HActive = %d\n", iTE6805_CurVTiming.HActive);
		VIDEOTIMNG_DEBUG_PRINTF("HTotal = %d\n", iTE6805_CurVTiming.HTotal);
	}

    VIDEOTIMNG_DEBUG_PRINTF("VActive = %d\n", iTE6805_CurVTiming.VActive);
	VIDEOTIMNG_DEBUG_PRINTF("VTotal = %d\n", iTE6805_CurVTiming.VTotal);

    if (iTE6805_DATA.Flag_IS_YUV420) {
		VIDEOTIMNG_DEBUG_PRINTF("HFrontPorch = %d\n", iTE6805_CurVTiming.HFrontPorch*2);
		VIDEOTIMNG_DEBUG_PRINTF("HSyncWidth = %d\n", iTE6805_CurVTiming.HSyncWidth*2);
		VIDEOTIMNG_DEBUG_PRINTF("HBackPorch = %d\n", iTE6805_CurVTiming.HBackPorch*2);
	} else {
		VIDEOTIMNG_DEBUG_PRINTF("HFrontPorch = %d\n", iTE6805_CurVTiming.HFrontPorch);
		VIDEOTIMNG_DEBUG_PRINTF("HSyncWidth = %d\n", iTE6805_CurVTiming.HSyncWidth);
		VIDEOTIMNG_DEBUG_PRINTF("HBackPorch = %d\n", iTE6805_CurVTiming.HBackPorch);
	}


    VIDEOTIMNG_DEBUG_PRINTF("VFrontPorch = %d\n", iTE6805_CurVTiming.VFrontPorch);
    VIDEOTIMNG_DEBUG_PRINTF("VSyncWidth = %d\n", iTE6805_CurVTiming.VSyncWidth);
    VIDEOTIMNG_DEBUG_PRINTF("VBackPorch = %d\n", iTE6805_CurVTiming.VBackPorch);
	VIDEOTIMNG_DEBUG_PRINTF("FrameRate = %ld\n", (long int)iTE6805_CurVTiming.FrameRate);

	if (iTE6805_CurVTiming.ScanMode == 0) {
		VIDEOTIMNG_DEBUG_PRINTF("ScanMode = Progressive\n");
	} else {
		VIDEOTIMNG_DEBUG_PRINTF("ScanMode = InterLaced\n");
	}

	if (iTE6805_CurVTiming.VPolarity == 1) {
		VIDEOTIMNG_DEBUG_PRINTF("VSyncPol = Positive\n");
	} else {
		VIDEOTIMNG_DEBUG_PRINTF("VSyncPol = Negative\n");
	}

	if (iTE6805_CurVTiming.HPolarity == 1) {
		VIDEOTIMNG_DEBUG_PRINTF("HSyncPol = Positive\n");
	} else {
		VIDEOTIMNG_DEBUG_PRINTF("HSyncPol = Negative\n");
	}

    if (voutstb == 1) {
		VIDEOTIMNG_DEBUG_PRINTF("Video Output Detect Stable   LVPCLK_HS=%d\n", lvpclk_hs);
	} else {
		VIDEOTIMNG_DEBUG_PRINTF("Video Output Detect Non-Stable   LVPCLK_HS=%d\n", lvpclk_hs);
	}
	VIDEOTIMNG_DEBUG_PRINTF("\n\n");
}

void iTE6805_Show_AUD_Info(void)
{
	iTE_u8 Aud3DOn, MultAudOn, DSDAud, DSDFs, HBRAud, AudFmt, AudCh;
	//iTE_u32 N, CTS;

	Aud3DOn = (hdmirxrd(0xB0) & 0x10) >> 4;
	MultAudOn = (hdmirxrd(0xB0) & 0x20) >> 5;
	HBRAud = (hdmirxrd(0xB0) & 0x40) >> 6;
	DSDAud = (hdmirxrd(0xB0) & 0x80) >> 7;
	AudCh = (hdmirxrd(0xB1) & 0x3F);
	AudFmt = ((hdmirxrd(0xB5) & 0xC0) >> 2) + (hdmirxrd(0xB5) & 0x0F);

	chgbank(2);
	//N = ((iTE_u32)hdmirxrd(0xBE)) << 12;
	//N += ((iTE_u32)hdmirxrd(0xBF)) << 4;
	//N += ((iTE_u32)hdmirxrd(0xC0)) & 0x0F;

	//CTS = ((hdmirxrd(0xC0) & 0xF0)) >> 4;
	//CTS += ((iTE_u32)hdmirxrd(0xC2)) << 4;
	//CTS += ((iTE_u32)hdmirxrd(0xC1)) << 12;
	DSDFs = (hdmirxrd(0x46) & 0x1C) >> 2;
	chgbank(0);

	HDMIRX_AUDIO_PRINTF("\nAudio Format: ");

	if (Aud3DOn == 1) {
		if (DSDAud == 1) {
			HDMIRX_AUDIO_PRINTF("\n One Bit 3D Audio \n");
		} else {
			HDMIRX_AUDIO_PRINTF("\n 3D Audio \n");
		}
	} else if (MultAudOn == 1) {
		if (DSDAud == 1) {
			HDMIRX_AUDIO_PRINTF("\n One Bit Multi-Stream Audio \n");
		} else {
			HDMIRX_AUDIO_PRINTF("\n Multi-Stream Audio \n");
		}
	} else {
		if (DSDAud == 1) {
			HDMIRX_AUDIO_PRINTF("\n One Bit Audio \n");
		} else {
			HDMIRX_AUDIO_PRINTF("\n Audio Sample \n");
		}
	}

	if (HBRAud == 1) {
		HDMIRX_AUDIO_PRINTF("\n High BitRate Audio \n");
	}

	if (HBRAud == 1) {
		HDMIRX_AUDIO_PRINTF("\r\n AudCh=2Ch ");
		iTE6805_DATA.Audio_Channel_Count = 2;
	} else {
		switch (AudCh) {
		case 1:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh=2Ch ");
			iTE6805_DATA.Audio_Channel_Count = 2;
			break;
		case 3:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh=4Ch ");
			iTE6805_DATA.Audio_Channel_Count = 4;
			break;
		case 7:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh=6Ch ");
			iTE6805_DATA.Audio_Channel_Count = 6;
			break;
		case 15:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh=8Ch ");
			iTE6805_DATA.Audio_Channel_Count = 8;
			break;
		case 31:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh=10Ch ");
			iTE6805_DATA.Audio_Channel_Count = 10;
			break;
		case 63:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh=12Ch ");
			iTE6805_DATA.Audio_Channel_Count = 12;
			break;
		default:
			HDMIRX_AUDIO_PRINTF("\r\nAudCh Error !!!\r\n");
			break;
		}
	}

	if (DSDAud == 1) {
		HDMIRX_AUDIO_PRINTF("AudFs=");
		switch (DSDFs) {
		case 0:
			HDMIRX_AUDIO_PRINTF("Refer to Stream Header\n");
			break;
		case 1:
			HDMIRX_AUDIO_PRINTF("32K\n");
			break;
		case 2:
			HDMIRX_AUDIO_PRINTF("44.1K\n");
			break;
		case 3:
			HDMIRX_AUDIO_PRINTF("48K\n");
			break;
		case 4:
			HDMIRX_AUDIO_PRINTF("88.2K\n");
			break;
		case 5:
			HDMIRX_AUDIO_PRINTF("96K\n");
			break;
		case 6:
			HDMIRX_AUDIO_PRINTF("176.4K\n");
			break;
		case 7:
			HDMIRX_AUDIO_PRINTF("192K\n");
			break;
		default:
			HDMIRX_AUDIO_PRINTF("Error !!!\n");
			break;
		}
	} else {
		HDMIRX_AUDIO_PRINTF("AudFs=");
		switch (AudFmt) {
		case 3:
			HDMIRX_AUDIO_PRINTF("=32K\n");
			break;
		case 0:
			HDMIRX_AUDIO_PRINTF("=44.1K\n");
			break;
		case 2:
			HDMIRX_AUDIO_PRINTF("=48K\n");
			break;
		case 11:
			HDMIRX_AUDIO_PRINTF("=64K\n");
			break;
		case 8:
			HDMIRX_AUDIO_PRINTF("=88.2K\n");
			break;
		case 10:
			HDMIRX_AUDIO_PRINTF("=96K\n");
			break;
		case 43:
			HDMIRX_AUDIO_PRINTF("=128K\n");
			break;
		case 12:
			HDMIRX_AUDIO_PRINTF("=176.4K\n");
			break;
		case 14:
			HDMIRX_AUDIO_PRINTF("=192K\n");
			break;
		case 27:
			HDMIRX_AUDIO_PRINTF("=256K\n");
			break;
		case 13:
			HDMIRX_AUDIO_PRINTF("=352.8K\n");
			break;
		case 5:
			HDMIRX_AUDIO_PRINTF("=384K\n");
			break;
		case 59:
			HDMIRX_AUDIO_PRINTF("=512K\n");
			break;
		case 45:
			HDMIRX_AUDIO_PRINTF("=705.6K\n");
			break;
		case 9:
			HDMIRX_AUDIO_PRINTF("=768K\n");
			break;
		case 53:
			HDMIRX_AUDIO_PRINTF("=1024K\n");
			break;
		case 29:
			HDMIRX_AUDIO_PRINTF("=1411.2K\n");
			break;
		case 21:
			HDMIRX_AUDIO_PRINTF("=1536K\n");
			break;
		default:
			HDMIRX_AUDIO_PRINTF(" Error !!!\n");
			break;
		}
	}

	//HDMIRX_AUDIO_PRINTF("N value = %ld , ", N);
	//HDMIRX_AUDIO_PRINTF("CTS value = %ld\n\n", CTS);
}

void iTE6805_Show_AVIInfoFrame_Info(void)
{
	VIDEOTIMNG_DEBUG_PRINTF("AVI Info Frame VIC = %d\n", (int) iTE6805_DATA.AVIInfoFrame_VIC);
	VIDEOTIMNG_DEBUG_PRINTF("AVI Info Frame Input ColorFormat = %X ", (int) iTE6805_DATA.AVIInfoFrame_Input_ColorFormat);
	switch (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat) {
	case Color_Format_RGB:
		VIDEOTIMNG_DEBUG_PRINTF("- RGB\n");
		break;
	case Color_Format_YUV422:
		VIDEOTIMNG_DEBUG_PRINTF("- YUV422\n");
		break;
	case Color_Format_YUV444:
		VIDEOTIMNG_DEBUG_PRINTF("- YUV444\n");
		break;
	case Color_Format_YUV420:
		VIDEOTIMNG_DEBUG_PRINTF("- YUV420\n");
		break;
	default:
		break;
	}

	VIDEOTIMNG_DEBUG_PRINTF("AVI Info Frame Colorimetry = %X", (int) iTE6805_DATA.AVIInfoFrame_Colorimetry);
	switch (iTE6805_DATA.AVIInfoFrame_Colorimetry) {
	case Colormetry_ITU601:
		VIDEOTIMNG_DEBUG_PRINTF(" - ITU601\n");
		break;
	case Colormetry_ITU709:
		VIDEOTIMNG_DEBUG_PRINTF(" - ITU709\n");
		break;
	case Colormetry_Extend:
		VIDEOTIMNG_DEBUG_PRINTF(" - Extend\n");
		break;
	default:
		VIDEOTIMNG_DEBUG_PRINTF(" - UNKNOW\n");
		break;
	}

	VIDEOTIMNG_DEBUG_PRINTF("AVI Info Frame RGBQuantizationRange = %X", (int) iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange);
	switch (iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange) {
	case RGB_RANGE_DEPENDED_ON_VIC:
		VIDEOTIMNG_DEBUG_PRINTF(" - DEPENDED_ON_VIC\n");
		break;
	case RGB_RANGE_LIMIT:
		VIDEOTIMNG_DEBUG_PRINTF(" - RGB_RANGE_LIMIT\n");
		break;
	case RGB_RANGE_FULL:
		VIDEOTIMNG_DEBUG_PRINTF(" - RGB_RANGE_FULL\n");
		break;
	default:
		VIDEOTIMNG_DEBUG_PRINTF(" - UNKNOW\n");
		break;
	}

	VIDEOTIMNG_DEBUG_PRINTF("AVI Info Frame YUVQuantizationRange = %X", (int) iTE6805_DATA.AVIInfoFrame_YUVQuantizationRange);
	switch (iTE6805_DATA.AVIInfoFrame_YUVQuantizationRange) {
	case YUV_RANGE_LIMIT:
		VIDEOTIMNG_DEBUG_PRINTF(" - YUV_RANGE_LIMIT\n");
		break;
	case YUV_RANGE_FULL:
		VIDEOTIMNG_DEBUG_PRINTF(" - YUV_RANGE_FULL\n");
		break;
	default:
		VIDEOTIMNG_DEBUG_PRINTF(" - UNKNOW\n");
		break;
	}

    VIDEOTIMNG_DEBUG_PRINTF("AVI Info Frame ExtendedColorimetry = %X\n", (int) iTE6805_DATA.AVIInfoFrame_ExtendedColorimetry);
}

void chgbank(iTE_u16 bank)
{
	hdmirxset(0x0F, 0x07, bank & 7);
}



void iTE6805_OCLK_Cal(void)
{
	iTE_u32 t1usint, t1usflt;
	iTE_u32 OSCCLK, CPOCLK ;
	iTE_u8	AA, Xp;
	iTE_u32 WCLKValidNum, WCLKHighNum, WCLKHighNumB, WCLKHighNumC;
	iTE_u32 temp;
	iTE_u32 WCLKHighExt;

	#if (iTE6807 == TRUE) && (_ENABLE_IT6805_MHL_FUNCTION_  == TRUE)
	iTE_u8 temp_CPOCLK;
	#endif

    CPOCLK = iTE6805_OCLK_Load() ;

#if (iTE6807 == TRUE)
	HDMIRX_DEBUG_PRINT("CPRCLK=%ld.%03ldMHz\n", (long int)(CPOCLK / 1000), (long int)(CPOCLK % 1000));
#else
	HDMIRX_DEBUG_PRINT("CPOCLK=%ld.%03ldMHz\n", (long int)(CPOCLK / 1000), (long int)(CPOCLK % 1000));
#endif


#if _MCU_8051_EVB_
	OSCCLK = iTE6805_EVB_Ca100ms();
#if (iTE6807 == FALSE)
	HDMIRX_DEBUG_PRINT("Before Adjust, OSCCLK=%ld.%03ldMHz\n", (long int)(OSCCLK / 1000), (long int)(OSCCLK % 1000));
#endif
#endif

	chgbank(3);
	AA = 0x0C;
	HDMIRX_DEBUG_PRINT("adjust AA from 0x0C to 0x%02X\n", (int) AA);
	hdmirxset(0xAA, 0x1f, 0x0C);
	chgbank(0);

#if _MCU_8051_EVB_
	OSCCLK = iTE6805_EVB_Ca100ms();
#if (iTE6807 == FALSE)
	HDMIRX_DEBUG_PRINT("After Adjust, OSCCLK=%ld.%03ldMHz, diff to 39.000MHz .....\n", OSCCLK / 1000, OSCCLK % 1000);
#endif
#endif

#if (iTE6807 == TRUE)
	RCLKVALUE = CPOCLK; // RCLK and OSCCLK is different in 68051/6807
	OSCCLK = RCLKVALUE*2;
	HDMIRX_DEBUG_PRINT("iTE6807 , OSCCLK=%ld.%03ldMHz \n", (long int)(OSCCLK / 1000), (long int)(OSCCLK % 1000));
#else
	OSCCLK = CPOCLK; // assume after 3AA adjust, it must be 41000
	RCLKVALUE = OSCCLK / 2;   // 20 MHz
#endif

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
	RCLKVALUE_HDMI = RCLKVALUE;
#endif

#if (_ENABLE_IT6805_CEC_ == TRUE)
    CEC_timeunit = (iTE_u8)(RCLKVALUE/160);	// use RCLK/160 instead  of OCLK/320 , because timeunit last step came to RCLK
    HDMIRX_DEBUG_PRINT("CEC_timeunit (T100us) = %X\n", (int)CEC_timeunit);
#endif
	temp = RCLKVALUE;
	RCLKVALUE = RCLKVALUE + RCLKVALUE/10;    // 20170327 MHL CTS
	HDMIRX_DEBUG_PRINT("RCLKVALUE=%ld.%02ldMHz\n", (long int)(RCLKVALUE / 1000), (long int)((RCLKVALUE % 1000) / 10));
	t1usint = RCLKVALUE / 1000;
	t1usflt = (RCLKVALUE % 1000) * 256 / 1000;
	hdmirxset(0x91, 0x3F, t1usint & 0x3F);
	hdmirxwr(0x92, t1usflt);
	HDMIRX_DEBUG_PRINT("1us Timer reg91=%02X, reg92=%02X \n", hdmirxrd(0x91), hdmirxrd(0x92));
	RCLKVALUE = temp;    // 20170327 MHL CTS add for CTS (RCLK*=1.1)

	//2017/11/24 add for B0/B1 chip by Emily mail ( B1 Software change: SCDC RR relative register )
	Xp = RCLKVALUE/100; // 40us hold

	#if (iTE68051 == TRUE || iTE68052 == TRUE)
	if (iTE6805_DATA.ChipID == 0xB0) {
		chgbank(3);
		hdmirxwr(0xFA, Xp); // to adjust stop/start hold time
	}
	if (iTE6805_DATA.ChipID == 0xB1) {
		chgbank(1);
		hdmirxwr(0xFD, Xp);
		hdmirxset(0xFE, 0x20, 0x00); // bus free wait time when rr test dly<4 ms
		hdmirxset(0xFE, 0x0F, 0x0C); // 1msto timer (D[3:0]* 80us) <0xC0
		hdmirxset(0xFE, 0x10, 0x10); //  use interanl 10ms timer
		hdmirxset(0xFE, 0x80, 0x80); //  use RR_period
	}
	#endif

	chgbank(0);

	// IT6805Bx/Ax change for Reg0x44h, Reg0x45h, Reg0x46h and Reg0x47h
	WCLKValidNum = OSCCLK/312;
	hdmirxwr(0x45, WCLKValidNum);

	WCLKHighNum =  WCLKValidNum/5;
	hdmirxwr(0x44, WCLKHighNum);

	WCLKHighNumB = OSCCLK/2320;	// 2320.185
	WCLKHighNumC = OSCCLK/5312;	// 5312.084
	temp = (((WCLKHighNumB*100)%100)/50);
	WCLKHighExt = (int) (temp);
	hdmirxwr(0x46, (WCLKHighExt<<6) + ((int) WCLKHighNumB));

	temp = ((WCLKHighNumC*100)%100)/25;
	WCLKHighExt = (int) (temp);
	hdmirxwr(0x47, (WCLKHighExt<<6) + ((int) WCLKHighNumC));

#if (iTE6807 == 1)
	chgbank(5);
	WCLKHighNum =  2560*(OSCCLK/4)/1000000;
	hdmirxwr(0xD3, (int) (WCLKHighNum*2));
	hdmirxwr(0xD4, (int) (WCLKHighNum));
#endif

	chgbank(0);
	HDMIRX_DEBUG_PRINT("read 0x91=0x%02X, \n", (int)hdmirxrd(0x91));
	HDMIRX_DEBUG_PRINT("read 0x92=0x%02X, \n", (int)hdmirxrd(0x92));
	HDMIRX_DEBUG_PRINT("read 0x44=0x%02X, \n", (int)hdmirxrd(0x44));
	HDMIRX_DEBUG_PRINT("read 0x45=0x%02X, \n", (int)hdmirxrd(0x45));
	HDMIRX_DEBUG_PRINT("read 0x46=0x%02X, \n", (int)hdmirxrd(0x46));
	HDMIRX_DEBUG_PRINT("read 0x47=0x%02X, \n", (int)hdmirxrd(0x47));


#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)

	mhlrxset(0x01, 0x70, 4 << 4);

	chgbank(0);
	HDMI_0x91 = hdmirxrd(0x91);
	HDMI_0x92 = hdmirxrd(0x92);
	HDMI_0x44 = hdmirxrd(0x44);
	HDMI_0x45 = hdmirxrd(0x45);
	HDMI_0x46 = hdmirxrd(0x46);
	HDMI_0x47 = hdmirxrd(0x47);

	MHL_AA = 0x0C;

	#if (iTE68051 == TRUE) || (iTE68052 == TRUE)
	if (CPOCLK < 27315) {
		MHL_AA = 0x1F;
	} else if (CPOCLK < 27830) {
		MHL_AA = 0x1E;
	} else if (CPOCLK < 28320) {
		MHL_AA = 0x0D;
	} else if (CPOCLK < 28435) {
		MHL_AA = 0x1C;
	} else if (CPOCLK < 29275) {
		MHL_AA = 0x1B;
	} else if (CPOCLK < 29900) {
		MHL_AA = 0x1A;
	} else if (CPOCLK < 30500) {
		MHL_AA = 0x19;
	} else if (CPOCLK < 30850) {
		MHL_AA = 0x18;
	} else if (CPOCLK < 31510) {
		MHL_AA = 0x17;
	} else if (CPOCLK < 32285) {
		MHL_AA = 0x16;
	} else if (CPOCLK < 33040) {
		MHL_AA = 0x15;
	} else if (CPOCLK < 33685) {
		MHL_AA = 0x14;
	} else if (CPOCLK < 34535) {
		MHL_AA = 0x13;
	} else if (CPOCLK < 35545) {
		MHL_AA = 0x12;
	} else if (CPOCLK < 36535) {
		MHL_AA = 0x11;
	} else if (CPOCLK < 37190) {
		MHL_AA = 0x10;
	} else if (CPOCLK < 38315) {
		MHL_AA = 0x0F;
	} else if (CPOCLK < 39670) {
		MHL_AA = 0x0E;
	} else if (CPOCLK < 41020) {
		MHL_AA = 0x0D;
	} else if (CPOCLK < 42200) {
		MHL_AA = 0x0C;
	} else if (CPOCLK < 43790) {
		MHL_AA = 0x0B;
	} else if (CPOCLK < 45760) {
		MHL_AA = 0x0A;
	} else if (CPOCLK < 47760) {
		MHL_AA = 0x09;
	} else if (CPOCLK < 48930) {
		MHL_AA = 0x08;
	} else if (CPOCLK < 51335) {
		MHL_AA = 0x07;
	} else if (CPOCLK < 54365) {
		MHL_AA = 0x06;
	} else if (CPOCLK < 57575) {
		MHL_AA = 0x05;
	} else if (CPOCLK < 60505) {
		MHL_AA = 0x04;
	} else if (CPOCLK < 64785) {
		MHL_AA = 0x03;
	} else if (CPOCLK < 70470) {
		MHL_AA = 0x02;
	} else {
		MHL_AA = 0x01;
	}
	#else
	temp_CPOCLK = CPOCLK;
	CPOCLK = CPOCLK*2;
	if (CPOCLK < 27315) {
		MHL_AA = 0x1F;
	} else if (CPOCLK < 27830) {
		MHL_AA = 0x1E;
	} else if (CPOCLK < 28320) {
		MHL_AA = 0x0D;
	} else if (CPOCLK < 28435) {
		MHL_AA = 0x1C;
	} else if (CPOCLK < 29275) {
		MHL_AA = 0x1B;
	} else if (CPOCLK < 29900) {
		MHL_AA = 0x1A;
	} else if (CPOCLK < 30500) {
		MHL_AA = 0x19;
	} else if (CPOCLK < 30850) {
		MHL_AA = 0x18;
	} else if (CPOCLK < 31510) {
		MHL_AA = 0x17;
	} else if (CPOCLK < 32285) {
		MHL_AA = 0x16;
	} else if (CPOCLK < 33040) {
		MHL_AA = 0x15;
	} else if (CPOCLK < 33685) {
		MHL_AA = 0x14;
	} else if (CPOCLK < 34535) {
		MHL_AA = 0x13;
	} else if (CPOCLK < 35545) {
		MHL_AA = 0x12;
	} else if (CPOCLK < 36535) {
		MHL_AA = 0x11;
	} else if (CPOCLK < 37190) {
		MHL_AA = 0x10;
	} else if (CPOCLK < 38315) {
		MHL_AA = 0x0F;
	} else if (CPOCLK < 39670) {
		MHL_AA = 0x0E;
	} else if (CPOCLK < 41020) {
		MHL_AA = 0x0D;
	} else if (CPOCLK < 42200) {
		MHL_AA = 0x0C;
	} else if (CPOCLK < 43790) {
		MHL_AA = 0x0B;
	} else if (CPOCLK < 45760) {
		MHL_AA = 0x0A;
	} else if (CPOCLK < 47760) {
		MHL_AA = 0x09;
	} else if (CPOCLK < 48930) {
		MHL_AA = 0x08;
	} else if (CPOCLK < 51335) {
		MHL_AA = 0x07;
	} else if (CPOCLK < 54365) {
		MHL_AA = 0x06;
	} else if (CPOCLK < 57575) {
		MHL_AA = 0x05;
	} else if (CPOCLK < 60505) {
		MHL_AA = 0x04;
	} else if (CPOCLK < 64785) {
		MHL_AA = 0x03;
	} else if (CPOCLK < 70470) {
		MHL_AA = 0x02;
	} else {
		MHL_AA = 0x01;
	}
	MHL_AA--;
	CPOCLK = temp_CPOCLK;
	#endif

	HDMI_CEC_timeunit = CEC_timeunit;
#endif
}

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
void iTE6805_OCLK_Set(iTE_u8 MODE)
{
	chgbank(0);
	if (MODE == MODE_MHL) {
		HDMIRX_DEBUG_PRINT("\n\niTE6805_OCLK_Set = MODE_MHL\n\n");
		hdmirxwr(0x91, 0x56);
		hdmirxwr(0x92, 0x8C);
		hdmirxwr(0x44, 0x1A);
		hdmirxwr(0x45, 0x83);
		hdmirxwr(0x46, 0x11);
		hdmirxwr(0x47, 0x07);

		chgbank(3);
		hdmirxset(0xAA, 0x1f, MHL_AA);
		RCLKVALUE = 20500; // 41000 / 2 = 20500

		// other setting by Andrew mail 20180312
		hdmirxset(0xA6, 0x07, 0x03);
		hdmirxset(0xAE, 0x1C, 0x14);

		chgbank(0);
		hdmirxset(0x24, 0xF8, 0xF8);
		hdmirxset(0x24, 0xF8, 0x00);

		#if (_ENABLE_IT6805_CEC_ == TRUE)
		cecwr(0x0C, 0x80);	// 20500/160 = 0x80
		#endif

	} else {
		HDMIRX_DEBUG_PRINT("\n\niTE6805_OCLK_Set = MODE_HDMI\n\n");
		hdmirxwr(0x91, HDMI_0x91);
		hdmirxwr(0x92, HDMI_0x92);
		hdmirxwr(0x44, HDMI_0x44);
		hdmirxwr(0x45, HDMI_0x45);
		hdmirxwr(0x46, HDMI_0x46);
		hdmirxwr(0x47, HDMI_0x47);
		chgbank(3);
		hdmirxset(0xAA, 0x1f, 0x0C);
		RCLKVALUE = RCLKVALUE_HDMI;

		// other setting by Andrew mail 20180312
		hdmirxset(0xA6, 0x07, 0x04);
		hdmirxset(0xAE, 0x1C, 0x00);
		chgbank(0);

		#if (_ENABLE_IT6805_CEC_ == TRUE)
		cecwr(0x0C, HDMI_CEC_timeunit);
		HDMIRX_DEBUG_PRINT("CEC_timeunit (T100us) = %X\n", (int)HDMI_CEC_timeunit);
		#endif
	}
	HDMIRX_DEBUG_PRINT("RCLKVALUE=%ld.%02ldMHz\n", RCLKVALUE / 1000, (RCLKVALUE % 1000) / 10);
	chgbank(0);
}
#endif

#define iTE6805_OSC_TYPICAL_VALUE 38000
#define iTE6805_OSC_MAX_VALUE ((iTE6805_OSC_TYPICAL_VALUE/4)*5)
#define iTE6805_OSC_MIN_VALUE ((iTE6805_OSC_TYPICAL_VALUE/4)*3)
#define iTE6807_OSC_TYPICAL_VALUE 19000
#define iTE6807_OSC_MAX_VALUE ((iTE6807_OSC_TYPICAL_VALUE/4)*5)
#define iTE6807_OSC_MIN_VALUE ((iTE6807_OSC_TYPICAL_VALUE/4)*3)
iTE_u32 iTE6805_OCLK_Load(void)
{
	iTE_u8 Read_Blcok;
	iTE_u32 lTemp ;
	iTE_u8 temp[4];
	iTE_u8 uc;
	iTE_u8 timeout;
	chgbank(0) ;
	hdmirxwr(0xF8, 0xC3) ;
	hdmirxwr(0xF8, 0xA5) ;
	hdmirxwr(0x34, 0x00) ; // reg34[0] to prevent HW gatting something.

	chgbank(1) ;
	hdmirxwr(0x5F, 0x04) ;
	hdmirxwr(0x5F, 0x05) ;
	hdmirxwr(0x58, 0x12) ;
	hdmirxwr(0x58, 0x02) ;
	uc = hdmirxrd(0x60);
	if (uc != 0x19) {
		hdmirxwr(0xF8, 0xC3) ;
		hdmirxwr(0xF8, 0xA5) ;
		hdmirxwr(0x5F, 0x04) ;
		hdmirxwr(0x58, 0x12) ;
		hdmirxwr(0x58, 0x02) ;
		timeout = 50;
		do {
			uc = hdmirxrd(0x60);
			delay1ms(1);
			timeout--;
		} while ((uc != 0x19) && (timeout != 0));
		delay1ms(10);
		chgbank(0);
		hdmirxset(0xCF, BIT0, BIT0);
		chgbank(1);
	}

	hdmirxwr(0x57, 0x01) ;


	hdmirxwr(0x50, 0x00) ;
	hdmirxwr(0x51, 0x00) ;
	hdmirxwr(0x54, 0x04) ;
	temp[0] = hdmirxrd(0x61) ;
	temp[1] = hdmirxrd(0x62) ;

	hdmirxwr(0x50, 0x00) ;
	hdmirxwr(0x51, 0x01) ;
	hdmirxwr(0x54, 0x04) ;
	temp[2] = hdmirxrd(0x61) ;
	temp[3] = hdmirxrd(0x62) ;

	if (temp[0] == 0xFF && temp[1] == 0xFF && temp[2] == 0x00 && temp[3] == 0x00) {
		Read_Blcok = 0x04;
	} else {
		Read_Blcok = 0x00;
	}

	hdmirxwr(0x50, Read_Blcok) ;
	hdmirxwr(0x51, 0xb0) ;
	hdmirxwr(0x54, 0x04) ;
	// hdmirxbrd(0x61, 4, temp);
	temp[0] = hdmirxrd(0x61) ;
	temp[1] = hdmirxrd(0x62) ;
	hdmirxwr(0x50, Read_Blcok) ;
	hdmirxwr(0x51, 0xb1) ;
	hdmirxwr(0x54, 0x04) ; // trigger read.
	// hdmirxbrd(0x61, 4, temp);
	temp[2] = hdmirxrd(0x61) ;
	temp[3] = hdmirxrd(0x62) ;

	HDMIRX_DEBUG_PRINT("read 0x61=0x%02X, \n", (int)temp[0]);
	HDMIRX_DEBUG_PRINT("read 0x61=0x%02X, \n", (int)temp[1]);
	HDMIRX_DEBUG_PRINT("read 0x61=0x%02X, \n", (int)temp[2]);
	HDMIRX_DEBUG_PRINT("read 0x61=0x%02X, \n", (int)temp[3]);
	lTemp = (iTE_u32)temp[0] ;
	lTemp += ((iTE_u32)temp[1])<<8 ;
	lTemp += ((iTE_u32)temp[2])<<16 ;

	if ((temp[3] & 0xC0) == 0xC0) {
		lTemp /= 100;
	}

	#if (iTE6807 == TRUE)
	if ((lTemp > iTE6807_OSC_MAX_VALUE) || (lTemp < iTE6807_OSC_MIN_VALUE)) {
		HDMIRX_DEBUG_PRINT(" Readback value invalid: 0x%08lX, use typical value instead.\n", (long unsigned int)lTemp);
		lTemp = iTE6807_OSC_TYPICAL_VALUE;
	}
	#else
	if ((lTemp > iTE6805_OSC_MAX_VALUE) || (lTemp < iTE6805_OSC_MIN_VALUE)) {
		HDMIRX_DEBUG_PRINT(" Readback value invalid: 0x%08lX, use typical value instead.\n", (long unsigned int)lTemp);
		lTemp = iTE6805_OSC_TYPICAL_VALUE ;
	}
	#endif
	else {
		HDMIRX_DEBUG_PRINT(" Readback value valid: 0x%08lX(%ldKHz)\n", (long unsigned int)lTemp, (long int)lTemp);
	}

	hdmirxwr(0x5F, 0x00) ;
	chgbank(0) ;
	hdmirxwr(0xF8, 0x00) ;

	return (iTE_u32)lTemp ;
}
/*******************wanpeng add ***************************/
static const struct i2c_board_info it6807_mhl_info = {
	I2C_BOARD_INFO("it6807_mhl", 0x70),
};

static const struct i2c_board_info it6807_edid_info = {
	I2C_BOARD_INFO("it6807_edid", 0x54),
};

static const struct i2c_board_info it6807_cec_info = {
	I2C_BOARD_INFO("it6807_cec", 0x64),
};
/*******************wanpeng add ***************************/


void iTE6805_Init_fsm(void)
{
	iTE6805_DATA.ChipID = hdmirxrd(0x04);
	HDMIRX_DEBUG_PRINT("iTE6805_DATA.ChipID = %X !!!\n", (int) iTE6805_DATA.ChipID);

	#if (ENABLE_DETECT_DRM_PKT == TRUE)
	iTE6805_DATA.Flag_HAVE_DRM_PKT = FALSE;
	iTE6805_DATA.Flag_NEW_DRM_PKT_RECEIVE = FALSE;
	#endif

	iTE6805_DATA._iTE6805_4K_Mode_ = iTE6805_4K60_Mode;

	iTE6805_DATA.STATEV = STATEV_Unplug;
	iTE6805_DATA.STATEA = STATEA_AudioOff;
	iTE6805_DATA.STATEEQ = STATEEQ_Off;
	iTE6805_DATA.DumpREG = FALSE;

	// init table
	hdimrx_write_init(iTE6805_INIT_HDMI_TABLE);

	/*********************************wanpeng add******************************/
	if (mhl_adap == NULL) {
		mhl_adap = i2c_get_adapter(1);
		if (mhl_adap) {
			printinfo("mhl_adap->name=%s\n", mhl_adap->name);
			if (mhl_client == NULL) {
				mhl_client = i2c_new_device(mhl_adap, &it6807_mhl_info);
				if (mhl_client) {
					printinfo("mhl_client->addr=0x%x\n", mhl_client->addr);
				} else {
					printinfo("i2c_new_device fail\n");
				}
			} else {
				printinfo("mhl_client is not null,,,mhl_client->addr=0x%x\n", mhl_client->addr);
			}
		} else {
			printinfo("i2c_get_adapter fail\n");
		}

	} else {
		printinfo("mhl_adap is not null,,,mhl_adap->name=%s\n", mhl_adap->name);
		if (mhl_client == NULL) {
			mhl_client = i2c_new_device(mhl_adap, &it6807_mhl_info);
			if (mhl_client) {
				printinfo("mhl_client->addr=0x%x\n", mhl_client->addr);
			} else {
				printinfo("i2c_new_device fail\n");
			}
		} else {
			printinfo("mhl_client is not null,,,mhl_client->addr=0x%x\n", mhl_client->addr);
		}
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (cec_adap == NULL) {
		cec_adap = i2c_get_adapter(1);
		if (cec_adap) {
			printinfo("cec_adap->name=%s\n", cec_adap->name);
			if (cec_client == NULL) {
				cec_client = i2c_new_device(cec_adap, &it6807_cec_info);
				if (cec_client) {
					printinfo("cec_client->addr=0x%x\n", cec_client->addr);
				} else {
					printinfo("i2c_new_device fail\n");
				}
			} else {
				printinfo("cec_client is not null,,,cec_client->addr=0x%x\n", cec_client->addr);
			}
		} else {
			printinfo("i2c_get_adapter fail\n");
		}

	} else {
		printinfo("cec_adap is not null,,,cec_adap->name=%s\n", cec_adap->name);
		if (cec_client == NULL) {
			cec_client = i2c_new_device(cec_adap, &it6807_cec_info);
			if (cec_client) {
				printinfo("cec_client->addr=0x%x\n", cec_client->addr);
			} else {
				printinfo("i2c_new_device fail\n");
			}
		} else {
			printinfo("cec_client is not null,,,cec_client->addr=0x%x\n", cec_client->addr);
		}
	}
	/*********************************wanpeng add******************************/

	iTE6805_Init_CAOF();
	chgbank(0);
	hdmirxwr(0x28, 0x88); // 0714 MHL CTS need set 0x28 to 0x88
	iTE6805_Set_Video_Tristate(TRISTATE_ON);
	iTE6805_OCLK_Cal();

	// init variable
	iTE6805_DATA.US_CurrentPort_EnableChange = 0;
	iTE6805_DATA.US_Port_Reset_EnableChange = 0;

	#if (_ENABLE_EDID_RAM_ == TRUE)
	// init EDID
	iTE6805_DATA.CurrentPort = 0;	// !!!!!! SOME OS/MCU NOT INIT GLOBAL VARIABLE, NEED TO INIT BY CODE OR MAY INIT ERROR WHEN GOT EDID ARRAY POINTER !!!!!!
	iTE6805_EDID_Init();
	iTE6805_DATA.US_SetEDID_EnableChange = 0;

	/*********************************wanpeng add******************************/
	if (edid_adap == NULL) {
		edid_adap = i2c_get_adapter(1);
		if (edid_adap) {
			printinfo("edid_adap->name=%s\n", edid_adap->name);
			if (edid_client == NULL) {
				edid_client = i2c_new_device(edid_adap, &it6807_edid_info);
				if (edid_client) {
					printinfo("edid_client->addr=0x%x\n", edid_client->addr);
				} else {
					printinfo("i2c_new_device fail\n");
				}
			} else {
				printinfo("edid_client is not null,,,edid_client->addr=0x%x\n", edid_client->addr);
			}
		} else {
			printinfo("i2c_get_adapter fail\n");
		}

	} else {
		printinfo("edid_adap is not null,,,edid_adap->name=%s\n", edid_adap->name);
		if (edid_client == NULL) {
			edid_client = i2c_new_device(edid_adap, &it6807_edid_info);
			if (edid_client) {
				printinfo("edid_client->addr=0x%x\n", edid_client->addr);
			} else {
				printinfo("i2c_new_device fail\n");
			}
		} else {
			printinfo("edid_client is not null,,,edid_client->addr=0x%x\n", edid_client->addr);
		}
	}
	/*********************************wanpeng add******************************/
	#else
	chgbank(0);
	hdmirxset(0xC5, 0x01, 0x01);// Reg_P0_DisableShadow
	chgbank(4);
	hdmirxset(0xC5, 0x01, 0x01);// Reg_P1_DisableShadow
	chgbank(0);
	#endif

	iTE6805_Init_TTL_VideoOutputConfigure();
	iTE6805_Set_TTL_Video_Path();

	#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
	//iTE6805_DATA.DumpREG = TRUE;
	iTE6805_DATA.MHL_DiscoveryDone = 0;
	iTE6805_DATA.MHL_RAP_Content_State = RAP_CONTENT_ON;
	mhlrx_write_init(iTE6805_INIT_MHL_TABLE);
	//iTE6805_DATA.DumpREG = FALSE;
	#endif

	#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
	iTE6805_HDCP_Repeater_INIT();
		#if (_ENABLE_6805_BE_NON_HDCP_REPEATER_WHEN_HDMI_UNPLUG_ == TRUE)
		iTE6805_HDCP_EnableRepeaterMode(0); // enable to non-repeater case
		#else
		iTE6805_HDCP_EnableRepeaterMode(_ENABLE_6805_AS_HDCP_REPEATER_); // will keep stay in repeater mkdir
		#endif
	#endif

	#if (_ENABLE_IT6805_CEC_ == TRUE)
	iTE6805_CEC_INIT();
	#endif

	#if (_ENABLE_EXTERN_EQ_CTRL_ == TRUE)
	iTE6805_DATA.EQ_Customer_Setting[0] = EQ_INIT_VALUE;
	iTE6805_DATA.EQ_Customer_Setting[1] = EQ_INIT_VALUE;
	#endif

	#if (_MCU_8051_EVB_ == FALSE) //EVB_AUTO_DETECT_PORT_BY_PIN
	// main port select
	// check if port 5v on, then that port will be main port
	// if no port have 5v, then main port = port0

    // software pretend need to change port in init stage
    iTE6805_DATA.US_CurrentPort_EnableChange = 1; // trigger
	if (iTE6805_Check_5V_State(PORT0) || !iTE6805_Check_5V_State(PORT1)) {
		iTE6805_DATA.US_CurrentPort = 0;
		iTE6805_DATA.CurrentPort = 1;
		iTE6805_Port_Select_Body(PORT0);
	} else {
		iTE6805_DATA.US_CurrentPort = 1;
		iTE6805_DATA.CurrentPort = 0;
		iTE6805_Port_Select_Body(PORT1);
	}
	#else

    // software pretend need to change port in init stage
    //iTE6805_DATA.US_CurrentPort_EnableChange = 1; // trigger
    //iTE6805_DATA.US_CurrentPort = 1;
    //iTE6805_DATA.CurrentPort = 0;
	//iTE6805_Port_Select_Body(PORT1); //	add for init only EVB board

    iTE6805_DATA.US_CurrentPort_EnableChange = 1; // trigger
    iTE6805_DATA.US_CurrentPort = 0;
    iTE6805_DATA.CurrentPort = 1;
	iTE6805_Port_Select_Body(PORT0);	//	add for init only EVB board
	//iTE6805_Port_Detect();	// port select by MCU pin
	#endif

	#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
	iTE6805_DATA.STATE_HDCP = DYNAMIC_HDCP_ENABLE_DISABLE_INIT_VALUE;
	iTE6805_DATA.STATE_HDCP_FINAL = HDCP_ENABLE;
	iTE6805_HDCP_Detect();
	#endif
	iTE6805_DATA.STATE_HDCP1 = 0;
	iTE6805_DATA.STATE_HDCP2 = 0;

	// add for checking power saving mode
	iTE6805_INT_5VPWR_Chg(iTE6805_DATA.CurrentPort);
}

void iTE6805_Init_TTL_VideoOutputConfigure(void)
{
	switch (eVidOutConfig) {
	case eTTL_SepSync_FullBusSDR_RGB444:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_RGB;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusSDR_YUV444:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV444;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusSDR_YUV422:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV422;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusSDR_BYPASS_CSC:
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusDDR_RGB444:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_RGB;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusDDR_YUV444:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV444;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusDDR_YUV422:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV422;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_FullBusDDR_BYPASS_CSC:
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_HalfBusDDR_RGB444:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_RGB;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_HalfBusDDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_HalfBusDDR_YUV444:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV444;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_HalfBusDDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_SepSync_HalfBusDDR_BYPASS_CSC:
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_HalfBusDDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_SepSync;
		break;

	case eTTL_EmbSync_FullBusSDR_YUV422:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV422;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_EmbSync;
		break;

	case eTTL_EmbSync_FullBusDDR_YUV422:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV422;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_EmbSync;
		break;

	case eTTL_EmbSync_FullBusDDR_BYPASS_CSC:
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_EmbSync;
		break;

	case eTTL_BTA1004_SDR:	//BTA1004_SDR_Emb_Sync
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV422;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_SDR_BTA1004;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_EmbSync;
		break;

	case eTTL_BTA1004_DDR:  //BTA1004_DDR_Emb_Sync
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_YUV422;
		iTE6805_DATA.US_Video_Out_Data_Path = eTTL_DDR_BTA1004;
		iTE6805_DATA.US_Video_Sync_Mode = eTTL_EmbSync;
		break;
	}


	iTE6805_DATA.US_Flag_PYPASS_CSC = FALSE;
	switch (eVidOutConfig) {
	case eTTL_SepSync_FullBusSDR_BYPASS_CSC:
	case eTTL_SepSync_FullBusDDR_BYPASS_CSC:
	case eTTL_SepSync_HalfBusDDR_BYPASS_CSC:
	case eTTL_EmbSync_FullBusDDR_BYPASS_CSC:
		iTE6805_DATA.US_Output_ColorFormat = Color_Format_BYPASS_CSC;
		iTE6805_DATA.US_Flag_PYPASS_CSC = TRUE;
		break;
	case eTTL_SepSync_FullBusDDR_RGB444:
		break;
	}
}

void iTE6805_Init_CAOF(void)
{
	iTE_u8 Reg08h, Reg0Dh;
	iTE_u8 Port0_Status;
	iTE_u8 Port1_Status;
	iTE_u8 Port0_Int;
	iTE_u8 Port1_Int;
	iTE_u8 waitcnt, failcnt;

	chgbank(3);
	hdmirxset(0x3A, 0x80, 0x00); // Reg_CAOFTrg low
	hdmirxset(0xA0, 0x80, 0x80);
	hdmirxset(0xA1, 0x80, 0x80);
	hdmirxset(0xA2, 0x80, 0x80);
	hdmirxset(0xA4, 0x08, 0x08); //IT6805Bx change

	hdmirxset(0x3B, 0xC0, 0x00); // Reg_ENSOF, Reg_ENCAOF
	hdmirxset(0xA7, 0x10, 0x10); // set Reg_PHSELEN high
	hdmirxset(0x48, 0x80, 0x80); // for read back sof value registers
	chgbank(0);
	// Disable MHL AutoPWD
	hdmirxset(0x29, 0x01, 0x01);
	// Inverse COF CLK of Port0, CAOFRST
	hdmirxset(0x2A, 0x41, 0x41);
	delay1ms(10);
	hdmirxset(0x2A, 0x40, 0x00);
	hdmirxset(0x24, 0x04, 0x04); // IPLL RST
	hdmirxwr(0x25, 0x00);        // Disable AFE PWD
	hdmirxwr(0x26, 0x00);
	hdmirxwr(0x27, 0x00);
	hdmirxwr(0x28, 0x00);
	hdmirxset(0x3C, 0x10, 0x00); //disable PLLBufRst
	//----------------------------------------
	// Port 1
	chgbank(7);
	hdmirxset(0x3A, 0x80, 0x00); // Reg_CAOFTrg low
	hdmirxset(0xA0, 0x80, 0x80);
	hdmirxset(0xA1, 0x80, 0x80);
	hdmirxset(0xA2, 0x80, 0x80);
    hdmirxset(0xA4, 0x08, 0x08); //IT6805Bx change

	hdmirxset(0x3B, 0xC0, 0x00); // Reg_ENSOF, Reg_ENCAOF
	hdmirxset(0xA7, 0x10, 0x10); // set Reg_PHSELEN high
	hdmirxset(0x48, 0x80, 0x80); // for read back sof value registers

	chgbank(0);
	hdmirxset(0x32, 0x41, 0x41); // CAOF RST, inverse CAOF CLK
	delay1ms(10);
	hdmirxset(0x32, 0x40, 0x00);
	hdmirxset(0x2C, 0x04, 0x04); // IPLL RST
	hdmirxwr(0x2D, 0x00);        // Disable AFE PWD
	hdmirxwr(0x2E, 0x00);
	hdmirxwr(0x2F, 0x00);
	hdmirxwr(0x30, 0x00);
	chgbank(4);
	hdmirxset(0x3C, 0x10, 0x00); //disable PLLBufRst
	//----------------------------------------
	// Port 0
	chgbank(3);
	hdmirxset(0x3A, 0x80, 0x80); // Reg_CAOFTrg high
	// Port 1
	chgbank(7);
	hdmirxset(0x3A, 0x80, 0x80); // Reg_CAOFTrg high


	// wait for INT Done
	chgbank(0);
	Reg08h = hdmirxrd(0x08) & 0x30;
	Reg0Dh = hdmirxrd(0x0D) & 0x30;
	waitcnt = 0;
	failcnt = 0;
	while (Reg08h == 0x00 || Reg0Dh == 0x00) {
		Reg08h = hdmirxrd(0x08) & 0x30;
		Reg0Dh = hdmirxrd(0x0D) & 0x30;
		HDMIRX_DEBUG_PRINT("Wait for CAOF Done!!!!!!\n");
		HDMIRX_DEBUG_PRINT(" Reg08h= %x,  Reg0Dh=%x .......................\n", (int) hdmirxrd(0x08), (int) hdmirxrd(0x0D));
		if (waitcnt > 4) {
			HDMIRX_DEBUG_PRINT("\n");
			HDMIRX_DEBUG_PRINT("CAOF Fail to Finish!! \n");
			if (Reg08h == 0x00) { // 20170322
				hdmirxset(0x2A, 0x40, 0x40);
				delay1ms(10);
				hdmirxset(0x2A, 0x40, 0x00);
			}
			if (Reg0Dh == 0x00) {
				hdmirxset(0x32, 0x40, 0x40);
				delay1ms(10);
				hdmirxset(0x32, 0x40, 0x00);
			}
			waitcnt = 0;
			failcnt++;
		}
		if (failcnt > 5) {
			HDMIRX_DEBUG_PRINT("CAOF Fail !! \n");
			if (Reg08h == 0x00) {
				chgbank(3);
				hdmirxset(0x3A, 0x80, 0x00);
				chgbank(0);
				hdmirxset(0x2A, 0x40, 0x40);
				delay1ms(10);
				hdmirxset(0x2A, 0x40, 0x00);
			}
			if (Reg0Dh == 0x00) {
				chgbank(7);
				hdmirxset(0x3A, 0x80, 0x00);
				chgbank(0);
				hdmirxset(0x32, 0x40, 0x40);
				delay1ms(10);
				hdmirxset(0x32, 0x40, 0x00);
			}
			break;
		}
		delay1ms(10);
		waitcnt++;
	}
	chgbank(3);
	Port0_Status = (hdmirxrd(0x5A) << 4) + (hdmirxrd(0x59) & 0x0F);
	Port0_Int = hdmirxrd(0x59) & 0xC0;
	chgbank(7);
	Port1_Status = (hdmirxrd(0x5A) << 4) + (hdmirxrd(0x59) & 0x0F);
	Port1_Int = hdmirxrd(0x59) & 0xC0;
	HDMIRX_DEBUG_PRINT("CAOF     CAOF    CAOF     CAOF    CAOF     CAOF\n");
	HDMIRX_DEBUG_PRINT("Port 0 CAOF Int =%x , CAOF Status=%3x\n", Port0_Int, Port0_Status);
	HDMIRX_DEBUG_PRINT("Port 1 CAOF Int =%x , CAOF Status=%3x\n", Port1_Int, Port1_Status);
	// De-assert Port 0
	chgbank(0);

	hdmirxset(0x08, 0x30, 0x30);
	hdmirxset(0x0D, 0x30, 0x30);
	hdmirxset(0x29, 0x01, 0x00); // Enable MHL AutoPWD
	hdmirxset(0x24, 0x04, 0x00); // IPLL RST low
	hdmirxset(0x3C, 0x10, 0x10); //Enable PLLBufRst
	// De-assert Port 1
	hdmirxset(0x2C, 0x04, 0x00); // Port 1 IPLL RST low
	chgbank(4);
	hdmirxset(0x3C, 0x10, 0x10); // Port 1 Enable PLLBufRst

	chgbank(3);
	hdmirxset(0x3A, 0x80, 0x00); // Reg_CAOFTrg low
	hdmirxset(0xA0, 0x80, 0x00);
	hdmirxset(0xA1, 0x80, 0x00);
	hdmirxset(0xA2, 0x80, 0x00);
	chgbank(7);
	hdmirxset(0x3A, 0x80, 0x00); // Reg_CAOFTrg low
	hdmirxset(0xA0, 0x80, 0x00);
	hdmirxset(0xA1, 0x80, 0x00);
	hdmirxset(0xA2, 0x80, 0x00);
	chgbank(0);

}

void iTE6805_Set_47_By_TMDS(void)
{
	// by mail SDI-to-HDMI 4K2K30, need retry IPLL lock again
	iTE_u8 IPLL_HS1P48G, Reg47h, Reg43h, Reg48h, RefSUM, temp;

	chgbank(0);

	Reg43h = hdmirxrd(0x43) & 0xE0;
	Reg47h = hdmirxrd(0x47);
	Reg48h = hdmirxrd(0x48);

	if (Reg43h & BIT7) {
		RefSUM = Reg48h/8;
	} else if (Reg43h & BIT6) {
		RefSUM = Reg48h/4;
	} else if (Reg43h & BIT5) {
		RefSUM = Reg48h/2;
	} else {
		RefSUM = Reg48h;
	}

	if (iTE6805_DATA.CurrentPort == PORT0) {
		IPLL_HS1P48G = hdmirxrd(0x14) & BIT1;
	} else {
		IPLL_HS1P48G = hdmirxrd(0x17)&BIT1;
	}
	temp = RCLKVALUE/1000;
	HDMIRX_DEBUG_PRINT("RefSUM = %d \n", (int) RefSUM);
	HDMIRX_DEBUG_PRINT("RCLKVALUE = %d \n", (int) temp);

	if (RefSUM < temp && IPLL_HS1P48G == 0x00) {
		HDMIRX_DEBUG_PRINT("OverWrite IPLL/OPLL \n");
		Reg47h = hdmirxrd(0x47);
		hdmirxwr(0x47, Reg47h);
	}
}

#if (iTE6807 == 1) && (iTE6807_EnSSC == 1)
void iTE6807_Set_EnSSC(void)
{
	//iTE6805_DATA.VBO_LaneCount
	//iTE6805_DATA.VBO_ByteMode
	//iTE6805_CurVTiming.PCLK

	iTE_u16 result;
	iTE_u32 PCLK =  (iTE6805_CurVTiming.PCLK - (iTE6805_CurVTiming.PCLK%1000));
	PCLK = PCLK / 1000;
	HDMIRX_DEBUG_PRINT("Enable SSC! \n");
	HDMIRX_DEBUG_PRINT("PCLK = %d\n", (int) PCLK);
	HDMIRX_DEBUG_PRINT("VBO_ByteMode = %d\n", (int) iTE6805_DATA.VBO_ByteMode);
	HDMIRX_DEBUG_PRINT("VBO_LaneCount = %d\n", (int) iTE6805_DATA.VBO_LaneCount);
	switch (iTE6805_DATA.VBO_ByteMode) {
	case 0:
		result = 3 * PCLK;
		break;
	case 1:
		result = 4 * PCLK;
		break;
	case 2:
		result = 5 * PCLK;
		break;
	default:
		result = 3 * PCLK;
		break;
	}

	switch (iTE6805_DATA.VBO_LaneCount) {
	case 0:
		result = result / 2;
		break;
	case 1:
		result = result / 4;
		break;
	case 2:
		result = result / 8;
		break;
	case 3:
		result = result / 16;
		break;
	}

	HDMIRX_DEBUG_PRINT("Result = %d \n", (int) result);

	if (result < 46) {
		HDMIRX_DEBUG_PRINT("Result < 46 \n");
		chgbank(5);						//pccmd w 0f 05 90;
		hdmirxset(0xE3, 0x08, 0x00);	//pccmd s e3 08 00 90; :: [3] Reg_Spread : disable
		hdmirxset(0xD0, 0xF0, 0xD0);	//pccmd s d0 f0 d0 90; :: [7] SP_DEI, [6] EnSPBROvWr, [5:4]: SP_BR[1:0]
		hdmirxset(0xD0, 0x0F, 0x0C);	//pccmd s d0 0f 0c 90; :: [3:0] SP_PWDB/SP_RESETB/SoftSPSDMRst/GateSPSDMCLK
		hdmirxset(0xD1, 0x08, 0x00);	//pccmd s d1 08 00 90; :: [3] : SP_BYS
		hdmirxset(0xE3, 0x07, 0x00);	//pccmd s e3 07 00 90; :: [2:0] SDMInc[10:8]
		hdmirxwr(0xE2, 0x0F);			//pccmd w e2 0f 90; :: SDMInc[7:0]
		hdmirxwr(0xE1, 0x09);			//pccmd w e1 09 90; :: ASDM[13:8]
		hdmirxwr(0xE0, 0xE3);			//pccmd w e0 e3 90; :: ASDM[7:0]
		hdmirxset(0xE3, 0x08, 0x08);	//pccmd s e3 08 08 90; :: [3] Reg_Spread : enable

	} else if (result < 60) {
		HDMIRX_DEBUG_PRINT("Result < 60 \n");
		chgbank(5);	//pccmd w 0f 05 90;
		hdmirxset(0xE3, 0x08, 0x00);	//pccmd s e3 08 00 90; :: [3] Reg_Spread : disable
		hdmirxset(0xD0, 0xF0, 0xE0);	//pccmd s d0 f0 e0 90; :: [7] SP_DEI, [6] EnSPBROvWr, [5:4]: SP_BR[1:0]
		hdmirxset(0xD0, 0x0F, 0x0C);	//pccmd s d0 0f 0c 90; :: [3:0] SP_PWDB/SP_RESETB/SoftSPSDMRst/GateSPSDMCLK
		hdmirxset(0xD1, 0x08, 0x00);	//pccmd s d1 08 00 90; :: [3] : SP_BYS
		hdmirxset(0xE3, 0x07, 0x00);	//pccmd s e3 07 00 90; :: [2:0] SDMInc[10:8]
		hdmirxwr(0xE2, 0x05);			//pccmd w e2 05 90; :: SDMInc[7:0]
		hdmirxwr(0xE1, 0x08);			//pccmd w e1 08 90; :: ASDM[13:8]
		hdmirxwr(0xE0, 0xCA);			//pccmd w e0 ca 90; :: ASDM[7:0]
		hdmirxset(0xE3, 0x08, 0x08);	//pccmd s e3 08 08 90; :: [3] Reg_Spread : enable
	} else if (result < 90) {
		HDMIRX_DEBUG_PRINT("Result < 90 \n");
		chgbank(5);	//pccmd w 0f 05 90;
		hdmirxset(0xE3, 0x08, 0x00);	//pccmd s e3 08 00 90; :: [3] Reg_Spread : disable
		hdmirxset(0xD0, 0xF0, 0xE0);	//pccmd s d0 f0 e0 90; :: [7] SP_DEI, [6] EnSPBROvWr, [5:4]: SP_BR[1:0]
		hdmirxset(0xD0, 0x0F, 0x0C);	//pccmd s d0 0f 0c 90; :: [3:0] SP_PWDB/SP_RESETB/SoftSPSDMRst/GateSPSDMCLK
		hdmirxset(0xD1, 0x08, 0x00);	//pccmd s d1 08 00 90; :: [3] : SP_BYS
		hdmirxset(0xE3, 0x07, 0x00);	//pccmd s e3 07 00 90; :: [2:0] SDMInc[10:8]
		hdmirxwr(0xE2, 0x04);			//pccmd w e2 04 90; :: SDMInc[7:0]
		hdmirxwr(0xE1, 0x08);			//pccmd w e1 08 90; :: ASDM[13:8]
		hdmirxwr(0xE0, 0xCA);			//pccmd w e0 ca 90; :: ASDM[7:0]
		hdmirxset(0xE3, 0x08, 0x08);	//pccmd s e3 08 08 90; :: [3] Reg_Spread : enable

	} else if (result < 130) {
		HDMIRX_DEBUG_PRINT("Result < 130 \n");
		chgbank(5);	//pccmd w 0f 05 90;
		hdmirxset(0xE3, 0x08, 0x00);	//pccmd s e3 08 00 90; :: [3] Reg_Spread : disable
		hdmirxset(0xD0, 0xF0, 0xF0);	//pccmd s d0 f0 f0 90; :: [7] SP_DEI, [6] EnSPBROvWr, [5:4]: SP_BR[1:0]
		hdmirxset(0xD0, 0x0F, 0x0C);	//pccmd s d0 0f 0c 90; :: [3:0] SP_PWDB/SP_RESETB/SoftSPSDMRst/GateSPSDMCLK
		hdmirxset(0xD1, 0x08, 0x00);	//pccmd s d1 08 00 90; :: [3] : SP_BYS
		hdmirxset(0xE3, 0x07, 0x00);	//pccmd s e3 07 00 90; :: [2:0] SDMInc[10:8]
		hdmirxwr(0xE2, 0x05);			//pccmd w e2 05 90; :: SDMInc[7:0]
		hdmirxwr(0xE1, 0x09);			//pccmd w e1 09 90; :: ASDM[13:8]
		hdmirxwr(0xE0, 0x18);			//pccmd w e0 18 90; :: ASDM[7:0]
		hdmirxset(0xE3, 0x08, 0x08);	//pccmd s e3 08 08 90; :: [3] Reg_Spread : enable
	} else if (result < 170) {
		HDMIRX_DEBUG_PRINT("Result < 170 \n");
		chgbank(5);	//pccmd w 0f 05 90;
		hdmirxset(0xE3, 0x08, 0x00);	//pccmd s e3 08 00 90; :: [3] Reg_Spread : disable
		hdmirxset(0xD0, 0xF0, 0xF0);	//pccmd s d0 f0 f0 90; :: [7] SP_DEI, [6] EnSPBROvWr, [5:4]: SP_BR[1:0]
		hdmirxset(0xD0, 0x0F, 0x0C);	//pccmd s d0 0f 0c 90; :: [3:0] SP_PWDB/SP_RESETB/SoftSPSDMRst/GateSPSDMCLK
		hdmirxset(0xD1, 0x08, 0x00);	//pccmd s d1 08 00 90; :: [3] : SP_BYS
		hdmirxset(0xE3, 0x07, 0x00);	//pccmd s e3 07 00 90; :: [2:0] SDMInc[10:8]
		hdmirxwr(0xE2, 0x04);			//pccmd w e2 04 90; :: SDMInc[7:0]
		hdmirxwr(0xE1, 0x09);			//pccmd w e1 09 90; :: ASDM[13:8]
		hdmirxwr(0xE0, 0xB3);			//pccmd w e0 b3 90; :: ASDM[7:0]
		hdmirxset(0xE3, 0x08, 0x08);	//pccmd s e3 08 08 90; :: [3] Reg_Spread : enable
	} else {
		HDMIRX_DEBUG_PRINT("Result > 170 \n");
		chgbank(5);	//pccmd w 0f 05 90;
		hdmirxset(0xE3, 0x08, 0x00);	//pccmd s e3 08 00 90; :: [3] Reg_Spread : disable
		hdmirxset(0xD0, 0xF0, 0xF0);	//pccmd s d0 f0 f0 90; :: [7] SP_DEI, [6] EnSPBROvWr, [5:4]: SP_BR[1:0]
		hdmirxset(0xD0, 0x0F, 0x0C);	//pccmd s d0 0f 0c 90; :: [3:0] SP_PWDB/SP_RESETB/SoftSPSDMRst/GateSPSDMCLK
		hdmirxset(0xD1, 0x08, 0x00);	//pccmd s d1 08 00 90; :: [3] : SP_BYS
		hdmirxset(0xE3, 0x07, 0x00);	//pccmd s e3 07 00 90; :: [2:0] SDMInc[10:8]
		hdmirxwr(0xE2, 0x03);			//pccmd w e2 03 90; :: SDMInc[7:0],03
		hdmirxwr(0xE1, 0x09);			//pccmd w e1 09 90; :: ASDM[13:8],09
		hdmirxwr(0xE0, 0x18);			//pccmd w e0 18 90; :: ASDM[7:0],18
		hdmirxset(0xE3, 0x08, 0x08);		//pccmd s e3 08 08 90; :: [3] Reg_Spread : enable
	}
	chgbank(0);
}
#endif

void iTE6805_Set_1B0_By_PixelClock(void)
{
	iTE_u8 temp;
	chgbank(0);
	temp = (hdmirxrd(0x1B) & 0x30)>>4;
	switch (temp) {
	case 0:
		iTE6805_DATA.pixel_repeat = 1 ;
		break;
	case 1:
		iTE6805_DATA.pixel_repeat = 2 ;
		break;
	case 3:
		iTE6805_DATA.pixel_repeat = 4 ;
		break;
	default:
		iTE6805_DATA.pixel_repeat = 1 ;
		break;
	}
	chgbank(1);
	if ((iTE6805_CurVTiming.PCLK/iTE6805_DATA.pixel_repeat) < 25000) {
		hdmirxset(0xB0, BIT0, 0);
	} else {
		hdmirxset(0xB0, BIT0, BIT0);
	}

	if (iTE6805_Check_Single_Dual_Mode() == MODE_SINGLE) {
		chgbank(1);
		hdmirxset(0xB0, BIT0, 0);
	}

	#if (iTE68052 == TRUE)
	//should enable DE regen for 2/4 link
	chgbank(1);
	temp = hdmirxrd(0xBD) & 0x30;
	if (temp != 0x00) {
		hdmirxset(0xB0, 0x01, 0x01);
	}
	#endif

	// add by mail 'IT6807 480P  eye'
	#if (iTE6807 == TRUE)
	chgbank(1);
	// != 1 lane need to setting 1BD[0] = 1
	if (iTE6805_DATA.VBO_LaneCount != 0x00) {
		hdmirxset(0xB0, 0x01, 0x01);
	} else {
		hdmirxset(0xB0, 0x01, 0x00);
	}

	chgbank(0);
	if (iTE6805_Check_TMDS_Bigger_Than_1G()) {
		chgbank(3);
		hdmirxset(0xB0, BIT3, 0x00);
	} else {
		chgbank(3);
		hdmirxset(0xB0, BIT3, BIT3);
	}

	#endif
	chgbank(0);
}

#if defined(_ENABLE_6805_INT_MODE_FUNCTION_) && (_ENABLE_6805_INT_MODE_FUNCTION_ == TRUE)
void iTE6805_Set_INT_Port(void)
{

	chgbank(0);
	hdmirxset(0x60, BIT5, 0);
	if (iTE6805_DATA.CurrentPort == PORT0) {
		// P0 for INT PIN Enable
		hdmirxwr(0x53, 0xFF);	// reg0x05
		hdmirxwr(0x54, 0xFE);	// reg0x06
		hdmirxwr(0x55, 0xFF);	// reg0x07
		hdmirxwr(0x56, 0xFF);	// reg0x08
		hdmirxwr(0x57, 0xFF);	// reg0x09

		// Disable PORT1 INT
		hdmirxwr(0x58, 0x01);	// reg0x0A
		hdmirxwr(0x59, 0x00);	// reg0x0B
		hdmirxwr(0x5A, 0x00);	// reg0x0C
		hdmirxwr(0x5B, 0x00);	// reg0x0D
		hdmirxwr(0x5C, 0x00);	// reg0x0E
	} else {
		// P1 for INT PIN Enable
		hdmirxwr(0x58, 0xFF);
		hdmirxwr(0x59, 0xFE);
		hdmirxwr(0x5A, 0xFF);
		hdmirxwr(0x5B, 0xFF);
		hdmirxwr(0x5C, 0xFF);

		// P0 for INT PIN Disable
		hdmirxwr(0x53, 0x01);	// only 5v detect needed for HPD keeping
		hdmirxwr(0x54, 0x00);
		hdmirxwr(0x55, 0x00);
		hdmirxwr(0x56, 0x00);
		hdmirxwr(0x57, 0x00);
	}
	hdmirxset(0x60, BIT5, BIT5);
}
#endif

#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
void iTE6805_Set_Power_Mode(iTE_u8 Mode)
{
	iTE6805_DATA.CurrentPowerMode = Mode;
	chgbank(0);
	if (Mode == MODE_POWER_SAVING) {
		HDMIRX_DEBUG_PRINT("----------------------------------------MODE_POWER_SAVING\n");
		hdmirxset(0x23, BIT1, BIT1);
		hdmirxset(0x23, BIT1, 0);
		hdmirxset(0x2B, BIT1, BIT1);
		hdmirxset(0x2B, BIT1, 0);

		hdmirxwr(0x25, 0xDF);
		hdmirxwr(0x26, 0xFF);
		hdmirxwr(0x27, 0xFF);
		hdmirxwr(0x2A, 0x3A);
		hdmirxwr(0x2D, 0xFF);
		hdmirxwr(0x2E, 0xFF);
		hdmirxwr(0x2F, 0xFF);
		hdmirxwr(0x32, 0x3E);
		hdmirxset(0xEE, 0x80, 0x00);
		hdmirxwr(0xF8, 0xC3);
		hdmirxwr(0xF8, 0xA5);
		hdmirxwr(0x0F, 0x01);	// chbank
		hdmirxwr(0x5F, 0x04);
		hdmirxwr(0x58, 0x03);
		hdmirxwr(0x0F, 0x03);	// chbank
		hdmirxset(0xA8, 0x08, 0x00);
		//hdmirxset(0xAA, 0x60, 0x20);
		hdmirxset(0xAC, 0x03, 0x03);
		hdmirxwr(0x0F, 0x05);	// chbank

		#if (iTE6807 == 1)
		hdmirxset(0xC0, 0x01, 0x01); //VBOSoftSRst
		hdmirxset(0xC5, 0x30, 0x00); //IP_PWDB&IP_RESETB
		hdmirxset(0xC6, 0xC0, 0x80); //XP_PWDPLL&XP_RESETB
		hdmirxset(0xC9, 0x89, 0x01); //PAT_RSTB&ALPWDB&DRV_RST
		hdmirxwr(0xCA, 0xFF); //DRV_PWD
		hdmirxset(0xCB, 0x40, 0x00); //disable AutoDRVPWD
		hdmirxset(0xCD, 0x20, 0x00); //disable AutoSCLK
		hdmirxwr(0xCE, 0xFF); //GateSCLK
		hdmirxset(0xD0, 0x0F, 0x03); //SP_PWDB&SP_RESETB&SPSDMRst&GateSPSDMCLK
		#else
		hdmirxset(0xC1, 0x03, 0x00);
		hdmirxset(0xC2, 0x03, 0x00);
		hdmirxset(0xC6, 0x20, 0x00);
		hdmirxwr(0xC7, 0x80);
		hdmirxwr(0xC8, 0x00);
		hdmirxwr(0xC9, 0x00);
		hdmirxwr(0xCA, 0x00);
		hdmirxset(0xCB, 0x0F, 0x00);
		#endif

		hdmirxwr(0x0F, 0x07);	// chbank
		hdmirxset(0xA8, 0x08, 0x00);
		hdmirxwr(0x0F, 0x00);
		hdmirxwr(0x21, 0xFB);	//hdmirxwr(0x21, 0xFB);	for Enable CEC  when powersaving , //hdmirxwr(0x21, 0xFF);	for Disable CEC when powersaving
		hdmirxwr(0x20, 0xE9);	// 20181219 fix for EDID need to change even if power saving, can't power down it
		#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
		if (iTE6805_DATA.STATE_HDCP == HDCP_DISABLE) {
			hdmirxset(0x23, BIT1, BIT1);
			hdmirxset(0x2B, BIT1, BIT1);
		}
		#endif

	}

	if (Mode == MODE_POWER_NORMAL) {
		HDMIRX_DEBUG_PRINT("----------------------------------------MODE_POWER_NORMAL\n");
		hdmirxwr(0x25, 0x00);
		// only turn on the current port power
		if (iTE6805_DATA.CurrentPort == PORT0) {
			hdmirxwr(0x26, 0x00);
			hdmirxwr(0x27, 0x00);
			hdmirxwr(0x2A, 0x01);
			hdmirxwr(0x0F, 0x03);	// chbank
			hdmirxset(0xA8, 0x08, 0x08);
		} else {
			hdmirxwr(0x2D, 0x00);
			hdmirxwr(0x2E, 0x00);
			hdmirxwr(0x2F, 0x00);
			hdmirxwr(0x32, 0x01);
			hdmirxwr(0x0F, 0x07);	// chbank
			hdmirxset(0xA8, 0x08, 0x08);	//same
		}

		chgbank(0);
		hdmirxset(0xEE, 0x80, 0x80);
		hdmirxwr(0xF8, 0xC3);
		hdmirxwr(0xF8, 0xA5);
		hdmirxwr(0x0F, 0x01);	// chbank
		hdmirxwr(0x5F, 0x04);
		hdmirxwr(0x58, 0x02);
		hdmirxwr(0x0F, 0x03);	// chbank
		hdmirxset(0xAA, 0x60, 0x60);
		hdmirxset(0xAC, 0x03, 0x00);
		hdmirxwr(0x0F, 0x05);	// chbank

		#if (iTE6807 == 1)
		hdmirxset(0xC0, 0x01, 0x00); //VBOSoftSRst
		hdmirxset(0xC5, 0x30, 0x30); //IP_PWDB&IP_RESETB
		hdmirxset(0xC6, 0xC0, 0x40); //XP_PWDPLL&XP_RESETB
		hdmirxset(0xC9, 0x89, 0x88); //PAT_RSTB&ALPWDB&DRV_RST
		hdmirxwr(0xCA, 0x00); //DRV_PWD
		hdmirxset(0xCB, 0x40, 0x40); //disable AutoDRVPWD
		hdmirxset(0xCD, 0x20, 0x20); //disable AutoSCLK
		hdmirxwr(0xCE, 0x00); //GateSCLK
		hdmirxset(0xD0, 0x0F, 0x0C); //SP_PWDB&SP_RESETB&SPSDMRst&GateSPSDMCLK
		#else
		hdmirxset(0xC1, 0x03, 0x00); // same
		hdmirxset(0xC2, 0x03, 0x00); // same
		hdmirxset(0xC6, 0x20, 0x20);
		hdmirxwr(0xC7, 0x00);
		hdmirxwr(0xC8, 0x00);	//same
		hdmirxwr(0xC9, 0x00);	//same
		hdmirxwr(0xCA, 0x00);	//same
		hdmirxset(0xCB, 0x0F, 0x00);	//same
		#endif

		hdmirxwr(0x0F, 0x00);	// chbank
		hdmirxwr(0x21, 0x40);
		hdmirxwr(0x20, 0x00);
	}

	if (Mode == MODE_POWER_STANDBY) {
		HDMIRX_DEBUG_PRINT("----------------------------------------MODE_POWER_STANDBY\n");

		#if (ENABLE_6805_POWER_DETECT_ONLY_TMDS_CLOCK_POWER_ON == TRUE)

		hdmirxwr(0x25, 0xD0);
		if (iTE6805_DATA.CurrentPort == PORT0) {
			hdmirxwr(0x26, 0x7E);
			hdmirxwr(0x27, 0xFE);
			hdmirxwr(0x2A, 0x01);
			hdmirxwr(0x0F, 0x03);	// chbank
			hdmirxset(0xA8, 0x08, 0x08);
		} else {
			hdmirxwr(0x2D, 0xD0);
			hdmirxwr(0x2E, 0x7E);
			hdmirxwr(0x2F, 0xFE);
			hdmirxwr(0x32, 0x01);
			hdmirxwr(0x0F, 0x07);	// chbank
			hdmirxset(0xA8, 0x08, 0x08);	//same
		}

		hdmirxwr(0x0F, 0x03);	// chbank
		hdmirxset(0xAA, 0x60, 0x60);
		hdmirxset(0xAC, 0x03, 0x00);
		//hdmirxwr(0x0F, 0x05);	// chbank
		//hdmirxset(0xC1, 0x03, 0x00); // same
		//hdmirxset(0xC2, 0x03, 0x00); // same
		//hdmirxset(0xC6, 0x20, 0x20);
		//hdmirxwr(0xC7, 0x00);
		//hdmirxwr(0xC8, 0x00);	//same
		//hdmirxwr(0xC9, 0x00);	//same
		//hdmirxwr(0xCA, 0x00);	//same
		//hdmirxset(0xCB, 0x0F, 0x00);	//same
		hdmirxwr(0x0F, 0x00);	// chbank
		hdmirxwr(0x21, 0x40);
		hdmirxwr(0x20, 0x00);

		#else

		hdmirxwr(0x25, 0x00);
		if (iTE6805_DATA.CurrentPort == PORT0) {
			hdmirxwr(0x26, 0x00);
			hdmirxwr(0x27, 0x00);
			hdmirxwr(0x2A, 0x01);
			hdmirxwr(0x0F, 0x03);	// chbank
			hdmirxset(0xA8, 0x08, 0x08);
		} else {
			hdmirxwr(0x2D, 0x00);
			hdmirxwr(0x2E, 0x00);
			hdmirxwr(0x2F, 0x00);
			hdmirxwr(0x32, 0x01);
			hdmirxwr(0x0F, 0x07);	// chbank
			hdmirxset(0xA8, 0x08, 0x08);	//same
		}

		chgbank(0);
		hdmirxset(0xEE, 0x80, 0x80);
		//hdmirxwr(0xF8, 0xC3);
		//hdmirxwr(0xF8, 0xA5);
		//hdmirxwr(0x0F, 0x01);	// chbank
		//hdmirxwr(0x5F, 0x04);
		//hdmirxwr(0x58, 0x02);
		hdmirxwr(0x0F, 0x03);	// chbank
		hdmirxset(0xAA, 0x60, 0x60);
		hdmirxset(0xAC, 0x03, 0x00);
		//hdmirxwr(0x0F, 0x05);	// chbank
		//hdmirxset(0xC1, 0x03, 0x00); // same
		//hdmirxset(0xC2, 0x03, 0x00); // same
		//hdmirxset(0xC6, 0x20, 0x20);
		//hdmirxwr(0xC7, 0x00);
		//hdmirxwr(0xC8, 0x00);	//same
		//hdmirxwr(0xC9, 0x00);	//same
		//hdmirxwr(0xCA, 0x00);	//same
		//hdmirxset(0xCB, 0x0F, 0x00);	//same

		hdmirxwr(0x0F, 0x00);	// chbank
		hdmirxwr(0x21, 0x40);
		hdmirxwr(0x20, 0x00);

		#endif

	}
}
#endif

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
void iTE6805_Check_MHL_Mode_Change_Need_Power_On(void)
{
	iTE_u8 Reg07h;

	if (iTE6805_DATA.CurrentPort == PORT1) {
		return;
	}

	chgbank(0);
	Reg07h = hdmirxrd(0x07);
	hdmirxwr(0x07, Reg07h & 0x08); // Bus Mode Change detect here
	if (Reg07h & 0x08) {
		HDMIRX_DEBUG_PRINT("# Port 0 HDMI Bus Mode Change #\n");
		chgbank(0);
		if (hdmirxrd(0x13) & 0x40) {
			HDMIRX_DEBUG_PRINT("# Port 0 Bus Mode : MHL #\n");
			#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
			iTE6805_Set_Power_Mode(MODE_POWER_NORMAL);
			#endif

			iTE6805_OCLK_Set(MODE_MHL);
		} else {
			HDMIRX_DEBUG_PRINT("# Port 0 Bus Mode : HDMI #\n");
			iTE6805_OCLK_Set(MODE_HDMI);
		}
	}
}
#endif



#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
void iTE6805_Set_HDCP(iTE_u8 HDCP_STATE)
{
	if (HDCP_STATE == HDCP_ENABLE) {
		// enable HDCP2
		chgbank(0);
		hdmirxset(0xE2, BIT0, BIT0);

		// enable HDCP1
		chgbank(0);
		hdmirxset(0xCB, BIT2, 0x00);
		chgbank(4);
		hdmirxset(0xCB, BIT2, 0x00);

		chgbank(1);
		hdmirxwr(0xAC, 0x2A);
		hdmirxwr(0xAD, 0xA5);

		chgbank(0);
		hdmirxset(0xCB, BIT2, BIT2);
		chgbank(4);
		hdmirxset(0xCB, BIT2, BIT2);

		chgbank(0);
		hdmirxset(0xCF, BIT2, BIT2);
		chgbank(4);
		hdmirxset(0xCF, BIT2, BIT2);

		chgbank(0);
		hdmirxset(0x23, BIT1, BIT1);
		hdmirxset(0x2B, BIT1, BIT1);

		hdmirxset(0x23, BIT1, 0x00);
		hdmirxset(0x2B, BIT1, 0x00);
	} else {
		// disable HDCP2
		chgbank(0);
		hdmirxset(0xE2, BIT0, 0x00);

		// disable HDCP1
		chgbank(1);
		hdmirxwr(0xAC, 0x00);
		hdmirxwr(0xAD, 0x00);

		chgbank(0);
		hdmirxset(0xCB, BIT2, BIT2);
		chgbank(4);
		hdmirxset(0xCB, BIT2, BIT2);

		chgbank(0);
		hdmirxset(0xCF, BIT2, BIT2);
		chgbank(4);
		hdmirxset(0xCF, BIT2, BIT2);

		chgbank(0);
		hdmirxset(0x23, BIT1, BIT1);
		hdmirxset(0x2B, BIT1, BIT1);
		hdmirxset(0x23, BIT1, 0x00);
		hdmirxset(0x2B, BIT1, 0x00);

		chgbank(0);
		hdmirxset(0xCB, BIT2, 0x00);
		chgbank(4);
		hdmirxset(0xCB, BIT2, 0x00);
		chgbank(0);

		// set 0x74 cant read
		hdmirxset(0x23, BIT1, BIT1);
		hdmirxset(0x2B, BIT1, BIT1);
	}
	chgbank(0);
}
#endif
