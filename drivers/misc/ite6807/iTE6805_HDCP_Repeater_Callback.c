///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_HDCP_Repeater_Callback.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#include "iTE6805_Global.h"

#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)

extern _iTE6805_DATA	iTE6805_DATA;


void iTE6805_Inform_HPD_Unplug_CB(void)
{
	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_HPD_Unplug_CB called \r\n");

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

#if (_ENABLE_6805_BE_NON_HDCP_REPEATER_WHEN_HDMI_UNPLUG_ == TRUE)
	iTE6805_Inform_HDCP_EnableRepeaterMode(0);
#endif

	// software trigger
	iTE6805_DATA.US_Port_Reset_EnableChange = 1;
	iTE6805_Port_Reset_Body(iTE6805_DATA.CurrentPort);

	iTE6805_HDCP_Repeater_chg(STATER_6805_Wait_For_HDCP_Done);
}


void iTE6805_Inform_HDCP1_KSVListReady_CB(unsigned char *BKSV, unsigned char *BStatus, unsigned char *KSV_List)
{
	iTE_u8 i, dev_count, depth;

	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_HDCP1_KSVListReady_CB called \r\n");

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

	chgbank(1);
	hdmirxwr(0x28, BKSV[0]);
	hdmirxwr(0x29, BKSV[1]);
	hdmirxwr(0x2A, BKSV[2]);
	hdmirxwr(0x2B, BKSV[3]);
	hdmirxwr(0x2C, BKSV[4]);

	hdmirxwr(0x10, BStatus[0]);
	hdmirxwr(0x11, BStatus[1]);
	HDCP_DEBUG_PRINTF("BStatus[0] = 0x%02X \r\n", (int)BStatus[0]);
	HDCP_DEBUG_PRINTF("BStatus[1] = 0x%02X \r\n", (int)BStatus[1]);

	dev_count =  BStatus[0]&0x7F;
	depth = BStatus[1]&0x07;
	HDCP_DEBUG_PRINTF("KSV_List addr = 0x%X \r\n", KSV_List);
	if (dev_count != 0) {
		// update KSV List
		chgbank(1);
		for (i = 0; (i < dev_count) && i < 7; i++) {
			// 680X only support 7KSV List
			HDCP_DEBUG_PRINTF("i = %d \r\n", (int) i);
			hdmirxwr(0x2D + (i*5), *(KSV_List + (i*5 + 0)));
			HDCP_DEBUG_PRINTF("KSVList[i*5+0] = 0x%02X \r\n", *(KSV_List+(i*5 + 0)));
			hdmirxwr(0x2E + (i*5), *(KSV_List + (i*5 + 1)));
			HDCP_DEBUG_PRINTF("KSVList[i*5+1] = 0x%02X \r\n", *(KSV_List+(i*5 + 1)));
			hdmirxwr(0x2F + (i*5), *(KSV_List+(i*5 + 2)));
			HDCP_DEBUG_PRINTF("KSVList[i*5+2] = 0x%02X \r\n", *(KSV_List+(i*5 + 2)));
			hdmirxwr(0x30 + (i*5), *(KSV_List + (i*5 + 3)));
			HDCP_DEBUG_PRINTF("KSVList[i*5+3] = 0x%02X \r\n", *(KSV_List+(i*5 + 3)));
			hdmirxwr(0x31 + (i*5), *(KSV_List + (i*5 + 4)));
			HDCP_DEBUG_PRINTF("KSVList[i*5+4] = 0x%02X \r\n", *(KSV_List+(i*5 + 4)));
		}

		// exceeded maxmium dev count
		if (dev_count >= 0x07) {
			hdmirxset(0x10, BIT7, BIT7);
		}

		// exceeded maxmium depth
		if (depth >= 0x06) {
			hdmirxset(0x11, BIT3, BIT3);
		}

		// update KSV(dev) count in BStatus
		hdmirxset(0x10, 0x7F, dev_count + 1);	// set dev count
		hdmirxset(0x11, 0x07, depth + 1);	// set dev count
		chgbank(0);
	}
	iTE6805_HDCP_Repeater_chg(STATER_6805_HDCP_PrepareSendReady);
}

