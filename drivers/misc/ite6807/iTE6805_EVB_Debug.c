///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_EVB_Debug.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/30
//   @fileversion: iTE6805_IT6350_SAMPLE_1.43
//******************************************/
#include "iTE6805_Global.h"

#if _MCU_8051_EVB_ || (defined(_6350_EVB_) && _6350_EVB_ == 1)

extern _iTE6805_DATA	iTE6805_DATA;

#if (defined(_6350_EVB_) && _6350_EVB_ == 1)
#define EDID_WP0 (GPDRA & 0x02)
#define EDID_WP1 (GPDRA & 0x40)
#endif

#if _MCU_8051_EVB_
iTE_u32 iTE6805_EVB_Ca100ms(void)
{
	iTE_u32 OSCCLK, rddata;
	mhlrxwr(0x01, 0x41); //Hold_Pin=0;
	delay1ms(99);
	mhlrxwr(0x01, 0x40); //Hold_Pin=1;
	rddata = mhlrxrd(0x12);
	rddata += ((iTE_u32)mhlrxrd(0x13)) << 8;
	rddata += ((iTE_u32)mhlrxrd(0x14)) << 16;

	OSCCLK = rddata / 100;
	return OSCCLK;
}
#endif

#if (iTE68051 == TRUE && _MCU_8051_EVB_ == TRUE)
#define iTE6028_DE_Delay 0xD1 // can be D0~D3, De Delay Level D3>D2>D1>D0
void iTE6805_Init_6028LVDS(iTE_u8 chip)
{
	HDMIRX_DEBUG_PRINT("iTE6805_Init_6028LVDS\r\n");
// kuro
// w 07 1f b4
// w 07 1f b4
// w 07 1e b4
// w 37 50 b4
// w 2e 99 b4
// w 0a 34 b4
// w 0b ff b4
// w 08 D0 b4
// w 09 21 b4
// w 2f 02 b4
// w 07 00 b4

#if _MCU_8051_EVB_
	if (chip == 0)
		gp6028 = 1;
	else
		gp6028 = 0;
#endif
	lvdsrxwr(0x07, 0x1f);
	lvdsrxwr(0x07, 0x1f);
	lvdsrxwr(0x07, 0x1e);
	lvdsrxwr(0x37, 0x50);
	lvdsrxwr(0x2e, 0x99);

	if (iTE6805_DATA.US_Video_Out_Data_Path == eTTL_SDR) {
		lvdsrxwr(0x0a, 0x64);
	} else {
		lvdsrxwr(0x0a, 0x34);
	}
	lvdsrxwr(0x0b, 0xff);

	lvdsrxwr(0x08, iTE6028_DE_Delay);

	lvdsrxwr(0x09, 0x21);
	lvdsrxwr(0x2f, 0x02);
	lvdsrxwr(0x07, 0x00);
	#if _MCU_8051_EVB_
	gp6028 = 1;
	#endif
}
#endif


#if (_MCU_8051_EVB_ == TRUE)
iTE_u8 LAST_PORT = 3;
void iTE6805_Port_Detect(void)
{
	if (PORT_SWITCH == LAST_PORT) {
		return;
	}

	if (PORT_SWITCH != 0) {
		iTE6805_Port_Select(PORT0);
		//iTE6805_Reset_ALL_Logic(PORT0);
	} else {
		iTE6805_Port_Select(PORT1);
		//iTE6805_Reset_ALL_Logic(PORT1);
	}

	LAST_PORT = PORT_SWITCH;
}

#endif

#if (_MCU_8051_EVB_ == TRUE)
void iTE6805_EVB_4K_SET_BY_PIN(void)
{
	if (EDID_WP0 == 1) {
		if (EDID_WP1 == 0) {
			iTE6805_DATA._iTE6805_4K_Mode_ = MODE_EvenOdd;
			iTE6805_DATA._iTE6807_EnableTwoSectionMode_ = 1;
		} else {
			iTE6805_DATA._iTE6805_4K_Mode_ = MODE_LeftRight;
			iTE6805_DATA._iTE6807_EnableTwoSectionMode_ = 0;
		}
	} else {
		iTE6805_DATA._iTE6805_4K_Mode_ = MODE_DownScale;
	}
}
#endif

#if (defined(_6350_EVB_) && _6350_EVB_ == 1)
void iTE6805_EVB_4K_SET_BY_PIN(void)
{
	if (EDID_WP0) {
		if (!EDID_WP1) {
			iTE6805_DATA._iTE6805_4K_Mode_ = MODE_EvenOdd;
			iTE6805_DATA._iTE6807_EnableTwoSectionMode_ = 1;
		} else {
			iTE6805_DATA._iTE6805_4K_Mode_ = MODE_LeftRight;
			iTE6805_DATA._iTE6807_EnableTwoSectionMode_ = 0;
		}
	} else {
		iTE6805_DATA._iTE6805_4K_Mode_ = MODE_DownScale;
	}
}
#endif

#endif// _MCU_8051_EVB_
