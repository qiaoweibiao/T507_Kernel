///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_HDCP_Repeater.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#include "iTE6805_Global.h"

#if _Enable_6615_CB_
#include "..\IT6615\iTE6615_HDCP_Repeater_Callback.h"
#endif

#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)

extern _iTE6805_DATA	iTE6805_DATA;
extern _iTE6805_VTiming	iTE6805_CurVTiming;

#define MAX_HDCP_DOWNSTREAM_TIMEOUT_COUNT 80
iTE_u8 HDCP_DOWNSTREAM_TIMEOUT_COUNT = MAX_HDCP_DOWNSTREAM_TIMEOUT_COUNT;


void iTE6805_HDCP_Repeater_INIT(void)
{
	//return;
    // Enable repeat bit in BCaps
    chgbank(0);
	hdmirxset(0xCE, BIT7, BIT7);	// Enable HDCP1.4 repeater
	hdmirxset(0xD9, 0x01, 0x01);	// Enable HDCP2.x repeater

#if CTS_HDCP14	// for HDCP1.4 CTS, need disable HDCP2.2 or CTS fail (should not have value in Rsvd Register)
	hdmirxset(0xE2, BIT0, 0);
#endif

	chgbank(4) ;
	hdmirxset(0xCE, BIT7, BIT7);	// Enable HDCP1.4 repeater
    chgbank(0) ;
	iTE6805_DATA.STATER_6805 = STATER_6805_Wait_For_HDCP_Done;
	iTE6805_DATA.Flag_EDIDReady = 0;
	#if _Enable_6615_CB_
	iTE6805_DATA.CB_HDCP.iTE6805_Get_DownstreamHDCPStatus_CB = iTE6615_Get_DownstreamHDCPStatus_CB;
	iTE6805_DATA.CB_HDCP.iTE6805_Set_DownstreamHDCPVersion_CB = iTE6615_Set_DownstreamHDCPVersion_CB;
	#endif
}

void iTE6805_HDCP_Repeater_fsm(void)
{
	iTE_u8	HDCPStatus;
	iTE_u8	valid, i;

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

	switch (iTE6805_DATA.STATER_6805) {
	case STATER_6805_Wait_For_HDCP_Done:
		break;
	case STATER_6805_HDCP_AuthPart2_Start:
		HDCP_DEBUG_PRINTF("STATER_6805 in STATER_6805_HDCP_AuthPart2_Start \r\n");

		chgbank(1);
		// clear KSV List
		for (i = 0x28; i < 0x4F; i++) {
				hdmirxwr(i, 0x00);
		}

		// init BStatus
		hdmirxwr(0x10, 0x01);	// DEVICE_COUNT = 1
		hdmirxwr(0x11, 0x01);	// DEPTH = 1

		// init RxInfo
		hdmirxwr(0xE4, 0x12);	// HDCP2_0_REPEATER_DOWNSTREAM = 1, DEVICE_COUNT = 1
		hdmirxwr(0xE5, 0x02);	// DEPTH = 1

		chgbank(0);

		iTE6805_HDCP_Repeater_chg(STATER_6805_HDCP_PrepareSendReady);

		break;
	case STATER_6805_HDCP_PrepareSendReady:
		HDCP_DEBUG_PRINTF("STATER_6805 in STATER_6805_HDCP_PrepareSendReady \r\n");


		if (HDCP_DOWNSTREAM_TIMEOUT_COUNT == 0) {
			iTE6805_HDCP_Repeater_chg(STATER_6805_Wait_For_HDCP_Done);
		} else {
			HDCP_DOWNSTREAM_TIMEOUT_COUNT--;

			iTE6805_DATA.CB_HDCP.iTE6805_Get_DownstreamHDCPStatus_CB(&valid, &HDCPStatus);

			HDCP_DEBUG_PRINTF("iTE6805_DATA.Flag_HDCP_NOW = 2 \r\n");
			// HDCPStatus = 0 = INIT status
			// HDCPStatus = 1 = HDCP Success
			// HDCPStatus = 2 = HDCP Fail
			if (valid == 1 && HDCPStatus == 1) {
				if (iTE6805_DATA.Flag_HDCP_NOW == 1) {
					// trigger SHA
					// 1D = SHA Done INT
					hdmirxset(0xE8, BIT3, BIT3);	// setting to hardware cal
					hdmirxset(0xF6, BIT0, BIT0);
					delay1ms(1);
					hdmirxset(0xF6, BIT0, 0);

					// set ready
					hdmirxset(0xCE, 0x40, 0x40);
				} else {
					HDCP_DEBUG_PRINTF("iTE6805_DATA.Flag_HDCP_NOW = 2 \r\n");
					hdmirxset(0xE2, BIT7, 0);
					hdmirxset(0xE2, BIT7, BIT7);
					hdmirxset(0xE2, BIT7, 0);
				}
				iTE6805_HDCP_Repeater_chg(STATER_6805_Wait_For_HDCP_Done);
			}
		}


		break;
	case STATER_6805_Unknown:
		break;
	default:
		break;
	}
}