void iTE6805_Inform_HDCP2_KSVListReady_CB(unsigned char *RxID, unsigned char *RxInfo,  unsigned char *RxID_List)
{
	iTE_u8 i, dev_count, depth;

	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_HDCP2_KSVListReady_CB called \r\n");

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

	chgbank(1);

	hdmirxwr(0x28, RxID[0]);
	hdmirxwr(0x29, RxID[1]);
	hdmirxwr(0x2A, RxID[2]);
	hdmirxwr(0x2B, RxID[3]);
	hdmirxwr(0x2C, RxID[4]);

	hdmirxwr(0xE4, RxInfo[0]);
	hdmirxwr(0xE5, RxInfo[1]);
	HDCP_DEBUG_PRINTF("RxInfo[0] = 0x%02X \r\n", (int)RxInfo[0]);
	HDCP_DEBUG_PRINTF("RxInfo[1] = 0x%02X \r\n", (int)RxInfo[1]);

	dev_count =  (RxInfo[0]&0xF0 >> 4) + ((RxInfo[1]&0x01)>>4);
	depth = (RxInfo[1]&0x0E)>>1;
	HDCP_DEBUG_PRINTF("RxID_List addr = 0x%X \r\n", RxID_List);
	if (dev_count != 0) {
		// update KSV List
		chgbank(1);
		for (i = 0; (i < dev_count) && i < 7; i++) {
			// 680X only support 7KSV List
			HDCP_DEBUG_PRINTF("i = %d \r\n", (int) i);
			hdmirxwr(0x2D + (i*5), *(RxID_List + (i*5+0)));
			HDCP_DEBUG_PRINTF("RxID_List[i*5+0] = 0x%02X \r\n", *(RxID_List + (i*5 + 0)));
			hdmirxwr(0x2E + (i*5), *(RxID_List+(i*5 + 1)));
			HDCP_DEBUG_PRINTF("RxID_List[i*5+1] = 0x%02X \r\n", *(RxID_List + (i*5 + 1)));
			hdmirxwr(0x2F + (i*5), *(RxID_List + (i*5 + 2)));
			HDCP_DEBUG_PRINTF("RxID_List[i*5+2] = 0x%02X \r\n", *(RxID_List + (i*5 + 2)));
			hdmirxwr(0x30 + (i*5), *(RxID_List + (i*5 + 3)));
			HDCP_DEBUG_PRINTF("RxID_List[i*5+3] = 0x%02X \r\n", *(RxID_List + (i*5 + 3)));
			hdmirxwr(0x31 + (i*5), *(RxID_List + (i*5 + 4)));
			HDCP_DEBUG_PRINTF("RxID_List[i*5+4] = 0x%02X \r\n", *(RxID_List + (i*5 + 4)));
		}

		// exceeded maxmium dev count
		if (dev_count >= 0x07) {
			// 6805 max supprted dev count
			HDCP_DEBUG_PRINTF("exceeded max HDCP support dev count");
			hdmirxset(0xE4, BIT3, BIT3);
		}

		// exceeded maxmium depth
		if (depth >= 0x03) {
			hdmirxset(0xE4, BIT2, BIT2);
		}
		// update KSV(dev) count in BStatus
		dev_count++;
		depth++;
		hdmirxset(0xE4, 0xF0, (dev_count&0x0F) << 4);	// set dev count
		hdmirxset(0xE5, 0x01, (dev_count&0x10) >> 4);	// set dev count
		hdmirxset(0xE5, 0x0E, (depth)<<1);	// set dev count
		chgbank(0);
	}
	iTE6805_HDCP_Repeater_chg(STATER_6805_HDCP_PrepareSendReady);
}


void iTE6805_Inform_HDCPDone_CB(unsigned char *BKSV_RxID)
{
	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_HDCPDone_CB called \r\n");

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

	if (iTE6805_DATA.STATER_6805 == STATER_6805_HDCP_PrepareSendReady) {
		chgbank(1);
		hdmirxwr(0x28, BKSV_RxID[0]);
		hdmirxwr(0x29, BKSV_RxID[1]);
		hdmirxwr(0x2A, BKSV_RxID[2]);
		hdmirxwr(0x2B, BKSV_RxID[3]);
		hdmirxwr(0x2C, BKSV_RxID[4]);
		chgbank(0);
		iTE6805_HDCP_Repeater_chg(STATER_6805_HDCP_PrepareSendReady);
	}
}

void iTE6805_Inform_EDIDReadyBlock0_CB(unsigned char *EDID)
{
	unsigned char Block0_CheckSum ;
	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_EDIDReadyBlock0_CB called \r\n");
	Block0_CheckSum = iTE6805_EDID_UpdateBloack0(EDID);
	HDCP_DEBUG_PRINTF("PRT : Block0_CheckSum = 0x%02X \r\n", (int) Block0_CheckSum);
	chgbank(0);
	hdmirxwr(0xC9, Block0_CheckSum);		//Port 0 Block 0 CheckSum

#if (_ENABLE_6805_BE_NON_HDCP_REPEATER_WHEN_HDMI_UNPLUG_ == TRUE)
	iTE6805_Set_HPD_Ctrl(iTE6805_DATA.CurrentPort, HPD_LOW);
	iTE6805_Inform_HDCP_EnableRepeaterMode(1);
#endif
}

