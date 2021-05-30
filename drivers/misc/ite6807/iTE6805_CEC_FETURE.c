///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_CEC_FETURE.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#include "iTE6805_Global.h"
#include "iTE6805_CEC_DRV.h"
#include "iTE6805_CEC_SYS.h"
#include "iTE6805_CEC_FETURE.h"

#if (_ENABLE_IT6805_CEC_ == TRUE)

extern _iTE6805_CEC *iTE6805_CEC;

// for more CEC Feture setting, plz reference http://lxr.free-electrons.com/source/include/uapi/linux/cec-funcs.h
// In CEC , Give mean Request


iTE_u8 CEC_POWER_STATUS = CEC_OP_POWER_STATUS_ON;


void iTE6805_CEC_CMD_Feature_decode(pCEC_FRAME CEC_FRAME)
{
	CEC_DEBUG_PRINTF("iTE6805_CEC_CMD_Feature_decode!!");
	// only response
	switch (CEC_FRAME->id.OPCODE) {

		///////////////////////////////
		// General Protocol Messages //
		///////////////////////////////
	case CEC_MSG_FEATURE_ABORT:
		// need to check abort reason is valid or not when need to care CEC_MSG_FEATURE_ABORT
		/* Don't reply with CEC_MSG_FEATURE_ABORT to a CEC_MSG_FEATURE_ABORT message! */
		break;
	case CEC_MSG_ABORT:
		// CEC MSG for TEST, need to reply CEC Feture Abort
		iTE6805_CEC_MSG_Reply_Feture_Abort(CEC_FRAME, CEC_OP_ABORT_REFUSED);
		break;

		////////////////////////////////
		// System Information Feature //
		////////////////////////////////
	case CEC_MSG_GET_CEC_VERSION:
		iTE6805_CEC_MSG_Reply_CEC_Version(CEC_FRAME->id.Initiator, CEC_OP_CEC_VERSION_2_0);
		break;
	case CEC_MSG_CEC_VERSION:
		CEC_DEBUG_PRINTF(" CEC_MSG_CEC_VERSION = ");
		switch (CEC_FRAME->id.OPERAND1) {
		case CEC_OP_CEC_VERSION_1_3A:
			CEC_DEBUG_PRINTF("CEC_OP_CEC_VERSION_1_3A");
			break;
		case CEC_OP_CEC_VERSION_1_4:
			CEC_DEBUG_PRINTF("CEC_OP_CEC_VERSION_1_4");
			break;
		case CEC_OP_CEC_VERSION_2_0:
			CEC_DEBUG_PRINTF("CEC_OP_CEC_VERSION_2_0");
			break;
		}
		break;

	case CEC_MSG_REPORT_PHYSICAL_ADDR:
		CEC_DEBUG_PRINTF(" CEC_MSG_REPORT_PHYSICAL_ADDR = %02X%02X, Primary Device Type = %02X \n", (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND1, (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND2, (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND3);
		break;
	case CEC_MSG_GIVE_PHYSICAL_ADDR:
		iTE6805_CEC_MSG_Future_Report_PA(iTE6805_CEC->MY_PA_LOW, iTE6805_CEC->MY_PA_HIGH, CEC_OP_PRIM_DEVTYPE_TV);
		break;


		//////////////////////////////////////
		// Vendor Specific Commands Feature //
		//////////////////////////////////////
	case CEC_MSG_DEVICE_VENDOR_ID:
		CEC_DEBUG_PRINTF(" CEC_MSG_DEVICE_VENDOR_ID: %02X %02X %02X\n", (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND1, (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND2, (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND3);
		break;

	case CEC_MSG_GIVE_DEVICE_VENDOR_ID:
		iTE6805_CEC_MSG_Device_VenderID(CEC_VENDOR_ID);
		break;


		/* Standby Feature */
	case CEC_MSG_STANDBY:
		break;	// do not need to reply, only TV is Initiator

		/* System Information Feature */
	case CEC_MSG_GET_MENU_LANGUAGE:
	case CEC_MSG_SET_MENU_LANGUAGE:
		break;	// dont need to reply
	case CEC_MSG_REPORT_FEATURES:
		break;

		/* Power Status Feature */
	case CEC_MSG_GIVE_DEVICE_POWER_STATUS:
		iTE6805_CEC_MSG_REPORT_POWER_STATUS(DIRECTED, CEC_FRAME->id.Initiator);
		break;
	case CEC_MSG_REPORT_POWER_STATUS:
		CEC_DEBUG_PRINTF(" CEC_MSG_REPORT_POWER_STATUS: %02X\n", (int) iTE6805_CEC->CEC_FRAME_RX.id.OPERAND1);
		break;

		/* Routing Control Feature */
	case CEC_MSG_REQUEST_ACTIVE_SOURCE:
		iTE6805_CEC_MSG_ACTIVE_SOURCE(iTE6805_CEC->MY_PA_LOW, iTE6805_CEC->MY_PA_HIGH);
		break;
	case CEC_MSG_INACTIVE_SOURCE:
		break;
	case CEC_MSG_ROUTING_CHANGE:
		// need to varifty parameter if implement
		break;
	case CEC_MSG_ROUTING_INFORMATION:
	case CEC_MSG_SET_STREAM_PATH:
		break;	// dont need to reply

		/* OSD Display Feature */
	case CEC_MSG_GIVE_OSD_NAME:
		iTE6805_CEC_MSG_SET_OSD_NAME(CEC_FRAME->id.Initiator);
		break;
	case CEC_MSG_SET_OSD_STRING:
	case CEC_MSG_SET_OSD_NAME:
		break; // dont need to reply

	case CEC_MSG_USER_CONTROL_PRESSED:
	case CEC_MSG_USER_CONTROL_RELEASED:
		break; // dont need to reply, need to update UI by which key CEC get

	case CEC_MSG_GIVE_AUDIO_STATUS:
	case CEC_MSG_REQUEST_SHORT_AUDIO_DESCRIPTOR:
	case CEC_MSG_GIVE_SYSTEM_AUDIO_MODE_STATUS:
	case CEC_MSG_REPORT_AUDIO_STATUS:
	case CEC_MSG_REPORT_SHORT_AUDIO_DESCRIPTOR:
	case CEC_MSG_SET_SYSTEM_AUDIO_MODE:
	case CEC_MSG_SYSTEM_AUDIO_MODE_REQUEST:
	case CEC_MSG_SYSTEM_AUDIO_MODE_STATUS:
		break; // dont need to reply

	case CEC_MSG_INITIATE_ARC:
	case CEC_MSG_REPORT_ARC_INITIATED:
	case CEC_MSG_REPORT_ARC_TERMINATED:
	case CEC_MSG_REQUEST_ARC_INITIATION:
	case CEC_MSG_REQUEST_ARC_TERMINATION:
	case CEC_MSG_TERMINATE_ARC:
		break;

		/* One Touch Play Feature */
	case CEC_MSG_ACTIVE_SOURCE:
		// need to varifty parameter if implement
		break;
	case CEC_MSG_IMAGE_VIEW_ON:
	case CEC_MSG_TEXT_VIEW_ON:
		break;

		/* Deck Control Feature */
	case CEC_MSG_PLAY:
	case CEC_MSG_DECK_STATUS:
	case CEC_MSG_GIVE_DECK_STATUS:
		break;
	case CEC_MSG_DECK_CONTROL:
		iTE6805_CEC_MSG_Reply_Feture_Abort(CEC_FRAME, CEC_OP_ABORT_REFUSED);
		break;

		/* Tuner Control Feature */
	case CEC_MSG_GIVE_TUNER_DEVICE_STATUS:
	case CEC_MSG_SELECT_ANALOGUE_SERVICE:
	case CEC_MSG_SELECT_DIGITAL_SERVICE:
	case CEC_MSG_TUNER_DEVICE_STATUS:
		break;

		/* Vendor Specific Commands Feature */
	case CEC_MSG_VENDOR_COMMAND:
	case CEC_MSG_VENDOR_COMMAND_WITH_ID:
	case CEC_MSG_VENDOR_REMOTE_BUTTON_UP:
	case CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN:
		break;

	case CEC_MSG_RECORD_OFF:
	case CEC_MSG_RECORD_ON:
	case CEC_MSG_RECORD_STATUS:
		break;
	case CEC_MSG_RECORD_TV_SCREEN:
		iTE6805_CEC_MSG_Reply_Feture_Abort(CEC_FRAME, CEC_OP_ABORT_REFUSED);
		break;
	default:
		CEC_DEBUG_PRINTF(" CEC - Can't recognize CEC MSG , ignore it\n");
		// If don't recognize CEC MSG, need to ignore this MSG
		// do not reply any MSG
		break;
	}
}




//////////////////////////////////////////////////////////////////////////////////////////
// System Information Feature
//////////////////////////////////////////////////////////////////////////////////////////
#define CEC_OP_CEC_VERSION_1_3A                         4
#define CEC_OP_CEC_VERSION_1_4                          5
#define CEC_OP_CEC_VERSION_2_0                          6
void iTE6805_CEC_MSG_Reply_CEC_Version(iTE_u8 TARGET_LA, iTE_u8 CEC_Version)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Reply_CEC_Version\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 3;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_CEC_VERSION;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 =  CEC_Version;
	iTE6805_CEC_CMD_Ready_To_Fire();
}
void iTE6805_CEC_MSG_Feture_CEC_Version(iTE_u8 TARGET_LA)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Feture_CEC_Version\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 2;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_GET_CEC_VERSION;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

/* Primary Device Type Operand (prim_devtype) */
#define CEC_OP_PRIM_DEVTYPE_TV				0
#define CEC_OP_PRIM_DEVTYPE_RECORD			1
#define CEC_OP_PRIM_DEVTYPE_TUNER			3
#define CEC_OP_PRIM_DEVTYPE_PLAYBACK		4
#define CEC_OP_PRIM_DEVTYPE_AUDIOSYSTEM		5
#define CEC_OP_PRIM_DEVTYPE_SWITCH			6
#define CEC_OP_PRIM_DEVTYPE_PROCESSOR		7
void iTE6805_CEC_MSG_Future_Report_PA(iTE_u8 PA_LOW, iTE_u8 PA_HIGH, iTE_u8 prim_devtype)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Future_Report_PA\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 5;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_REPORT_PHYSICAL_ADDR;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 =  PA_HIGH;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 =  PA_LOW;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 =  prim_devtype;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

void iTE6805_CEC_MSG_Future_Give_PA(void)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Future_Give_PA\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 2;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_GIVE_PHYSICAL_ADDR;
	iTE6805_CEC_CMD_Ready_To_Fire();
}


//////////////////////////////////////////////////////////////////////////////////////////
// General Protocol Messages
//////////////////////////////////////////////////////////////////////////////////////////
//	The whole structure is zeroed, the len field is set to 1 (i.e. a poll
//	message) and the initiator and destination are filled in.
void iTE6805_CEC_MSG_Future_Polling(iTE_u8 TARGET_LA)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Future_Polling\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	iTE6805_CEC_CMD_Ready_To_Fire();
}

/* Abort Reason Operand (reason) */
#define CEC_OP_ABORT_UNRECOGNIZED_OP                    0
#define CEC_OP_ABORT_INCORRECT_MODE                     1
#define CEC_OP_ABORT_NO_SOURCE                          2
#define CEC_OP_ABORT_INVALID_OP                         3
#define CEC_OP_ABORT_REFUSED                            4
#define CEC_OP_ABORT_UNDETERMINED                       5
void iTE6805_CEC_MSG_Feture_Abort(iTE_u8 TARGET_LA, iTE_u8 CEC_RXCMD, iTE_u8 Abort_Reason)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Feture_Abort\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 4;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_FEATURE_ABORT;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 =  CEC_RXCMD;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 =  Abort_Reason;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

/* This changes the current message into a feature abort message */
// some as  iTE6805_CEC_MSG_Feture_Abort function, wrap it
void iTE6805_CEC_MSG_Reply_Feture_Abort(pCEC_FRAME RX_CEC_FRAME, iTE_u8 Abort_Reason)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Reply_Feture_Abort\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, RX_CEC_FRAME->id.Initiator); // reply to Initiator
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 4;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_FEATURE_ABORT;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 =  RX_CEC_FRAME->id.OPERAND1;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 =  Abort_Reason;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

void iTE6805_CEC_MSG_Abort(iTE_u8 TARGET_LA)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Abort\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 2;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_ABORT;
	iTE6805_CEC_CMD_Ready_To_Fire();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Vendor Specific Commands Feature
//////////////////////////////////////////////////////////////////////////////////////////
void iTE6805_CEC_MSG_Device_VenderID(iTE_u32 vendor_id)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Device_VenderID\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 5;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_DEVICE_VENDOR_ID;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 =  vendor_id >> 16;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 =  (vendor_id >> 8) & 0xff;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND3  = vendor_id & 0xff;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

void iTE6805_CEC_MSG_Give_Deive_VendorID(void)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Give_Deive_VendorID\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 2;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_GIVE_DEVICE_VENDOR_ID;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Standby Feature
//////////////////////////////////////////////////////////////////////////////////////////
// only for root TV using for all device to standby mode, need trigger by TV
void iTE6805_CEC_MSG_STANDBY(void)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_Give_Deive_VendorID\n");
	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 2;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_STANDBY;
	iTE6805_CEC_CMD_Ready_To_Fire();
}


//////////////////////////////////////////////////////////////////////////////////////////
// Power Status Feature
//////////////////////////////////////////////////////////////////////////////////////////
void iTE6805_CEC_MSG_REPORT_POWER_STATUS(iTE_u8 CASTING_MODE, iTE_u8 TARGET_LA)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_REPORT_POWER_STATUS\n");
	if (CASTING_MODE == BCAST) {
		iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	} else {
		iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	}

	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 3;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_REPORT_POWER_STATUS;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 = CEC_POWER_STATUS;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

//////////////////////////////////////////////////////////////////////////////////////////
// Device OSD Transfer Feature
//////////////////////////////////////////////////////////////////////////////////////////
void iTE6805_CEC_MSG_SET_OSD_NAME(iTE_u8 TARGET_LA)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_REPORT_POWER_STATUS\n");

	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, TARGET_LA);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 4;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_SET_OSD_NAME;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 = 0x68;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 = 0x05;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

void iTE6805_CEC_MSG_ACTIVE_SOURCE(iTE_u8 PA_LOW, iTE_u8 PA_HIGH)
{
	CEC_DEBUG_PRINTF(" CEC iTE6805_CEC_MSG_REPORT_POWER_STATUS\n");

	iTE6805_CEC_INIT_CMD(iTE6805_CEC->MY_LA, CEC_LOG_ADDR_BROADCAST);
	iTE6805_CEC->CEC_FRAME_TX.id.CMD_SIZE = 4;
	iTE6805_CEC->CEC_FRAME_TX.id.OPCODE = CEC_MSG_ACTIVE_SOURCE;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND1 = PA_HIGH;
	iTE6805_CEC->CEC_FRAME_TX.id.OPERAND2 = PA_LOW;
	iTE6805_CEC_CMD_Ready_To_Fire();
}

#endif