void iTE6805_HDCP_Repeater_chg(STATER_Type New_STATE)
{
	iTE_u8 valid, HDCPStatus;

	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 0) {
		return;
	}

	iTE6805_DATA.STATER_6805 = New_STATE;

	switch (New_STATE) {
	case STATER_6805_Wait_For_HDCP_Done:
		HDCP_DEBUG_PRINTF("STATER_6805 chgto STATER_6805_Wait_For_HDCP_Done \r\n");
		break;
	case STATER_6805_HDCP_AuthPart2_Start:
		HDCP_DEBUG_PRINTF("STATER_6805 chgto STATER_6805_HDCP_AuthPart2_Start \r\n");
		break;
	case STATER_6805_HDCP_PrepareSendReady:
		HDCP_DEBUG_PRINTF("STATER_6805 chgto STATER_6805_HDCP_PrepareSendReady \r\n");
		iTE6805_DATA.CB_HDCP.iTE6805_Get_DownstreamHDCPStatus_CB(&valid, &HDCPStatus);
		// HDCPStatus = 0 = INIT status
		// HDCPStatus = 1 = HDCP Success
		// HDCPStatus = 2 = HDCP Fail
		if (valid == 1 && HDCPStatus == 1) {
			if (iTE6805_DATA.Flag_HDCP_NOW == 1) {
				// trigger SHA
				// 1D = SHA Done INT
				HDCP_DEBUG_PRINTF("iTE6805_DATA.Flag_HDCP_NOW = 1 \r\n");
				hdmirxset(0xE8, BIT3, BIT3);	// setting to hardware cal
				hdmirxset(0xF6, BIT0, BIT0);
				delay1ms(1);
				hdmirxset(0xF6, BIT0, 0);

				// set ready
				hdmirxset(0xCE, 0x40, 0x40);
			} else {
				HDCP_DEBUG_PRINTF("iTE6805_DATA.Flag_HDCP_NOW = 2 \r\n");
				hdmirxset(0xE2, BIT7, 0);
				hdmirxset(0xE2, BIT7, BIT7);
				hdmirxset(0xE2, BIT7, 0);
			}
			iTE6805_HDCP_Repeater_chg(STATER_6805_Wait_For_HDCP_Done);
		}

		HDCP_DOWNSTREAM_TIMEOUT_COUNT = MAX_HDCP_DOWNSTREAM_TIMEOUT_COUNT;

		break;
	case STATER_6805_Unknown:
		break;
	}
}


void iTE6805_EDID_Set_RepeaterCECPhyaddress(void)
{
	iTE_u8 rxphyA, rxphyB, rxphyC, rxphyD, rxcurport;			// for CEC function
	iTE_u8 txphyA, txphyB, txphyC, txphyD, txphylevel;
	txphyA = (iTE6805_DATA.txphyadr[0]&0xF0)>>4;
	txphyB = (iTE6805_DATA.txphyadr[0]&0x0F);
	txphyC = (iTE6805_DATA.txphyadr[1]&0xF0)>>4;
	txphyD = (iTE6805_DATA.txphyadr[1]&0x0F);

	if (txphyA == 0 && txphyB == 0 && txphyC == 0 && txphyD == 0)
		txphylevel = 0;
	else if (txphyB == 0 && txphyC == 0 && txphyD == 0)
		txphylevel = 1;
	else if (txphyC == 0 && txphyD == 0)
		txphylevel = 2;
	else if (txphyD == 0)
		txphylevel = 3;
	else
		txphylevel = 4;

	rxcurport = 0;
	switch (txphylevel) {
	case 0:
		iTE6805_DATA.rxphyadr[0][0] = 0x10;
		iTE6805_DATA.rxphyadr[0][1] = 0x00;
		break;
	case 1:
		iTE6805_DATA.rxphyadr[0][0] = (txphyA<<4) + 0x01;
		iTE6805_DATA.rxphyadr[0][1] = 0x00;
		break;
	case 2:
		iTE6805_DATA.rxphyadr[0][0] = iTE6805_DATA.txphyadr[0];
		iTE6805_DATA.rxphyadr[0][1] = 0x10;
		break;
	case 3:
		iTE6805_DATA.rxphyadr[0][0] = iTE6805_DATA.txphyadr[0];
		iTE6805_DATA.rxphyadr[0][1] = (txphyC<<4)+0x01;
		break;
	default:
		iTE6805_DATA.rxphyadr[0][0] = 0xFF;
		iTE6805_DATA.rxphyadr[0][1] = 0xFF;
		break;
	}
	EDID_DEBUG_PRINTF("\r\nDnStrm PhyAdr = %X, %X, %X, %X\r\n", (int) txphyA, (int) txphyB, (int) txphyC, (int) txphyD);

	rxphyA = (iTE6805_DATA.rxphyadr[0][0]&0xF0)>>4;
	rxphyB = (iTE6805_DATA.rxphyadr[0][0]&0x0F);
	rxphyC = (iTE6805_DATA.rxphyadr[0][1]&0xF0)>>4;
	rxphyD = (iTE6805_DATA.rxphyadr[0][1]&0x0F);
    EDID_DEBUG_PRINTF(" PortA PhyAdr = %X, %X, %X, %X\r\n", (int) rxphyA, (int) rxphyB, (int) rxphyC, (int) rxphyD);
}

void iTE6805_HDCP_EnableRepeaterMode(iTE_u8 Enable)
{
	iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER = Enable;

	if (Enable) {
		iTE6805_DATA.Flag_EDIDReady = 0;
	} else {
		iTE6805_DATA.Flag_EDIDReady = 1;
	}

	iTE6805_Inform_HDCP_EnableRepeaterMode(Enable);
}

#endif