void iTE6805_Inform_EDIDReadyBlock1_CB(unsigned char *EDID)
{
	unsigned char Block1_CheckSum, VSDB_ADDR, PORT0_EDID_Block1_CheckSum;
	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_EDIDReadyBlock1_CB called \r\n");

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

	Block1_CheckSum = iTE6805_EDID_UpdateBloack1(EDID);
	VSDB_ADDR = iTE6805_EDID_Find_Phyaddress(EDID);

	if (VSDB_ADDR == 0) {
		chgbank(0);					// fill 0x01, 0x02 = 0xFF because can't find VSDB value
		hdmirxwr(0xC6, 0x01);		//VSDB Start Address
		hdmirxwr(0xC7, 0xFF);		//Port 0 AB
		hdmirxwr(0xC8, 0xFF);		//Port 0 CD
		hdmirxwr(0xCA, Block1_CheckSum);		//Port 0 Block 1 CheckSum
		HDCP_DEBUG_PRINTF("PRT : Block1_CheckSum = 0x%02X \r\n", (int) Block1_CheckSum);
	} else {
		iTE6805_DATA.txphyadr[0] = EDID[VSDB_ADDR];
		iTE6805_DATA.txphyadr[1] = EDID[VSDB_ADDR + 1];
		HDCP_DEBUG_PRINTF("txphyadr[0] = 0x%02X\r\n", (int) iTE6805_DATA.txphyadr[0]);
		HDCP_DEBUG_PRINTF("txphyadr[1] = 0x%02X\r\n", (int) iTE6805_DATA.txphyadr[1]);
		iTE6805_EDID_Set_RepeaterCECPhyaddress();

		PORT0_EDID_Block1_CheckSum = (Block1_CheckSum + EDID[VSDB_ADDR] + EDID[VSDB_ADDR+1] - iTE6805_DATA.rxphyadr[PORT0][0] - iTE6805_DATA.rxphyadr[PORT0][1])%0x100;
		HDCP_DEBUG_PRINTF("PRT : PORT0_EDID_Block1_CheckSum = 0x%02X \r\n", (int) PORT0_EDID_Block1_CheckSum);
		chgbank(0);
		hdmirxwr(0xC6, VSDB_ADDR);						//VSDB Start Address
		hdmirxwr(0xC7, iTE6805_DATA.rxphyadr[PORT0][0]);	//Port 0 AB
		hdmirxwr(0xC8, iTE6805_DATA.rxphyadr[PORT0][1]);	//Port 0 CD
		hdmirxwr(0xCA, PORT0_EDID_Block1_CheckSum);		//Port 0 Block 1 CheckSum
	}

	iTE6805_DATA.Flag_EDIDReady = 1;
	delay1ms(500);
	// 5V INT
	iTE6805_INT_5VPWR_Chg(iTE6805_DATA.CurrentPort);
}

void iTE6805_Inform_EDIDReadyNoNeedUpdate_CB(void)
{
	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_EDIDReadyNoNeedUpdate_CB called \r\n");
	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}
	iTE6805_DATA.Flag_EDIDReady = 1;
}

void iTE6805_Inform_HDCP_EnableRepeaterMode(unsigned char Enable)
{
	HDCP_DEBUG_PRINTF("PRT : iTE6805_Inform_HDCP_EnableRepeaterMode called \r\n");
	if (Enable) {
		HDCP_DEBUG_PRINTF("PRT : iTE6805 RPT enable \r\n");
		iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER = 1;
		iTE6805_DATA.Flag_EDIDReady = 0;
		chgbank(0) ;
		hdmirxset(0xCE, BIT7, BIT7);	// Enable HDCP1.4 repeater
		hdmirxset(0xD9, 0x01, 0x01);	// Enable HDCP2.x repeater

		#if CTS_HDCP14	// for HDCP1.4 CTS, need disable HDCP2.2 or CTS fail (should not have value in Rsvd Register)
		hdmirxset(0xE2, BIT0, 0);
		#endif
		chgbank(4) ;
		hdmirxset(0xCE, BIT7, BIT7);	// Enable HDCP1.4 repeater
		chgbank(0) ;
	} else {
		HDCP_DEBUG_PRINTF("PRT : iTE6805 RPT disabled \r\n");
		iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER = 0;
		iTE6805_DATA.Flag_EDIDReady = 1;
		chgbank(0) ;
		hdmirxset(0xCE, BIT7, 0);	// disable HDCP1.4 repeater
		hdmirxset(0xD9, 0x01, 0);	// disable HDCP2.x repeater

		iTE6805_EDID_RAMInitial(); // low HPD and setting EDID

		hdmirxset(0xE2, BIT0, BIT0);	// Enable HDCP2.0

		chgbank(4) ;
		hdmirxset(0xCE, BIT7, 0);	// disable HDCP1.4 repeater
		chgbank(0) ;
	}

	iTE6805_DATA.US_Port_Reset_EnableChange = 1;
	iTE6805_Port_Reset_Body(iTE6805_DATA.CurrentPort);

}

#endif
