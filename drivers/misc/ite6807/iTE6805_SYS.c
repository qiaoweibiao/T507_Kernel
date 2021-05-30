///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_SYS.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
#include "iTE6805_Global.h"
#include "iTE6805_CSC_Table.h"

extern _iTE6805_DATA	iTE6805_DATA;
extern _iTE6805_VTiming	iTE6805_CurVTiming;
void delay1ms(iTE_u16 ms);

#define TIMEOUT_ECC_ERROR			0x0A
#define TIMEOUT_SCDT_CHECK_COUNT	MS_TimeOut(5)
#define TIMEOUT_AUDIO_CHECK_COUNT	MS_TimeOut(3)
#define TIMEOUT_AUDIO_MONITOR		MS_TimeOut(2)

iTE_u8 Current_ECCAbnormal_Count;
iTE_u8 Current_ECCError_Count;
iTE_u8 Current_SCDTCheck_Count;
iTE_u8 Current_AudioCheck_Count;
iTE_u8 Current_AudioMonitor_Count;

#define Max_VidStable_Count 50
iTE_u8 Current_VidStable_Count;
#define Max_SymbolLockRst_Need_Rst_SCDC 20
iTE_u8 Current_Ch0_SymbolLockRst_Count;
iTE_u8 Current_Ch1_SymbolLockRst_Count;
iTE_u8 Current_Ch2_SymbolLockRst_Count;

#define Max_AudioSamplingFreq_ErrorCount	15
iTE_u8 Current_AudioSamplingFreq_ErrorCount;

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
iTE_u8 Current_AuthStartCount;
#endif

// need to restart eq
#define MAX_deskewerrcnt 10
iTE_u8 deskewerrcnt;
iTE_u8 abnormalcnt;

// need to restart IPLL lock
#define Max_Current_IPLL_ReLock_Count	6
iTE_u8 Current_IPLL_ReLock_Count;



// pre Frame data container
iTE_u8 prevAVIDB;
iTE_u8 prevAudioB0_Status;		// compare mask 0xF0
iTE_u8 prevAudioB1_Status;		// compare mask 0xFF
iTE_u8 prevAudioB2_CHStatus;	// compare mask BIT1

// Flag
iTE_u8 Flag_NewAVIInfoFrame = FALSE;
iTE_u8 Flag_FirstTimeAudioMonitor = TRUE;
iTE_u8 Flag_HDCP_Trigger_AutoEQ_Again = TRUE;
iTE_u8 Flag_FirstTimeParameterChange = FALSE;
iTE_u8 Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = FALSE;
iTE_u8 Flag_First_Time_VidStable_Done = FALSE;

#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
iTE_u8 Flag_Need_Power_Normal_Mode_Setting = TRUE;
iTE_u8 iTE6805_POWER_MODE = MODE_POWER_SAVING;
#endif

/*** [iTE6805_Poll_Fsm IT6805 system polling] ***/
static iTE_u8 iTE6805_CurrentStage;
#define STAGE6805_INIT 0
#define STAGE6805_POLLING 1
void iTE6805_FSM(void)
{
	switch (iTE6805_CurrentStage) {
		// add this for prevent Infinite loop when I2C function not ready
	case STAGE6805_INIT:
		// waiting for I2C ready
		if (iTE6805_Identify_Chip()) {
			// init 6805
			iTE6805_Init_fsm();
			iTE6805_CurrentStage = STAGE6805_POLLING;
		} else {
			HDMIRX_DEBUG_PRINT("# iTE6805 Waiting for I2C function ready \n");
		}
		break;

	case STAGE6805_POLLING:
		// main loop of 6805
		iTE6805_MainBody();
		break;
	}
}

void iTE6805_MainBody(void)
{

	iTE6805_Port_Reset_Body(iTE6805_DATA.US_Port_Reset_Port);
	/*delay task to polling for multi-thread*/
	iTE6805_Port_Select_Body(iTE6805_DATA.US_CurrentPort);

	#if (_ENABLE_EDID_RAM_ == TRUE)
    iTE6805_Port_SetEDID_Body(iTE6805_DATA.US_SetEDID_Port, iTE6805_DATA.US_SetEDID_Index);
	#endif

	#if (_ENABLE_EXTERN_EQ_CTRL_ == TRUE)
	iTE6805_Set_EQ_LEVEL(iTE6805_DATA.EQ_US_PORT, iTE6805_DATA.EQ_US_EQ_LEVEL);
	#endif

	#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
	if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 1) {
		iTE6805_HDCP_Repeater_fsm();
	}
	#endif

	if (iTE6805_DATA.STATEV != STATEV_Unplug && iTE6805_DATA.STATEV != STATEV_VideoOff) {
		if (iTE6805_DATA.CurrentPort == PORT0) {
			iTE6805_hdmirx_port0_SYS_irq();
			#if (_ENABLE_AUTO_EQ_ == TRUE)
			iTE6805_hdmirx_port0_EQ_irq();
			#endif

			#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
			iTE6805_mhlrx_irq();
			#endif
		} else {
			iTE6805_hdmirx_port1_SYS_irq();
			#if (_ENABLE_AUTO_EQ_ == TRUE)
			iTE6805_hdmirx_port1_EQ_irq();
			#endif
		}
		iTE6805_hdmirx_common_irq();
	}

	//do not need iTE6805_hdmirx_common_irq_for_HPD() for detect non-main port HPD anymore
	//can't keep HPD when non-main port or switch port TX may not resend signal

	#if (_ENABLE_IT6805_CEC_ == TRUE)
	iTE6805_hdmirx_CEC_irq();
	#endif

	#if (_ENABLE_AUTO_EQ_ == TRUE)
	iTE6805_EQ_fsm();
	#endif

	iTE6805_vid_fsm();
	iTE6805_aud_fsm();

	if (iTE6805_DATA.Flag_VidStable_Done == TRUE) {
		#if (ENABLE_DETECT_VSIF_PKT == TRUE)
		iTE6805_Detect3DFrame();
		#endif

		#if (ENABLE_DETECT_DRM_PKT == TRUE)
		iTE6805_DRM_Detect();
		#endif

		iTE6805_DATA.AVMute_Status = iTE6805_Check_AVMute();
	}


	#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
	iTE6805_HDCP_Detect();
	#endif

	// this only for EVB detect port
	#if (_MCU_8051_EVB_ == TRUE)
	iTE6805_Port_Detect();
	//iTE6805_Output_ColorSpace_Detect_BY_PIN();
	#endif

}

void iTE6805_hdmirx_port0_SYS_irq(void)
{
	iTE_u8 Reg05h, Reg06h, Reg08h, Reg09h;
	iTE_u8 Reg13h, Reg14h, Reg15h;

	chgbank(0);
	Reg05h = hdmirxrd(0x05);
	hdmirxwr(0x05, Reg05h);// port0
	Reg06h = hdmirxrd(0x06);
	hdmirxwr(0x06, Reg06h);// port0
	Reg08h = hdmirxrd(0x08);
	hdmirxwr(0x08, Reg08h);// port0
	Reg09h = hdmirxrd(0x09);
	hdmirxwr(0x09, Reg09h & 0xFB);// port0

	Reg13h = hdmirxrd(0x13);
	Reg14h = hdmirxrd(0x14);
	Reg15h = hdmirxrd(0x15);

	if (Reg05h != 0x00) {
		if (Reg05h & 0x10) {
			if (iTE6805_Check_CLK_Vaild()) {
				iTE6805_INT_HDMI_DVI_Mode_Chg(PORT0);
			}
		}
		if (Reg05h & 0x20) {
			HDMIRX_DEBUG_PRINT("# Port 0 ECC Error # \n");
			iTE6805_INT_ECC_ERROR();
		}
		if (Reg05h & 0x40) {
			deskewerrcnt += 1;
			HDMIRX_DEBUG_PRINT("# Port 0 Deskew Error #  deskewerrcnt=%d\n", (int) deskewerrcnt);
#if (_ENABLE_AUTO_EQ_ == TRUE)
			if (deskewerrcnt > MAX_deskewerrcnt && iTE6805_Check_HDMI2() == FALSE) {
				deskewerrcnt = 0;
				HDMIRX_DEBUG_PRINT("# Deskew Error > %d #  EQ Restart \n", (int) deskewerrcnt);
				iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
				abnormalcnt++;
				if (abnormalcnt == 3) {
					abnormalcnt = 0;
					iTE6805_Reset_ALL_Logic(PORT0);
				}
			}
#endif
		}

		if (Reg05h & 0x80) {
			HDMIRX_DEBUG_PRINT("# Port 0 H2VSkew Fail #\n");
		}

		if (Reg05h & 0x04) {
			HDMIRX_DEBUG_PRINT("# Port 0 Input Clock Change Detect #\n");
			Current_Ch0_SymbolLockRst_Count = 0;
			Current_Ch1_SymbolLockRst_Count = 0;
			Current_Ch2_SymbolLockRst_Count = 0;
			if (hdmirxrd(0x13) & 0x10) {
				HDMIRX_DEBUG_PRINT("# Clock Stable		#\n");
			} else {

				HDMIRX_DEBUG_PRINT("# Clock NOT Stable	#\n");
				iTE6805_DATA.STATE_HDCP1 = 0;
				iTE6805_DATA.STATE_HDCP2 = 0;
				#if (_ENABLE_AUTO_EQ_ == TRUE)
				iTE6805_EQ_chg(STATEEQ_Off);
				iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
				#endif

				//if((hdmirxrd(0x23) & BIT1) == 0)	// prevent customer force 6805 not support HDCP but this reset will enable HDCP
				//{
				//	hdmirxset(0x23, BIT1, BIT1);
				//	hdmirxset(0x23, BIT1, 0);
				//}

			}
		}

		if (Reg05h & 0x02) {
			Current_ECCError_Count = 0;
			HDMIRX_DEBUG_PRINT("# Port 0 Rx CKOn Detect #\n");
			#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
			if (Flag_Need_Power_Normal_Mode_Setting == TRUE) {
				iTE6805_DEBUG_INT_PRINTF("# Power MODE_POWER_NORMAL #\n");
				iTE6805_Set_Power_Mode(MODE_POWER_NORMAL);
				iTE6805_POWER_MODE = MODE_POWER_NORMAL;
				Flag_Need_Power_Normal_Mode_Setting = FALSE;
			}
			#endif
		}

		if (Reg05h & 0x01) {
			HDMIRX_DEBUG_PRINT("# Port 0 5V state change INT #\n");
			iTE6805_INT_5VPWR_Chg(PORT0);
		}
	}

	if (Reg06h != 0x00) {
		if (Reg06h & 0x80) {
			HDMIRX_DEBUG_PRINT("# FSM Error  #\n");
		}

		if ((Reg06h & 0x40) && iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) == MODE_HDMI) {
			HDMIRX_DEBUG_PRINT("# CH2 Symbol lock Rst #\n");
			Current_Ch2_SymbolLockRst_Count++;
		}

		if ((Reg06h & 0x20) && iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) == MODE_HDMI) {
			HDMIRX_DEBUG_PRINT("# CH1 Symbol lock Rst #\n");
			Current_Ch1_SymbolLockRst_Count++;
		}

		if (Reg06h & 0x10) {
			HDMIRX_DEBUG_PRINT("# CH0 Symbol lock Rst Count=%d #\n", (int) Current_Ch0_SymbolLockRst_Count);
			Current_Ch0_SymbolLockRst_Count++;

			#if (_ENABLE_AUTO_EQ_ == TRUE)
			// If 3Ch all symbolLockRst and clk not stable cus EQ Can't start
			// force SCDC Rst and force setting Clock Ration from 1/10 to 1/40
			// try to clock valid , this case is 3.4Gup and not write SCDC to RX
			if ((Current_Ch0_SymbolLockRst_Count > Max_SymbolLockRst_Need_Rst_SCDC) &&
					(Current_Ch1_SymbolLockRst_Count > Max_SymbolLockRst_Need_Rst_SCDC) &&
					(Current_Ch2_SymbolLockRst_Count > Max_SymbolLockRst_Need_Rst_SCDC) &&
					(iTE6805_DATA.STATEEQ == STATEEQ_Off) && iTE6805_Check_HDMI2() == FALSE) {
				HDMIRX_DEBUG_PRINT("# Force set SCDC to 1/40#\n");

				// force set SCDC Clock Ratio 1/40 and Scramble
				chgbank(3);
				hdmirxset(0xE5, 0x1C, 0x1C);
				chgbank(0);
			}
			#endif
		}

		if (Reg06h & 0x08) {
			HDMIRX_DEBUG_PRINT("# CH2 CDR FIFO Aut0-Rst #\n");
		}

		if (Reg06h & 0x04) {
			HDMIRX_DEBUG_PRINT("# CH1 CDR FIFO Aut0-Rst #\n");
		}

		if (Reg06h & 0x02) {
			HDMIRX_DEBUG_PRINT("# CH0 CDR FIFO Aut0-Rst #\n");
		}

		if (Reg06h & 0x01) {
			HDMIRX_DEBUG_PRINT("# Symbol Lock State Change # ");
			HDMIRX_DEBUG_PRINT(" 0x14 =  %x \n", (int) hdmirxrd(0x14)&0x38);
			#if (_ENABLE_AUTO_EQ_ == TRUE)
			chgbank(0);
			if (hdmirxrd(0x14)&0x38 && iTE6805_DATA.STATEEQ == STATEEQ_Off) {
				iTE6805_EQ_chg(STATEEQ_Off);
				iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
			}
			#endif

			if (Reg13h & 0x80) {
				HDMIRX_DEBUG_PRINT("# Symbol Lock #\n");
			} else {
				HDMIRX_DEBUG_PRINT("# Symbol NOT Lock #\n");
				if (Reg14h & 0x20)
					HDMIRX_DEBUG_PRINT("# CH2 Symbol NOT Lock #\n");
				if (Reg14h & 0x10)
					HDMIRX_DEBUG_PRINT("# CH1 Symbol NOT Lock #\n");
				if (Reg14h & 0x08)
					HDMIRX_DEBUG_PRINT("# CH0 Symbol NOT Lock #\n");
			}
		}
	}

	if (Reg08h != 0x00) {
		if (Reg08h & 0x01) {

			HDMIRX_DEBUG_PRINT("# SCDC CLK Ratio Change #\n");
			iTE6805_vid_chg(STATEV_WaitSync);
			#if (_ENABLE_AUTO_EQ_ == TRUE)
			iTE6805_EQ_chg(STATEEQ_Off);
			iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
			#endif
			if (Reg14h & 0x40) {
				HDMIRX_DEBUG_PRINT("SCDC CLK Ratio 1/40 \n");
			} else {
				HDMIRX_DEBUG_PRINT("SCDC CLK Ratio 1/10 \n");
			}
		}
		if (Reg08h & 0x02) {
			HDMIRX_DEBUG_PRINT("# SCDC Scrambe Enable Change #\n");
			if (Reg14h & 0x80) {
				HDMIRX_DEBUG_PRINT("SCDC SCR Enable Bit= 1 \n");
			} else {
				HDMIRX_DEBUG_PRINT("SCDC SCR Enable Bit= 0\n");
			}
		}
		if (Reg08h & 0x04) {
			HDMIRX_DEBUG_PRINT("# SCDC Scramble Status Change #\n");
			if ((Reg15h & 0x02) >> 1) {
				HDMIRX_DEBUG_PRINT("#  SCDC Scarmable Status:  ON  #\n");
			} else {
				HDMIRX_DEBUG_PRINT("#  SCDC Scarmable Status: OFF  #\n");
			}
		}

		if (Reg08h & 0x08) {
			HDMIRX_DEBUG_PRINT("# EDID Bus Hange #\n");
		}
		if (Reg08h & 0x80) {
			// HDMI2 Auto Det
			HDMIRX_DEBUG_PRINT("HDMI2 Det State=%x \n", (hdmirxrd(0x15) & 0x3C) >> 2);
			hdmirxset(0x4C, 0x80, 0x80);
		}
	}

	if (Reg09h != 0x00) {
		if (Reg09h & 0x01) {
			Current_ECCError_Count = 0;
			HDMIRX_DEBUG_PRINT("# Port 0 HDCP Authentication Start #\n");
			#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
			if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 1) {
				if (iTE6805_DATA.STATER_6805 == STATER_6805_Wait_For_HDCP_Done) {
					iTE6805_DATA.Flag_HDCP_NOW = 1;
					iTE6805_DATA.CB_HDCP.iTE6805_Set_DownstreamHDCPVersion_CB(1);	// setting chip call back HDCP1 enabled
					iTE6805_HDCP_Repeater_chg(STATER_6805_HDCP_AuthPart2_Start);
				}
			}
			#endif
			#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
			if (iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) && Current_AuthStartCount++ > 2) {
				Current_AuthStartCount = 0 ;
				HDMIRX_DEBUG_PRINT("# Port 0 HDCP Authentication Start Exceed , reset PATH_EN #\n");
				chgbank(0);
				hdmirxset(0x24, 0xF8, 0xF8);
				hdmirxset(0x24, 0xF8, 0x00);
				iTE6805_DATA.MHL_DiscoveryDone = 0;

				mhlrxset(0x0C, BIT2, BIT2);
				delay1ms(300);
				mhlrxset(0x0C, BIT1, BIT1);
			}
			#endif
		}
		if (Reg09h & 0x02) {
			HDMIRX_DEBUG_PRINT("# Port 0 HDCP Authentication Done #\n");
		}
		if (Reg09h & 0x08) {
			Current_ECCError_Count = 0;
			#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
			Current_AuthStartCount = 0;
			#endif
			HDMIRX_DEBUG_PRINT("# Port 0 HDCP Encryption Off #\n");
		}
	}

}

void iTE6805_hdmirx_port1_SYS_irq(void)
{
	iTE_u8 Reg0Ah, Reg0Bh, Reg0Dh, Reg0Eh;
	iTE_u8 Reg16h, Reg17h, Reg18h;

	//if (iTE6805_DATA.STATEV == STATEV_Unplug || iTE6805_DATA.STATEV == STATEV_VideoOff ||
	//	iTE6805_DATA.CurrentPort != PORT1)  return;

	chgbank(0);
	Reg0Ah = hdmirxrd(0x0A);
	hdmirxwr(0x0A, Reg0Ah); // port1
	Reg0Bh = hdmirxrd(0x0B);
	hdmirxwr(0x0B, Reg0Bh); // port1
	Reg0Dh = hdmirxrd(0x0D);
	hdmirxwr(0x0D, Reg0Dh); // port1
	Reg0Eh = hdmirxrd(0x0E);
	hdmirxwr(0x0E, Reg0Eh); // port1

	Reg16h = hdmirxrd(0x16);
	Reg17h = hdmirxrd(0x17);
	Reg18h = hdmirxrd(0x18);

	if (Reg0Ah != 0x00) {
		if (Reg0Ah & 0x10) {
			if (iTE6805_Check_CLK_Vaild()) {
				iTE6805_INT_HDMI_DVI_Mode_Chg(PORT1);
			}
		}
		if (Reg0Ah & 0x20) {
			HDMIRX_DEBUG_PRINT("# Port 1 ECC Error # \n");
			iTE6805_INT_ECC_ERROR();
		}
		if (Reg0Ah & 0x40) {
			deskewerrcnt += 1;
			HDMIRX_DEBUG_PRINT("# Port 1 Deskew Error #  deskewerrcnt=%d\n", (int) deskewerrcnt);
#if (_ENABLE_AUTO_EQ_ == TRUE)
			if (deskewerrcnt > MAX_deskewerrcnt && iTE6805_Check_HDMI2() == FALSE) {
				deskewerrcnt = 0;
				HDMIRX_DEBUG_PRINT("# Deskew Error > %d #  EQ Restart \n", (int) deskewerrcnt);
				iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
				abnormalcnt++;
				if (abnormalcnt == 3) {
					abnormalcnt = 0;
					iTE6805_Reset_ALL_Logic(PORT1);
				}
			}
#endif
		}

		if (Reg0Ah & 0x80) {
			HDMIRX_DEBUG_PRINT("# Port 1 H2VSkew Fail #\n");
		}

		if (Reg0Ah & 0x04) {
			HDMIRX_DEBUG_PRINT("# Port 1 Input Clock Change Detect #\n");
			Current_Ch0_SymbolLockRst_Count = 0;
			Current_Ch1_SymbolLockRst_Count = 0;
			Current_Ch2_SymbolLockRst_Count = 0;
			if (hdmirxrd(0x16) & 0x10) {
				HDMIRX_DEBUG_PRINT("# Clock Stable		#\n");
			} else {
				HDMIRX_DEBUG_PRINT("# Clock NOT Stable	#\n");
				#if (_ENABLE_AUTO_EQ_ == TRUE)
				iTE6805_EQ_chg(STATEEQ_Off);
				iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
				#endif

				//if((hdmirxrd(0x2B)&BIT1)== 0)	// prevent customer force 6805 not support HDCP but this reset will enable HDCP
				//{
				//	hdmirxset(0x2B, BIT1, BIT1);
				//	hdmirxset(0x2B, BIT1, 0);
				//}

			}
		}

		if (Reg0Ah & 0x02) {
			Current_ECCError_Count = 0;
			HDMIRX_DEBUG_PRINT("# Port 1 Rx CKOn Detect #\n");
			#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
			if (Flag_Need_Power_Normal_Mode_Setting == TRUE) {
				iTE6805_DEBUG_INT_PRINTF("# Power MODE_POWER_NORMAL #\n");
				iTE6805_Set_Power_Mode(MODE_POWER_NORMAL);
				iTE6805_POWER_MODE = MODE_POWER_NORMAL;
				Flag_Need_Power_Normal_Mode_Setting = FALSE;
			}
			#endif
		}

		if (Reg0Ah & 0x01) {
			HDMIRX_DEBUG_PRINT("# Port 1 5V state change INT #\n");
			iTE6805_INT_5VPWR_Chg(PORT1);
		}
	}


	if (Reg0Bh != 0x00) {
		if (Reg0Bh & 0x80) {
			HDMIRX_DEBUG_PRINT("# FSM Error  #\n");
		}

		if ((Reg0Bh & 0x40) && iTE6805_Check_PORT0_IS_MHL_Mode(PORT1) == MODE_HDMI) {
			HDMIRX_DEBUG_PRINT("# CH2 Symbol lock Rst #\n");
			Current_Ch2_SymbolLockRst_Count++;
		}

		if ((Reg0Bh & 0x20) && iTE6805_Check_PORT0_IS_MHL_Mode(PORT1) == MODE_HDMI) {
			HDMIRX_DEBUG_PRINT("# CH1 Symbol lock Rst #\n");
			Current_Ch1_SymbolLockRst_Count++;
		}

		if (Reg0Bh & 0x10) {
			HDMIRX_DEBUG_PRINT("# CH0 Symbol lock Rst #\n");
			Current_Ch0_SymbolLockRst_Count++;

			#if (_ENABLE_AUTO_EQ_ == TRUE)
			if ((Current_Ch0_SymbolLockRst_Count > Max_SymbolLockRst_Need_Rst_SCDC) &&
					(Current_Ch1_SymbolLockRst_Count > Max_SymbolLockRst_Need_Rst_SCDC) &&
					(Current_Ch2_SymbolLockRst_Count > Max_SymbolLockRst_Need_Rst_SCDC) &&
					(iTE6805_DATA.STATEEQ == STATEEQ_Off) && iTE6805_Check_HDMI2() == FALSE) {
				HDMIRX_DEBUG_PRINT("# Force set SCDC to 1/40#\n");

				// force set SCDC Clock Ratio 1/40 and Scramble
				chgbank(7);
				hdmirxset(0xE5, 0x1C, 0x1C);
				chgbank(0);
			}
			#endif
		}

		if (Reg0Bh & 0x08) {
			HDMIRX_DEBUG_PRINT("# CH2 CDR FIFO Aut0-Rst #\n");
		}

		if (Reg0Bh & 0x04) {
			HDMIRX_DEBUG_PRINT("# CH1 CDR FIFO Aut0-Rst #\n");
		}

		if (Reg0Bh & 0x02) {
			HDMIRX_DEBUG_PRINT("# CH0 CDR FIFO Aut0-Rst #\n");
		}

		if (Reg0Bh & 0x01) {
			HDMIRX_DEBUG_PRINT("# Symbol Lock State Change #\n");

			#if (_ENABLE_AUTO_EQ_ == TRUE)
			chgbank(0);
			HDMIRX_DEBUG_PRINT(" 0x17 =  %x \n", (int) hdmirxrd(0x17)&0x38);
			HDMIRX_DEBUG_PRINT(" STATEEQ = %d \n", (int) iTE6805_DATA.STATEEQ);
			if (hdmirxrd(0x17)&0x38 && iTE6805_DATA.STATEEQ == STATEEQ_Off) {
				iTE6805_EQ_chg(STATEEQ_Off);
				iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ); // Check inside
			}
			#endif

			if (Reg16h & 0x80) {
				HDMIRX_DEBUG_PRINT("# Symbol Lock #\n");
			} else {
				HDMIRX_DEBUG_PRINT("# Symbol NOT Lock #\n");
				if (Reg17h & 0x20)
					HDMIRX_DEBUG_PRINT("# CH2 Symbol NOT Lock #\n");
				if (Reg17h & 0x10)
					HDMIRX_DEBUG_PRINT("# CH1 Symbol NOT Lock #\n");
				if (Reg17h & 0x08)
					HDMIRX_DEBUG_PRINT("# CH0 Symbol NOT Lock #\n");
			}
		}
	}

	if (Reg0Dh != 0x00) {
		if (Reg0Dh & 0x01) {
			HDMIRX_DEBUG_PRINT("# SCDC CLK Ratio Change #\n");
			iTE6805_vid_chg(STATEV_WaitSync);
			#if (_ENABLE_AUTO_EQ_ == TRUE)
			iTE6805_EQ_chg(STATEEQ_Off);
			iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
			#endif
			if (Reg17h & 0x40) {
				HDMIRX_DEBUG_PRINT("SCDC CLK Ratio 1/40 \n");
			} else {
				HDMIRX_DEBUG_PRINT("SCDC CLK Ratio 1/10 \n");
			}
		}
		if (Reg0Dh & 0x02) {
			HDMIRX_DEBUG_PRINT("# SCDC Scrambe Enable Change #\n");
			if (Reg17h & 0x80) {
				HDMIRX_DEBUG_PRINT("SCDC SCR Enable Bit= 1 \n");
			} else {
				HDMIRX_DEBUG_PRINT("SCDC SCR Enable Bit= 0\n");
			}
		}
		if (Reg0Dh & 0x04) {
			HDMIRX_DEBUG_PRINT("# SCDC Scramble Status Change #\n");
			if ((Reg18h & 0x02) >> 1) {
				HDMIRX_DEBUG_PRINT("#  SCDC Scarmable Status:  ON  #\n");
			} else {
				HDMIRX_DEBUG_PRINT("#  SCDC Scarmable Status: OFF  #\n");
			}
		}
		if (Reg0Dh & 0x08) {
			HDMIRX_DEBUG_PRINT("# EDID Bus Hange #\n");
		}
		if (Reg0Dh & 0x80) {
			// HDMI2 Auto Det
			HDMIRX_DEBUG_PRINT("HDMI2 Det State=%x \n", (hdmirxrd(0x15) & 0x3C) >> 2);
			hdmirxset(0x4C, 0x80, 0x80);
		}
	}


	if (Reg0Eh != 0x00) {
		if (Reg0Eh & 0x01) {
			Current_ECCError_Count = 0;
			HDMIRX_DEBUG_PRINT("# Port 1 HDCP Authentication Start #\n");
		}
		if (Reg0Eh & 0x02) {
			HDMIRX_DEBUG_PRINT("# Port 1 HDCP Authentication Done #\n");
		}
		if (Reg0Eh & 0x08) {
			Current_ECCError_Count = 0;
			HDMIRX_DEBUG_PRINT("# Port 1 HDCP Encryption Off #\n");
		}
	}

}

void iTE6805_hdmirx_common_irq(void)
{
	iTE_u8 Reg09h, Reg10h, Reg11h, Reg12h;
	iTE_u8 Reg1Ah, Reg1Bh, Reg1Dh, RegD4h, RegD5h;

#if (defined(SPI_ENABLE)) && (SPI_ENABLE == 1) && (iTE6807 == 1)
	iTE_u8 Reg1Ch, x, rddata[32];
#endif

	iTE_u8 HDCP2_Active;

	#if _MCU_8051_EVB_
	iTE_u16 HActive, Flag_Trigger_DownScale_Reset = FALSE;
	#endif

	chgbank(0);
	Reg09h = hdmirxrd(0x09);
	hdmirxwr(0x09, Reg09h&0x04);// common
	Reg10h = hdmirxrd(0x10);
	hdmirxwr(0x10, Reg10h);// common
	Reg11h = hdmirxrd(0x11);
	hdmirxwr(0x11, Reg11h&0xBF);// common // BIT6 for Gen Pkt
	Reg12h = hdmirxrd(0x12);
	hdmirxwr(0x12, Reg12h&0x5F);// common // DRM check change to DRM_Detect function
	//Reg19h = hdmirxrd(0x19);
	//hdmirxwr(0x19 ,Reg19h);// common //useless code
#if (defined(SPI_ENABLE)) && (SPI_ENABLE == 1) && (iTE6807 == 1)
	Reg1Ch = hdmirxrd(0x1C);
	hdmirxwr(0x1C, Reg1Ch&0x40);// common
#endif
	Reg1Dh = hdmirxrd(0x1D);
	hdmirxwr(0x1D, Reg1Dh);// common
	RegD4h = hdmirxrd(0xD4);
	hdmirxwr(0xD4, RegD4h);// common
	RegD5h = hdmirxrd(0xD5);
	hdmirxwr(0xD5, RegD5h);// common

	if (Reg09h & 0x04) {

		if (hdmirxrd(0xCF) & BIT5) {
			HDMIRX_DEBUG_PRINT("HDCP Encryption ON! \n");
			chgbank(0);
			HDCP2_Active = hdmirxrd(0xD6) & 0xC0;
			if (HDCP2_Active != 0x00) {
				iTE6805_DATA.STATE_HDCP2 = 1;
				iTE6805_DATA.STATE_HDCP1 = 0;
			} else {
				iTE6805_DATA.STATE_HDCP2 = 0;
				iTE6805_DATA.STATE_HDCP1 = 1;
			}
		} else {
			HDMIRX_DEBUG_PRINT("HDCP Encryption OFF !\n");
			iTE6805_DATA.STATE_HDCP1 = 0;
			iTE6805_DATA.STATE_HDCP2 = 0;
		}
	}

	if (Reg10h != 0x00) {
		if (Reg10h & 0x80) {
			if (iTE6805_DATA.STATEV == STATEV_VidStable) {
				HDMIRX_DEBUG_PRINT("# Audio FIFO Error #\n");
				iTE6805_Reset_Audio_Logic();
				iTE6805_Enable_Audio_Output();
			}
		}

		if (Reg10h & 0x40) {
			HDMIRX_DEBUG_PRINT("# Audio Auto Mute #\n");
			#if defined(Enable_Audio_Compatibility) && (Enable_Audio_Compatibility == 1)
			hdmirxset(0x89, 0x0C, 0x04);
			hdmirxset(0x86, 0x0C, 0x0C);
			#endif
		}

		if ((Reg10h & 0x20) && iTE6805_DATA.Flag_VidStable_Done) {
			HDMIRX_DEBUG_PRINT("# PKT Left Mute #\n");
			iTE6805_Set_AVMute(AVMUTE_OFF);
		}

		if ((Reg10h & 0x10) && iTE6805_DATA.Flag_VidStable_Done) {
			HDMIRX_DEBUG_PRINT("# Set Mute PKT Received #\n");
			iTE6805_Set_AVMute(AVMUTE_ON);
		}

		if (Reg10h & 0x08) {
			HDMIRX_DEBUG_PRINT("# Timer Counter Tntterrupt #\n");
		}

		if (Reg10h & 0x04) {
			HDMIRX_DEBUG_PRINT("# Video Mode Changed #\n");
		}

		if (Reg10h & 0x02) {
			HDMIRX_DEBUG_PRINT("# SCDT Change #\n");
			iTE6805_INT_SCDT_Chg();
		}

		if (Reg10h & 0x01) {
			HDMIRX_DEBUG_PRINT("# Video Abnormal Interrup #\n");
		}

	}

	if (Reg11h != 0x00) {
		if (Reg11h & BIT7) {
			//HDMIRX_DEBUG_PRINT("# No General PKT 2 Received #\n"));
		}

		if (Reg11h & BIT6) {
			//HDMIRX_DEBUG_PRINT("# No General PKT 1 Received #\n"));
		}

		if (Reg11h & BIT5) {
			HDMIRX_DEBUG_PRINT("# No Audio InfoFrame Received #\n");
		}

		if (Reg11h & BIT4) {
			HDMIRX_DEBUG_PRINT("# No AVI InfoFrame Received #\n");
		}

		if (Reg11h & BIT3) {
			HDMIRX_DEBUG_PRINT("# CD Detect #\n");
			//iTE6805_Set_ColorDepth();
		}

		if (Reg11h & BIT2) {
			//HDMIRX_DEBUG_PRINT("# Vender Specific InfoFrame Detect #\n"));
			//chgbank(2);
			//HDMI_VSIF_OUI = (hdmirxrd(0x2A) << 16) + (hdmirxrd(0x29) << 8) + (hdmirxrd(0x28));
			//HDMI_VSIF_3DValid = (hdmirxrd(0x2C) & 0x01);
			//if (HDMI_VSIF_OUI == 0xC45DD8) HDMIRX_DEBUG_PRINT("Valid HDMI_VSIF ! \n");
			//else if (HDMI_VSIF_OUI == 0x0C03) HDMIRX_DEBUG_PRINT("Valid HDMI1.4 VSIF !\n");
			//else HDMIRX_DEBUG_PRINT("NOT HDMI_VSIF !  XXXXXXX \n");
			//
			//if (HDMI_VSIF_OUI == 0xC45DD8 && HDMI_VSIF_3DValid == 1)
			//	HDMIRX_DEBUG_PRINT("HDMI_VSIF with 3D_Valid ! \n");
			//else HDMIRX_DEBUG_PRINT("NOT HDMI_VSIF with 3D  Valid ! XXXXX  \n");
			//chgbank(0);
		}

		if (Reg11h & BIT1) {
			hdmirxwr(0x11, BIT1);
			//HDMIRX_DEBUG_PRINT("# ISRC2 Detect #\n");
		}

		if (Reg11h & BIT0) {
			hdmirxwr(0x11, BIT0);
			//HDMIRX_DEBUG_PRINT("# ISRC1 Detect #\n");
		}
	}

	if (Reg12h != 0x00) {
		if (Reg12h & 0x80) {
			chgbank(0);
			Reg1Ah = hdmirxrd(0x1A);
			Reg1Bh = hdmirxrd(0x1B) & 0x07;
			HDMIRX_DEBUG_PRINT("# Video Parameters Change #\n");
			HDMIRX_DEBUG_PRINT("# VidParaChange_Sts=Reg1Bh=0x%02X Reg1Ah=0x%02X\n", (int) Reg1Bh, (int) Reg1Ah);

			hdmirxwr(0x12, 0x80);// only parameter change need to clear INT here , or register 1A/1B can't be read after clear.

			// only downscale need this section
			// because parameter change need reset downscale setting when case 4096 change to 3840 but video still stable


			#if ((_MCU_8051_EVB_ == TRUE) || iTE6805_4K60_Mode == MODE_DownScale || iTE6805_4K60_Mode == MODE_EvenOdd_Plus_DownScale) // DEMO

			HActive = ((hdmirxrd(0x9E) & 0x3F) << 8) + hdmirxrd(0x9D);
			Reg1Ah = Reg1Ah & (BIT1|BIT6|BIT3); // HDE Change/H Sync Width Change/V Sync Width Change
			Reg1Bh = Reg1Bh & (BIT1);			// VDE Change/


			Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = FALSE;

			if (iTE6805_DATA.Flag_VidStable_Done == TRUE) {
				if (Flag_First_Time_VidStable_Done == TRUE) {
					Flag_First_Time_VidStable_Done = FALSE;
				} else {
					Flag_Trigger_DownScale_Reset = FALSE;
					if (Flag_FirstTimeParameterChange == TRUE) {
						Flag_FirstTimeParameterChange = FALSE;
						if ((iTE6805_CurVTiming.HActive != 0 && (iTE6805_CurVTiming.HActive != HActive)) || Reg1Ah || Reg1Bh) {
							HDMIRX_DEBUG_PRINT("HActive = %d, before HActive = %d \n", HActive, iTE6805_CurVTiming.HActive);
							Flag_Trigger_DownScale_Reset = TRUE;
						}
					} else {
						#if (ENABLE_4K_MODE_ALL_DownScaling_1080p == TRUE)
						// more check HActive/VActive
						chgbank(0);
						iTE6805_CurVTiming.HActive = ((hdmirxrd(0x9E) & 0x3F) << 8) + hdmirxrd(0x9D);
						iTE6805_CurVTiming.VActive = ((hdmirxrd(0xA5) & 0x3F) << 8) + hdmirxrd(0xA4);
						if (iTE6805_Check_4K_Resolution() || (iTE6805_CurVTiming.HActive >= 3000 && iTE6805_CurVTiming.VActive >= 2000)) {
							Flag_Trigger_DownScale_Reset = TRUE;
						}
						#else
						if (iTE6805_Check_4K_Resolution()) {
							Flag_Trigger_DownScale_Reset = TRUE;
						}
						#endif
					}
					VIDEOTIMNG_DEBUG_PRINTF(" Flag_Trigger_DownScale_Reset   = %2.2x\r\n", (int)Flag_Trigger_DownScale_Reset);

					if (Flag_Trigger_DownScale_Reset == TRUE) {
						HDMIRX_DEBUG_PRINT(" Set_DNScale in Parameter Change \n");

						iTE6805_Get_AVIInfoFrame_Info(); // add here for setting DownScale variable
						iTE6805_Get_VID_Info(); // this need time to calculate, put this here for save time and get item for setting downscale

						iTE6805_Check_ColorChange_Update();
						#if Debug_message
						iTE6805_Show_VID_Info();
						#endif

						#if (iTE68051 == TRUE)
						iTE68051_Video_Output_Setting();
						#endif

						#if (iTE68052 == TRUE)
						iTE68052_Video_Output_Setting();
						#endif

						#if (iTE6807 == TRUE)
						iTE6807_Video_Output_Setting();
						#endif

						iTE6805_Enable_Video_Output(); ///color space reset

						iTE6805_Reset_Video_Logic();

						Flag_Trigger_DownScale_Reset = FALSE;
						Flag_FirstTimeParameterChange = TRUE;
						Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = TRUE; // for parameter change mean AVI infoframe already setting here

						chgbank(2) ;
						prevAVIDB = hdmirxrd(REG_RX_AVI_DB1) ;
						chgbank(0) ;

					}

				}
			}
			#endif
		}
		if (Reg12h & 0x40) {
			HDMIRX_DEBUG_PRINT("# 3D audio Valie Change #\n");
		}

		if (Reg12h & 0x10) {
			HDMIRX_DEBUG_PRINT("# New Audio PKT Received #\n");
		}

		if (Reg12h & 0x08) {
			HDMIRX_DEBUG_PRINT("# New ACP PKT Received #\n");
		}

		if (Reg12h & 0x04) {
			HDMIRX_DEBUG_PRINT("# New SPD PKT Received #\n");
		}

		if (Reg12h & 0x02) {
			HDMIRX_DEBUG_PRINT("# New MPEG InfoFrame Received #\n");
		}

		if (Reg12h & 0x01) {
			iTE_u8 AVIDB ;
			HDMIRX_DEBUG_PRINT("# New AVI InfoFrame Received #\n");
			chgbank(2) ;
			AVIDB = hdmirxrd(REG_RX_AVI_DB1) ;   // fix to only DB1 (color space) need to compare, checksum don't needed anymore.
			chgbank(0) ;

			// some device may send different resolution  but video still stable, but it will send new AVI InfoFrame to tell you
			// so need judge New AVI InfoFrame
			if (iTE6805_DATA.Flag_VidStable_Done != TRUE) {
				HDMIRX_DEBUG_PRINT("Flag_VidStable_Done!=TRUE, Save AVI Infoframe to variable. \n");
				HDMIRX_DEBUG_PRINT("AVIDB1 = %x \n", AVIDB);
				HDMIRX_DEBUG_PRINT("prevAVIDB1 = %x \n", prevAVIDB);
				prevAVIDB = AVIDB ;
				Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = FALSE;
			} else {
				iTE6805_Check_ColorChange_Update();
				if (AVIDB != prevAVIDB) {
					prevAVIDB = AVIDB ;
					if (Flag_Disable_NewAVIInfoFrame_When_Parameter_Change == TRUE) {
						// diable Flag_NewAVIInfoFrame because video parameter change already setting on top, do not trigger this Flag for STATEV_VidStable in vid_fsm
						Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = FALSE;
						Flag_NewAVIInfoFrame = TRUE;
					} else {
						HDMIRX_DEBUG_PRINT("AVIDB1 = %x \n", AVIDB);
						HDMIRX_DEBUG_PRINT("prevAVIDB1 = %x \n", prevAVIDB);

						HDMIRX_DEBUG_PRINT("# New AVI InfoFrame Received #\n");
						Flag_NewAVIInfoFrame = TRUE;
					}

				}
			}

		}

	}

#if (defined(SPI_ENABLE)) && (SPI_ENABLE == 1) && (iTE6807 == 1)
	if (Reg1Ch&0x40 != 0x00) {
		SPI_PRINTF("EMP Packet data update : \n");
		chgbank(6);
		hdmirxbrd(0x30, 32, rddata);
		for (x = 0; x < 32; x++) {
			SPI_PRINTF("PKt0 Data %02x= %02X ;  ", x, rddata[x]);
			if (x%8 == 7) {
				SPI_PRINTF("\n");
			}
		}
		hdmirxbrd(0x31, 32, rddata);
		for (x = 0; x < 32; x++) {
			SPI_PRINTF("Pkt1 Data %02x= %02X ;  ", x, rddata[x]);
			if (x%8 == 7) {
				SPI_PRINTF("\n");
			}
		}
		hdmirxbrd(0x32, 32, rddata);
		for (x = 0; x < 32; x++) {
			SPI_PRINTF("Pkt2 Data %02x= %02X ;  ", x, rddata[x]);
			if (x%8 == 7) {
				SPI_PRINTF("\n");
			}
		}
		chgbank(0);
	}
#endif
	if (Reg1Dh != 0x00) {
		if (Reg1Dh & 0x01) {
			HDMIRX_DEBUG_PRINT("HDCP2 Authen Start \n");
			#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
			if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 1) {
				if (iTE6805_DATA.STATER_6805 == STATER_6805_Wait_For_HDCP_Done) {
					iTE6805_DATA.Flag_HDCP_NOW = 2;
					iTE6805_DATA.CB_HDCP.iTE6805_Set_DownstreamHDCPVersion_CB(2);	// setting chip call back HDCP2 enabled
					iTE6805_HDCP_Repeater_chg(STATER_6805_HDCP_AuthPart2_Start);
				}
			}
			#endif
		}
		if (Reg1Dh & 0x02) {
			HDMIRX_DEBUG_PRINT("HDCP2 Authen Done \n");
		}
		if (Reg1Dh & 0x04) {
			HDMIRX_DEBUG_PRINT("HDCP2 Off Detect \n");
		}
		if (Reg1Dh & 0x08) {
			HDMIRX_DEBUG_PRINT("HDCP Encryption Change \n");

#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
			Current_AuthStartCount = 0 ;
#endif

#if (_ENABLE_AUTO_EQ_ == TRUE)
			if ((iTE6805_Check_HDMI2() == FALSE) && Flag_HDCP_Trigger_AutoEQ_Again == TRUE) {
				Flag_HDCP_Trigger_AutoEQ_Again = FALSE;
				// when hdmi1.x 30m/10cm (10cm with EQ 0x01 will check ok but EQ still fail, so need auto EQ again when EQ1.x and only trigger once)
				if (iTE6805_BitErr_Check_Again() == FALSE) {
					EQ_DEBUG_PRINTF("******EQ Done But Find CED Error when Encryption change, Restart Manual EQ ******\n");
					iTE6805_EQ_chg(STATEEQ_ClockONDet_And_TriggerEQ);
				}
			}
#endif
		}
		if (Reg1Dh & 0x10) {
			if (RegD4h & 0x01) {
				HDMIRX_DEBUG_PRINT(" AKE_Init_Rcv \n");
			}
			if (RegD4h & 0x02) {
				HDMIRX_DEBUG_PRINT(" AKE_NoStr_Km_Rcv \n");
			}
			if (RegD4h & 0x04) {
				HDMIRX_DEBUG_PRINT(" AKE_Str_Km_Rcv \n");
			}
			if (RegD4h & 0x08) {
				HDMIRX_DEBUG_PRINT(" LC_Init_Rcv \n");
			}
			if (RegD4h & 0x10) {
				HDMIRX_DEBUG_PRINT(" SKE_Send_Eks_Rcv \n");
			}
			if (RegD4h & 0x20) {
				HDMIRX_DEBUG_PRINT(" Rpt_Send_Ack_Rcv \n");
			}
			if (RegD4h & 0x40) {
				HDMIRX_DEBUG_PRINT(" Rpt_Str_Manage_Rcv \n");
			}
			if (RegD4h & 0x80) {
				HDMIRX_DEBUG_PRINT(" RSA_Fail_pulse \n");
			}
			if (RegD5h & 0x01) {
				HDMIRX_DEBUG_PRINT(" HDCP2 Read CERT Done \n");
			}
			if (RegD5h & 0x02) {
				HDMIRX_DEBUG_PRINT(" HDCP2 Read H Done \n");
			}
			if (RegD5h & 0x04) {
				HDMIRX_DEBUG_PRINT(" HDCP2 Read Pair Done \n");
			}

			if (RegD5h & 0x08) {
				HDMIRX_DEBUG_PRINT(" HDCP2 Read L' Done \n");
			}
			if (RegD5h & 0x10) {
				HDMIRX_DEBUG_PRINT(" HDCP2 Read KSV and V' \n");
			}
			if (RegD5h & 0x20) {
				HDMIRX_DEBUG_PRINT(" Rpt_Send_Ack_Rcv \n");
			}
			if (RegD5h & 0x40) {
				HDMIRX_DEBUG_PRINT(" HDCP2 Message Read Error !! \n");
			}
			if (RegD5h & 0x80) {
				HDMIRX_DEBUG_PRINT(" ECC Re-Authen !! \n");
			}
		}
	}
}


// ***************************************************************************
// FSM
// ***************************************************************************
void iTE6805_vid_fsm(void)
{
	static int laststatev;
	if (laststatev != iTE6805_DATA.STATEV) {
		laststatev = iTE6805_DATA.STATEV;
//		HDMIRX_DEBUG_PRINT("iTE6805_DATA.STATEV = %d \n", iTE6805_DATA.STATEV);
		printinfo_s("iTE6805_DATA.STATEV = %d \n", iTE6805_DATA.STATEV);
    }
	switch (iTE6805_DATA.STATEV) {
	case STATEV_VideoOff:
		break;
	case STATEV_Unplug:
		if (iTE6805_Check_5V_State(iTE6805_DATA.CurrentPort) == MODE_5V_ON) {
			iTE6805_INT_5VPWR_Chg(iTE6805_DATA.CurrentPort);
		}

		// for detect change to MHL mode need to power on
		#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		iTE6805_Check_MHL_Mode_Change_Need_Power_On();
		#endif
		break;
	case STATEV_WaitSync:
		if (iTE6805_Check_SCDT()) {
			iTE6805_vid_chg(STATEV_CheckSync);
		} else {
			if (iTE6805_Check_CLK_Stable()) {
				Current_IPLL_ReLock_Count++;
			}

			if (Current_IPLL_ReLock_Count > Max_Current_IPLL_ReLock_Count) {
				Current_IPLL_ReLock_Count = 0;
				iTE6805_Set_47_By_TMDS();
			}
		}
		#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		iTE6805_Check_MHL_Mode_Change_Need_Power_On();
		#endif
		break;
	case STATEV_CheckSync:
		if (Current_SCDTCheck_Count == 0) {
			iTE6805_vid_chg(STATEV_VidStable);
		} else {
			Current_SCDTCheck_Count--;
		}
		break;
	case STATEV_VidStable:

		// New AVI Info Frame
		if (Flag_NewAVIInfoFrame == TRUE) {
			HDMIRX_DEBUG_PRINT("!!! New AVI InfoFrmae in STATEV_VidStable !!!\n");
			iTE6805_Get_AVIInfoFrame_Info(); // add here for setting DownScale variable
			iTE6805_Check_ColorChange_Update();
			iTE6805_Get_VID_Info(); // need this for downscale setting parameter
			iTE6805_Show_VID_Info();

			#if (iTE68051 == TRUE)
			iTE68051_Video_Output_Setting(); // DaulPixel setting/ DownScaling setting/ Demo Setting/ PIN Config Setting only for 6805A0
			#endif

			#if (iTE68052 == TRUE)
			iTE68052_Video_Output_Setting();
			#endif

			#if (iTE6807 == TRUE)
			iTE6807_Video_Output_Setting();
			#endif

			iTE6805_Set_1B0_By_PixelClock(); // 20180605 modify single output set to 0, 0713 Andrew suggest if PCLK < 25 need to set 1B0[0] to 0, else 1
			iTE6805_Enable_Video_Output();
			iTE6805_Reset_Video_Logic(); // can't put reset video logic here or vidstable will be keep reset reset reset reset .... 0330

			Flag_NewAVIInfoFrame = FALSE;
			Flag_FirstTimeParameterChange = TRUE;
			#if (iTE68051 == TRUE && _MCU_8051_EVB_ == TRUE)
			// If new avi info frame arrive , need init LVDS again, 15m line might had new AVI infoframe condition
			iTE6805_Init_6028LVDS(0); // init first 6028 chip
			iTE6805_Init_6028LVDS(1); // init first 6028 chip
			#endif
			iTE6805_Set_Video_Tristate(TRISTATE_OFF); // tristate must put after setting single/dual pixel mode, or can't judge it
		}

		// If setting AV by last stable video, and do not receive av mute clear pkt
		// because HW auto reset and clock and vid stable again
		// VDGatting will be set by last AV Mute, should check and clear again
		chgbank(0);
		if (hdmirxrd(0x4F)&BIT5 && iTE6805_Check_AVMute() == AVMUTE_OFF) {
			#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
			if (iTE6805_DATA.MHL_RAP_Content_State == RAP_CONTENT_ON) {
				// MHL RAP MSG Content setting
			#endif

				iTE6805_Set_AVMute(AVMUTE_OFF);

			#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
			}
			#endif
		}
		Current_VidStable_Count++;
		if (Current_VidStable_Count == Max_VidStable_Count) {
			Current_VidStable_Count = 0;
			if (Current_AudioSamplingFreq_ErrorCount < 6) {
				Current_AudioSamplingFreq_ErrorCount = 0;
			}
		}

		// RegForce_FS : Force Audio FS mode
		chgbank(0);
		if (!(hdmirxrd(0x81) & BIT6)) {
			// if not software force audio mode
			// need to keep update channel status prevent from 48K to 44K and somehow not trigger FIFO error and status not update
			// need to value for setting other chip audio setting
			iTE6805_DATA.Audio_Frequency = ((hdmirxrd(0xB5) & 0xC0) >> 2) + (hdmirxrd(0xB5) & 0x0F);
		}

		break;

	default:
		HDMIRX_DEBUG_PRINT("Video State Error !!!\n");
		break;
	}
}

void iTE6805_aud_fsm(void)
{
	iTE_u8 AudioB0_Status, AudioB1_Status, AudioB2_CHStatus;
	iTE_u8 eccerr = 0;
	switch (iTE6805_DATA.STATEA) {
	case STATEA_AudioOff:
		break;
	case STATEA_RequestAudio:
		Flag_FirstTimeAudioMonitor = TRUE;
		AudioB0_Status = 0x00;
		AudioB1_Status = 0x00;
		AudioB2_CHStatus = 0x00;
		prevAudioB0_Status = 0x00;		// compare mask 0xF0
		prevAudioB1_Status = 0x00;		// compare mask 0xFF
		prevAudioB2_CHStatus = 0x00;	// compare mask BIT1
		iTE6805_aud_chg(STATEA_WaitForReady);
		break;
	case STATEA_WaitForReady:
		if (Current_AudioCheck_Count == 0) {
			if (iTE6805_DATA.CurrentPort == PORT0)
				chgbank(0);
			else
				chgbank(4);
			// REG0B9 : P0 Received error
			// REG4B9 : P1 Received error
			eccerr = hdmirxrd(0xB9) & 0xC0;
			chgbank(0);

			if (eccerr)
				iTE6805_Reset_Audio_Logic();

			if (iTE6805_Check_SCDT() && iTE6805_Check_AUDT()) {
				iTE6805_aud_chg(STATEA_AudioOn);
				break;
			}
			Current_AudioCheck_Count = TIMEOUT_AUDIO_CHECK_COUNT;
		} else {
			Current_AudioCheck_Count--;
		}
		break;
	case STATEA_AudioOn:
		if (Current_AudioMonitor_Count-- == 0) {
			Current_AudioMonitor_Count = TIMEOUT_AUDIO_MONITOR;
			chgbank(0);

			if (Flag_FirstTimeAudioMonitor == TRUE) {
				prevAudioB0_Status = hdmirxrd(0xB0)&0xF0;	// compare mask 0xF0
				prevAudioB1_Status = hdmirxrd(0xB1);		// compare mask 0xFF
				prevAudioB2_CHStatus = hdmirxrd(0xB2)&BIT1;	// compare mask BIT1
				Flag_FirstTimeAudioMonitor = FALSE;
				if (prevAudioB2_CHStatus == 0x00) {
					// If it is not LPCM i.e. compress audio , disable auto mute.
					hdmirxset(0x8C, BIT3, 0);
				} else {
					// LPCM , enablue HW auto mute.
					hdmirxset(0x8C, BIT3, BIT3);
				}
			} else {
				AudioB0_Status = hdmirxrd(0xB0) & 0xF0;
				AudioB1_Status = hdmirxrd(0xB1);
				AudioB2_CHStatus = hdmirxrd(0xB2) & BIT1;

				if (prevAudioB0_Status != AudioB0_Status ||
						prevAudioB1_Status != AudioB1_Status ||
						prevAudioB2_CHStatus != AudioB2_CHStatus) {
					iTE6805_aud_chg(STATEA_RequestAudio);
				}
			}
		}
		break;

	default:
		HDMIRX_DEBUG_PRINT("Audio State Error !!!\n");
		break;
	}
}

// ***************************************************************************
// Video State function
// ***************************************************************************

void iTE6805_vid_chg(STATEV_Type NewState)
{
	iTE6805_DATA.STATEV = NewState;

	switch (iTE6805_DATA.STATEV) {
	case STATEV_VideoOff:
		iTE6805_Set_HPD_Ctrl(PORT0, HPD_LOW);
		iTE6805_Set_HPD_Ctrl(PORT1, HPD_LOW); // if downstream switch off the state, it should be hold and HPD goes to off.
		break;
	case STATEV_Unplug:

		#if (iTE68051 == TRUE || iTE68052 == TRUE)
		// Auto Reset when B0
		if (iTE6805_DATA.ChipID == 0xA0 || iTE6805_DATA.ChipID == 0xA1) {
			// RD suggest START
			chgbank(0);
			hdmirxset(0xC5, BIT4, BIT4);  // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
			hdmirxset(0xC5, BIT4, 0x00);  // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes

			chgbank(4);
			hdmirxset(0xC5, BIT4, BIT4);  // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
			hdmirxset(0xC5, BIT4, 0x00); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
			chgbank(0);
		}
		#endif

		// force set SCDC Clock Ratio 1/40 and Scramble
		if (iTE6805_DATA.CurrentPort == PORT0)
			chgbank(3);
		else if (iTE6805_DATA.CurrentPort == PORT1)
			chgbank(7);
		hdmirxset(0xE5, 0x1C, 0x00);
		chgbank(0);

		Flag_First_Time_VidStable_Done = FALSE;
		Flag_FirstTimeParameterChange = FALSE;
		Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = FALSE;

		iTE6805_DATA.Flag_VidStable_Done = FALSE;
		iTE6805_DATA.Flag_DownScale = FALSE;
		iTE6805_DATA.Flag_Pixel_Mode = FALSE;
		iTE6805_DATA.Flag_IS_YUV420 = FALSE;

		#if (ENABLE_DETECT_DRM_PKT == TRUE)
		iTE6805_DATA.Flag_HAVE_DRM_PKT = FALSE;
		iTE6805_DATA.Flag_NEW_DRM_PKT_RECEIVE = FALSE;
		#endif

		prevAVIDB = 0;
		prevAudioB0_Status = 0;		// compare mask 0xF0
		prevAudioB1_Status = 0;		// compare mask 0xFF
		prevAudioB2_CHStatus = 0 ;	// compare mask BIT1
		deskewerrcnt = 0;
		Flag_HDCP_Trigger_AutoEQ_Again = TRUE;
		Current_Ch0_SymbolLockRst_Count = 0;
		Current_Ch1_SymbolLockRst_Count = 0;
		Current_Ch2_SymbolLockRst_Count = 0;
		Current_IPLL_ReLock_Count = 0;

		#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		iTE6805_DATA.MHL_RAP_Content_State = RAP_CONTENT_ON;
		iTE6805_DATA.MHL_DiscoveryDone = 0;
		Current_AuthStartCount = 0;
		#endif

		// RD suggest START
		hdmirxwr(0x08, 0x04); // port0
		hdmirxwr(0x0D, 0x04); // port1
		hdmirxwr(0x22, 0x12);
		hdmirxwr(0x22, 0x10);

		hdmirxset(0x23, 0xFD, 0xAD); // fix to AD for AVMute Issue, MHL->HDMI pattern gen will trigger keep AVMute on

#if (_ENABLE_IT6805_CEC_ == TRUE && _ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		iTE6805_CEC_INIT();   // fix to AD for AVMute Issue, MHL->HDMI pattern gen will trigger keep AVMute on
#endif
		hdmirxset(0x23, 0xFD, 0xA0);

		hdmirxset(0x2B, 0xFD, 0xAC);
		hdmirxset(0x2B, 0xFD, 0xA0);

		// RD suggest END
		HDMIRX_DEBUG_PRINT("VidState change to STATEV_Unplug state\n");
		iTE6805_Set_Video_Tristate(TRISTATE_ON);

		#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
		if (iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 1) {
			if (iTE6805_DATA.STATER_6805 == STATER_6805_Wait_For_HDCP_Done) {
				iTE6805_DATA.Flag_HDCP_NOW = 0;
				iTE6805_DATA.CB_HDCP.iTE6805_Set_DownstreamHDCPVersion_CB(0);	// setting chip call back HDCP2 enabled
			}
		}
		#endif

		break;
	case STATEV_WaitSync:
		Flag_First_Time_VidStable_Done = FALSE;
		Flag_FirstTimeParameterChange = FALSE;
		Flag_Disable_NewAVIInfoFrame_When_Parameter_Change = FALSE;
		iTE6805_DATA.Flag_VidStable_Done = FALSE;

		#if (ENABLE_DETECT_DRM_PKT == TRUE)
		iTE6805_DATA.Flag_HAVE_DRM_PKT = FALSE;
		iTE6805_DATA.Flag_NEW_DRM_PKT_RECEIVE = FALSE;
		#endif

		deskewerrcnt = 0;
		Current_IPLL_ReLock_Count = 0;
		HDMIRX_DEBUG_PRINT("VidState change to STATEV_WaitSync state\n");
		iTE6805_Set_Video_Tristate(TRISTATE_ON);
		break;
	case STATEV_CheckSync:
		HDMIRX_DEBUG_PRINT("VidState change to STATEV_CheckSync state\n");
		Current_SCDTCheck_Count = TIMEOUT_SCDT_CHECK_COUNT;
		break;
	case STATEV_VidStable:
		HDMIRX_DEBUG_PRINT("VidState change to STATEV_VidStable state\n");
		chgbank(0);
		hdmirxwr(0x90, 0x8F);	// for enable Rx Chip count

		iTE6805_Get_AVIInfoFrame_Info(); // add here for setting DownScale variable
		iTE6805_Get_VID_Info();

		iTE6805_Check_ColorChange_Update();

		#if (iTE68051 == TRUE)
		iTE68051_Video_Output_Setting(); // DaulPixel setting/ DownScaling setting/ Demo Setting/ PIN Config Setting
		#endif

		#if (iTE68052 == TRUE)
		iTE68052_Video_Output_Setting(); // DaulPixel setting/ DownScaling setting/ Demo Setting/ PIN Config Setting
		#endif

		#if (iTE6807 == TRUE)
		iTE6807_Video_Output_Setting(); // DaulPixel setting/ DownScaling setting/ Demo Setting/ PIN Config Setting
		#endif

		iTE6805_Set_1B0_By_PixelClock(); // 20180605 modify single output set to 0, 0713 Andrew suggest if PCLK < 25 need to set 1B0[0] to 0, else 1

		iTE6805_Enable_Video_Output();	// AVI Info Frame Handler , CSC Table Handler

		// need to set to AVMUTE_ON for might lost the PKT in the moment between Source-Video-In and Sink-Video-Stable/
		// and detect AVMUTE off status in the vid_fsm selection
		iTE6805_Set_AVMute(AVMUTE_ON);

		iTE6805_aud_chg(STATEA_RequestAudio);

		// 0330 fix it : no warning, because video logic reset need to reset parameter change INT and SCDT INT or it will be SCDT on->off
		// this cammand is wrong-> !!!!!! WARNING !!!!!!can't put logic reset between iTE6805_Set_AVMute and iTE6805_Set_AVMute or video tristate off then there will a SCDT off->SCDT on->SCDT off->......
		iTE6805_Reset_Video_Logic();

		iTE6805_Set_Video_Tristate(TRISTATE_OFF); // tristate must put after setting single/dual pixel mode, or can't judge it

		#if (iTE68051 == TRUE && _MCU_8051_EVB_ == TRUE)
		delay1ms(100);
		// 6028 init move from beginning to here because if init in the beginning, the DE between 6028 and 6805 can't sync
		// and the rightest V-line in the screen will be all block or white, need move 6028 init from beginning to here.
		iTE6805_Init_6028LVDS(0); // init first 6028 chip
		iTE6805_Init_6028LVDS(1); // init first 6028 chip

		iTE6805_Init_6028LVDS(0); // init first 6028 chip
		iTE6805_Init_6028LVDS(1); // init first 6028 chip
		// reset twice for ensurance ...
		#endif

		iTE6805_DATA.Flag_VidStable_Done = TRUE;
		Flag_First_Time_VidStable_Done = TRUE;

		// if this state here mean ECC error before, so if vid stable again, release EQ to EQDone state.
		// release state STATEEQ_KeepEQStateUntil5VOff to normal state
		#if (_ENABLE_AUTO_EQ_ == TRUE)
			if (iTE6805_DATA.STATEEQ == STATEEQ_KeepEQStateUntil5VOff) {
				HDMIRX_DEBUG_PRINT("Release state STATEEQ_KeepEQStateUntil5VOff to normal state(STATEEQ_EQDone)!!!\n");
				iTE6805_DATA.STATEEQ = STATEEQ_EQDone;
				iTE6805_EQ_chg(STATEEQ_EQDone);
			}
		#endif

		#if Debug_message
		iTE6805_Show_VID_Info();	// move to here for saving time to tristate off
		#endif

		break;
	default:
		HDMIRX_DEBUG_PRINT("ERROR: VidState change to Unknown state !!!\n");
		break;
	}
}


// ***************************************************************************
// Audio State function
// ***************************************************************************
void iTE6805_aud_chg(STATEA_Type NewState)
{
	if (iTE6805_DATA.STATEA == NewState)
		return;
	iTE6805_DATA.STATEA = NewState;

	switch (iTE6805_DATA.STATEA) {
	case STATEA_AudioOff:
	case STATEA_RequestAudio:
		HDMIRX_DEBUG_PRINT("AudState change to STATEA_RequestAudio state !!!\n");
		Current_AudioSamplingFreq_ErrorCount = 0;
		chgbank(0);
		hdmirxset(0x81, BIT6, 0x00);	// RegForce_FS : 0: Disable Force Audio FS mode
		iTE6805_Set_Audio_Tristate(TRISTATE_ON);
		break;
	case STATEA_WaitForReady:
		HDMIRX_DEBUG_PRINT("AudState change to STATEA_WaitForReady state !!!\n");
		chgbank(0);
		hdmirxset(0x8C, BIT4, BIT4); // set RegHWMuteClr
		hdmirxset(0x8C, BIT4, 0x00); // clear RegHWMuteClr for clear H/W Mute
		Current_AudioCheck_Count = TIMEOUT_AUDIO_CHECK_COUNT;
		break;
	case STATEA_AudioOn:
		iTE6805_Show_AUD_Info();
		HDMIRX_DEBUG_PRINT("AudState change to STATEA_AudioOn state !!!\n");
		iTE6805_Enable_Audio_Output();
		iTE6805_Set_Audio_Tristate(TRISTATE_OFF);
		break;
	default:
		 HDMIRX_DEBUG_PRINT("ERROR: AudState change to Unknown state !!!\n");
		 break;
	}
}

#if (_ENABLE_EDID_RAM_ == TRUE)
void iTE6805_Port_SetEDID(iTE_u8 SET_PORT, iTE_u8 EDID_INDEX)
{
    iTE6805_DATA.US_SetEDID_Port = SET_PORT;
    iTE6805_DATA.US_SetEDID_Index = EDID_INDEX;
    iTE6805_DATA.US_SetEDID_EnableChange = 1;
}

void iTE6805_Port_SetEDID_Body(iTE_u8 SET_PORT, iTE_u8 EDID_INDEX)
{
	if (iTE6805_DATA.US_SetEDID_EnableChange == 0) {
		return;
	}
	iTE6805_DATA.US_SetEDID_EnableChange = 0;

	if (EDID_INDEX > EDID_COUNT) {
		HDMIRX_DEBUG_PRINT("EDID table index exceeded! \n");
		return;
	}

    // EDID is same , do not do anything
	if (iTE6805_DATA.US_EDID_INDEX[SET_PORT] == EDID_INDEX) {
		HDMIRX_DEBUG_PRINT("iTE6805_Port_SetEDID called, EDID Same so do nothing! \n");
		return;
	}

	iTE6805_DATA.US_EDID_INDEX[SET_PORT] = EDID_INDEX;
	if (SET_PORT == iTE6805_DATA.CurrentPort) {
		iTE6805_EDID_RAMInitial(); // low HPD and setting EDID
		iTE6805_vid_chg(STATEV_Unplug);
		#if (_ENABLE_AUTO_EQ_ == TRUE)
		iTE6805_DATA.STATEEQ = STATEEQ_Off;
		iTE6805_EQ_chg(STATEEQ_Off);
		#endif
		delay1ms(600);
		iTE6805_INT_5VPWR_Chg(iTE6805_DATA.CurrentPort);
	}
}
#endif

void iTE6805_Port_Select(iTE_u8 ucPortSel)
{
	if (ucPortSel != PORT0 && ucPortSel != PORT1) {
		HDMIRX_DEBUG_PRINT("User Setting Current Port Warning !");
		return;
	}

	iTE6805_DATA.US_CurrentPort = ucPortSel;
	iTE6805_DATA.US_CurrentPort_EnableChange = 1;
}

// it6802PortSelect
void iTE6805_Port_Select_Body(iTE_u8 ucPortSel)
{
	if (iTE6805_DATA.US_CurrentPort_EnableChange == 0) {
		return;
	}
	HDMIRX_DEBUG_PRINT("\n\nCustomer Setting Current Port = %d ! \n\n", (int) ucPortSel);
	iTE6805_DATA.US_CurrentPort_EnableChange = 0;

	if (iTE6805_DATA.CurrentPort == ucPortSel) {
		return;
	}
	chgbank(0);
	if (ucPortSel == PORT0) {
	    hdmirxset(0x35, BIT0, 0); //select port 0
		iTE6805_Set_HPD_Ctrl(PORT1, HPD_LOW); // non main port need to set HPD low, or sometimes tx can't resend output when HPD no toggle
		#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		if (iTE6805_Check_PORT0_IS_MHL_Mode(PORT0)) {
			iTE6805_OCLK_Set(MODE_MHL);
		}
		#endif
	} else {
		hdmirxset(0x35, BIT0, 1); //select port 1

		#if (iTE68051 == TRUE || iTE68052 == TRUE)
		if (iTE6805_DATA.ChipID == 0xA0) {
			hdmirxset(0xE2, BIT0, 0x00);
		}
		#endif

		#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		iTE6805_OCLK_Set(MODE_HDMI);
		#endif
		iTE6805_Set_HPD_Ctrl(PORT0, HPD_LOW); // non main port need to set HPD low, or sometimes tx can't resend output when HPD no toggle
	}
	chgbank(0);
	iTE6805_DATA.CurrentPort = ucPortSel;

#if (_ENABLE_EDID_RAM_ == TRUE)
	// need to reset EDID only index is different
	if (iTE6805_DATA.US_EDID_INDEX[PORT0] != iTE6805_DATA.US_EDID_INDEX[PORT1]) {
		iTE6805_EDID_RAMInitial(); // add for changed edid when PORT select
	}
#endif


	#if defined(_ENABLE_6805_INT_MODE_FUNCTION_) && (_ENABLE_6805_INT_MODE_FUNCTION_ == TRUE)
	iTE6805_Set_INT_Port();
	#endif

	// clear force SCDC setting
	chgbank(3);
	hdmirxset(0xE5, 0x1C, 0x00);
	chgbank(7);
	hdmirxset(0xE5, 0x1C, 0x00);
	chgbank(0);

	// power setting , turn on main port TMDS, and turn off not main port TMDS
	chgbank(0);
	hdmirxwr(0x25, 0x00);
	if (iTE6805_DATA.CurrentPort == PORT0) {
		hdmirxwr(0x26, 0x00);
		hdmirxwr(0x27, 0x00);
		hdmirxwr(0x2A, 0x01);

		hdmirxwr(0x2D, 0xFF);
		hdmirxwr(0x2E, 0xFF);
		hdmirxwr(0x2F, 0xFF);
		hdmirxwr(0x32, 0x3E);

		chgbank(3);
		hdmirxset(0xA8, 0x08, 0x08);
		chgbank(7);
		hdmirxset(0xA8, 0x08, 0x00);
		chgbank(0);

		hdmirxset(0xC5, BIT4, BIT4); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
		delay1ms(1);
		hdmirxset(0xC5, BIT4, 0x00); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
	} else {
		//hdmirxwr(0x25, 0xDF);
		hdmirxwr(0x26, 0xFF);
		hdmirxwr(0x27, 0xFF);
		hdmirxwr(0x2A, 0x3A);

		hdmirxwr(0x2D, 0x00);
		hdmirxwr(0x2E, 0x00);
		hdmirxwr(0x2F, 0x00);
		hdmirxwr(0x32, 0x01);

		chgbank(3);
		hdmirxset(0xA8, 0x08, 0x00);	//same
		chgbank(7);
		hdmirxset(0xA8, 0x08, 0x08);	//same
		chgbank(0);

		chgbank(4);
		hdmirxset(0xC5, BIT4, BIT4); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
		delay1ms(1);
		hdmirxset(0xC5, BIT4, 0x00); // Unplug need to reset EDID, because EDID reset is also reset SCDC in hardware, or SCDC may hang in sometimes
		chgbank(0);
	}

	#if (_ENABLE_AUTO_EQ_ == TRUE)
	iTE6805_DATA.STATEEQ = STATEEQ_Off;
	iTE6805_EQ_chg(STATEEQ_Off);
	#endif
	iTE6805_INT_5VPWR_Chg(iTE6805_DATA.CurrentPort);
}


void iTE6805_Port_Reset(iTE_u8 ucPortSel)
{
    iTE6805_DATA.US_Port_Reset_EnableChange = 1;
    iTE6805_DATA.US_Port_Reset_Port = ucPortSel;
}

void iTE6805_Port_Reset_Body(iTE_u8 ucPortSel)
{
	if (iTE6805_DATA.US_Port_Reset_EnableChange == 0) {
		return;
	}

	if (ucPortSel != 0 && ucPortSel != 1) {
		iTE6805_DATA.US_Port_Reset_EnableChange = 0;
		HDMIRX_DEBUG_PRINT("\n\nCustomer PortReset need to setting Current Port to 0/1, can't be other value ! \n\n");
		return;
	}

	HDMIRX_DEBUG_PRINT("\n\nCustomer Reset Port = %d ! \n\n", (int) ucPortSel);
	iTE6805_DATA.US_Port_Reset_EnableChange = 0;

	if (iTE6805_DATA.CurrentPort != ucPortSel) {
		return;
	}

	iTE6805_Set_HPD_Ctrl(iTE6805_DATA.CurrentPort, HPD_LOW);
	iTE6805_vid_chg(STATEV_Unplug);
#if (_ENABLE_AUTO_EQ_ == TRUE)
	iTE6805_DATA.STATEEQ = STATEEQ_Off;
	iTE6805_EQ_chg(STATEEQ_Off);
#endif
	delay1ms(300);
	iTE6805_INT_5VPWR_Chg(iTE6805_DATA.CurrentPort);
}

#if (iTE68051 == TRUE)
void iTE68051_Video_Output_Setting(void)
{

	iTE6805_DATA.Flag_DownScale = FALSE;
	iTE6805_DATA.Flag_Pixel_Mode = FALSE;

	HDMIRX_DEBUG_PRINT("iTE68051_Video_Output_Setting called \n");
	chgbank(0);
	// set Dual Pixel output mode to Odd/Even Mode
	hdmirxset(0x64, BIT2, 0);
	// setting LREnable need to reset REG64[1]
	hdmirxset(0x64, BIT1, BIT1);
	hdmirxset(0x64, BIT1, 0);

	chgbank(5);
	hdmirxset(0x20, 0x40, 0x40); // reset Downscale

	chgbank(1);
	hdmirxset(0xC0, BIT0, 0x00); // setting to single pixel mode
	chgbank(0);

	iTE6805_DATA._iTE6805_4K_Mode_ = iTE6805_4K60_Mode;

	#if _MCU_8051_EVB_ || (defined(_6350_EVB_) && _6350_EVB_ == 1)
	iTE6805_EVB_4K_SET_BY_PIN();
	#endif

	if (iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd 	||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_LeftRight ||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd_Plus_DownScale) {

		if (iTE6805_Check_4K_Resolution() || iTE6805_CurVTiming.PCLK > DUAL_PIXEL_MODE_PCLK_CONDITION) {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Dual Pixel Mode !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(2);	// 2 lane for dual pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = TRUE;
		} else {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Single Pixel Mode !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(1);	// 1 lane for signle pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = FALSE;
			iTE6805_DATA._iTE6805_4K_Mode_ = MODE_EvenOdd; // not 4k default is even odd mode
		}

		chgbank(0);
		switch (iTE6805_DATA._iTE6805_4K_Mode_) {
		case MODE_EvenOdd:
			HDMIRX_DEBUG_PRINT("!! 4K MODE_EvenOdd !!\n");
			hdmirxset(0x64, BIT2, 0);
			break;
		case MODE_LeftRight:
			HDMIRX_DEBUG_PRINT("!! 4K MODE_LeftRight !!\n");
			#if (Enable_LR_Overlap == 1)
			hdmirxset(0x64, BIT2|BIT3, BIT2|BIT3);
			#else
			hdmirxset(0x64, BIT2, BIT2);
			#endif
			break;
		}

		hdmirxset(0x64, BIT1, BIT1); // setting _iTE68051_4K_Mode_ hardware suggest reset REG64[1]
		hdmirxset(0x64, BIT1, 0);
	}


	if (iTE6805_DATA._iTE6805_4K_Mode_ == MODE_DownScale ||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd_Plus_DownScale) {

#if (ENABLE_4K_MODE_ALL_DownScaling_1080p == TRUE)
		// more check HActive/VActive
		chgbank(0);
		iTE6805_CurVTiming.HActive = ((hdmirxrd(0x9E) & 0x3F) << 8) + hdmirxrd(0x9D);
		iTE6805_CurVTiming.VActive = ((hdmirxrd(0xA5) & 0x3F) << 8) + hdmirxrd(0xA4);
		if ((iTE6805_Check_4K_Resolution() || ((iTE6805_CurVTiming.HActive << iTE6805_DATA.Flag_IS_YUV420) >= 3000 && iTE6805_CurVTiming.VActive >= 2000)) && iTE6805_Check_Support4KTiming()) {
			HDMIRX_DEBUG_PRINT("!!!!!Set Video DownSacle !!!!!\n");
			iTE6805_Set_DNScale();
			iTE6805_DATA.Flag_DownScale = TRUE;
		}
#else
		if (iTE6805_Check_4K_Resolution() && iTE6805_Check_Support4KTiming()) {
			HDMIRX_DEBUG_PRINT("!!!!!Set Video DownSacle !!!!!\n");
			iTE6805_Set_DNScale();
			iTE6805_DATA.Flag_DownScale = TRUE;
		}
#endif

		if (iTE6805_CurVTiming.PCLK > DUAL_PIXEL_MODE_PCLK_CONDITION && iTE6805_DATA.Flag_DownScale == FALSE) {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Dual Pixel Mode by DUAL_PIXEL_MODE_PCLK_CONDITION !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(2);	// 2 lane for dual pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = TRUE;
		} else {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Single Pixel Mode !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(1);	// 1 lane for signle pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = FALSE;
		}
	}

	chgbank(0);
}
#endif

#if (iTE68052 == TRUE)
void iTE68052_Video_Output_Setting(void)
{
	iTE6805_DATA.Flag_DownScale = FALSE;
	iTE6805_DATA.Flag_Pixel_Mode = FALSE;

	HDMIRX_DEBUG_PRINT("iTE68052_Video_Output_Setting called \n");
	// reset to orign mode start
	chgbank(0);
	hdmirxset(0x36, BIT1, 0x00); // 0928 add for 68021 need to set this bit to 0 or some chip HDMI RX will stable but target LVDS RX keep unstable, 68051 need to set to 1, and default is 1
	// set Dual Pixel output mode to Odd/Even Mode
	hdmirxset(0x64, BIT2, 0);
	// setting LREnable need to reset REG64[1]
	hdmirxset(0x64, BIT1, BIT1);
	hdmirxset(0x64, BIT1, 0);

	chgbank(1);
	hdmirxwr(0xBD, 0x0F); // lvdsmap set BD[6:7] to 00 (seq)
	hdmirxwr(0xBD, 0x0D); // and reset BD[1]
	chgbank(0);
	// reset to orign mode end

	// Enable LVDS power and clock start
	chgbank(5);
	hdmirxwr(0xC1, 0x23);
	hdmirxwr(0xC2, 0x0B);
	hdmirxwr(0xCB, 0x0F); // 20190905 Min-Hui, YauTe suggest this value need change to 0x0F
	hdmirxset(0x20, 0x40, 0x40); // reset Downscale !!!!!
	chgbank(1);
	hdmirxset(0xC0, 0x04, 0x04); // setting to single pixel mode and Enable LVDS output module
	// Enable LVDS power and clock end

    iTE6805_DATA._iTE6805_4K_Mode_ = iTE6805_4K60_Mode;

	#if _MCU_8051_EVB_ || (defined(_6350_EVB_) && _6350_EVB_ == 1)
	iTE6805_EVB_4K_SET_BY_PIN();
	#endif

	if (iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd ||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_LeftRight ||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd_Plus_DownScale) {

		if (iTE6805_Check_4K_Resolution() || iTE6805_CurVTiming.PCLK > DUAL_PIXEL_MODE_PCLK_CONDITION) {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Dual Pixel Mode !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(4);	// 4 lane for dual pixel mode when 68052
			iTE6805_DATA.Flag_Pixel_Mode = TRUE;
		} else {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Single Pixel Mode !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(2);	// 1 lane for signle pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = FALSE;
			iTE6805_DATA._iTE6805_4K_Mode_ = MODE_EvenOdd; // not 4k default is even odd mode
		}

		chgbank(0);
		switch (iTE6805_DATA._iTE6805_4K_Mode_) {
		case MODE_EvenOdd:
			HDMIRX_DEBUG_PRINT("!! 4K MODE_EvenOdd !!\n");
			hdmirxset(0x64, BIT2, 0);

			// 68052 more setting than 68051
			chgbank(1);
			hdmirxset(0xBD, BIT6|BIT7, BIT6|BIT7);
			chgbank(0);

			break;
		case MODE_LeftRight:
			HDMIRX_DEBUG_PRINT("!! 4K MODE_LeftRight !!\n");
			#if (Enable_LR_Overlap == 1)
			hdmirxset(0x64, BIT2|BIT3, BIT2|BIT3);
			#else
			hdmirxset(0x64, BIT2, BIT2);
			#endif

			// 68052 more setting than 68051
			chgbank(1);
			hdmirxset(0xBD, BIT6|BIT7, BIT6);
			chgbank(0);
			break;
		}

		hdmirxset(0x64, BIT1, BIT1); // setting _iTE6805_4K_Mode_ hardware suggest reset REG64[1]
		hdmirxset(0x64, BIT1, 0);
	}


	if (iTE6805_DATA._iTE6805_4K_Mode_ == MODE_DownScale ||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd_Plus_DownScale) {

#if (ENABLE_4K_MODE_ALL_DownScaling_1080p == TRUE)
		// more check HActive/VActive
		chgbank(0);
		iTE6805_CurVTiming.HActive = ((hdmirxrd(0x9E) & 0x3F) << 8) + hdmirxrd(0x9D);
		iTE6805_CurVTiming.VActive = ((hdmirxrd(0xA5) & 0x3F) << 8) + hdmirxrd(0xA4);
		if ((iTE6805_Check_4K_Resolution() || (iTE6805_CurVTiming.HActive >= 3000 && iTE6805_CurVTiming.VActive >= 2000)) && iTE6805_Check_Support4KTiming()) {
			HDMIRX_DEBUG_PRINT("!!!!!Set Video DownSacle !!!!!\n");
			iTE6805_Set_DNScale();
			iTE6805_DATA.Flag_DownScale = TRUE;
		}
#else
		if (iTE6805_Check_4K_Resolution() && iTE6805_Check_Support4KTiming()) {
			HDMIRX_DEBUG_PRINT("!!!!!Set Video DownSacle !!!!!\n");
			iTE6805_Set_DNScale();
			iTE6805_DATA.Flag_DownScale = TRUE;
		}
#endif

		if (iTE6805_CurVTiming.PCLK > DUAL_PIXEL_MODE_PCLK_CONDITION && iTE6805_DATA.Flag_DownScale == FALSE) {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Dual Pixel Mode by DUAL_PIXEL_MODE_PCLK_CONDITION !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(4);	// 4 lane for dual pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = TRUE;
		} else {
			HDMIRX_DEBUG_PRINT("!!!!!! Set Single Pixel Mode !!!!!!\n");
			iTE6805_Set_LVDS_Video_Path(2);	// 2 lane for signle pixel mode
			iTE6805_DATA.Flag_Pixel_Mode = FALSE;
		}
	}


	// set color depth
	chgbank(0);
	iTE6805_CurVTiming.ColorDepth = (hdmirxrd(0x98) & 0xF0);
	VIDEOTIMNG_DEBUG_PRINTF("\n Input ColorDepth = ");
	switch (iTE6805_CurVTiming.ColorDepth) {
	case 0x00:
	case 0x40:
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x04);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set RegLVColDep = 01 \n");
		break;
	case 0x50:
	case 0x60:
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x08);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("10/12 b, set RegLVColDep = 10 \n");
		break;
	default:
		chgbank(5);
		hdmirxset(0xD1, 0x0C, 0x04);
		chgbank(0);
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set RegLVColDep = 01 \n");
		break;
	}
	// need to reset video logic or Left Screen Lan0, Lan1 might swap sometimes
	iTE6805_Reset_Video_Logic();
	chgbank(0);
}
#endif

#if (iTE6807 == TRUE)
void iTE6807_Video_Output_Setting(void)
{
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

	iTE6805_DATA.Flag_DownScale = FALSE;

	HDMIRX_DEBUG_PRINT("iTE6807_Video_Output_Setting called \n");
	HDMIRX_DEBUG_PRINT("start 6807 VBO output setting ! \n");
	chgbank(0);
	// set Dual Pixel output mode to Odd/Even Mode
	hdmirxset(0x64, BIT2, 0);
	// setting LREnable need to reset REG64[1]
	hdmirxset(0x64, BIT1, BIT1);
	hdmirxset(0x64, BIT1, 0);

	chgbank(5);
	hdmirxset(0x20, 0x40, 0x40); // reset Downscale
	hdmirxset(0xCB, 0x01, 0x00); // reset LR Mode

	// 20190917 add by mail 'RE: 6265 HDMI1.4 8-7 test 480p jitter'
	if (iTE6805_CurVTiming.PCLK < 30000) {
		hdmirxwr(0xC7, 0x49);
	} else {
		hdmirxwr(0xC7, 0x00);
	}

	chgbank(1);
	hdmirxset(0xC0, BIT0, 0x00); // setting to single pixel mode
	chgbank(0);

	iTE6805_DATA._iTE6805_4K_Mode_ = iTE6805_4K60_Mode;
	iTE6805_DATA._iTE6807_EnableTwoSectionMode_ = iTE6807_EnableTwoSectionMode;
	#if _MCU_8051_EVB_ || (defined(_6350_EVB_) && _6350_EVB_ == 1)
	iTE6805_EVB_4K_SET_BY_PIN();
	#endif

	if (iTE6805_DATA._iTE6807_EnableTwoSectionMode_) {
		HDMIRX_DEBUG_PRINT("!! Enable _iTE6807_EnableTwoSectionMode_ !!\n");
		chgbank(5);
		hdmirxset(0xCB, 0x01, 0x01);
		chgbank(0);
		#if (Enable_LR_Overlap == 1)
		hdmirxset(0x64, BIT2|BIT3, BIT2|BIT3);
		#else
		hdmirxset(0x64, BIT2, BIT2);
		#endif
	} else {
		HDMIRX_DEBUG_PRINT("!! _iTE6807_EnableTwoSectionMode_ Disabled !!\n");
		chgbank(5);
		hdmirxset(0xCB, 0x01, 0x00);
		chgbank(0);
		hdmirxset(0x64, BIT2, 0);
	}

	if (iTE6805_DATA._iTE6805_4K_Mode_ == MODE_DownScale ||
			iTE6805_DATA._iTE6805_4K_Mode_ == MODE_EvenOdd_Plus_DownScale) {

#if (ENABLE_4K_MODE_ALL_DownScaling_1080p == TRUE)
		// more check HActive/VActive
		chgbank(0);
		iTE6805_CurVTiming.HActive = ((hdmirxrd(0x9E) & 0x3F) << 8) + hdmirxrd(0x9D);
		iTE6805_CurVTiming.VActive = ((hdmirxrd(0xA5) & 0x3F) << 8) + hdmirxrd(0xA4);
		if ((iTE6805_Check_4K_Resolution() || (iTE6805_CurVTiming.HActive >= 3000 && iTE6805_CurVTiming.VActive >= 2000)) && iTE6805_Check_Support4KTiming()) {
			HDMIRX_DEBUG_PRINT("!!!!!Set Video DownSacle !!!!!\n");
			iTE6805_Set_DNScale();
			iTE6805_DATA.Flag_DownScale = TRUE;
		}
#else
		if (iTE6805_Check_4K_Resolution() && iTE6805_Check_Support4KTiming()) {
			HDMIRX_DEBUG_PRINT("!!!!!Set Video DownSacle !!!!!\n");
			iTE6805_Set_DNScale();
			iTE6805_DATA.Flag_DownScale = TRUE;
		}
#endif
	}


	if (iTE6805_DATA.Flag_DownScale == TRUE) {
		iTE6805_DATA.VBO_LaneCount = 1;
		HDMIRX_DEBUG_PRINT("!!!!!! Set VBO_LaneCount = 2 Lan !!!!!!\n");
	} else if (iTE6805_Check_4K_Resolution() || iTE6805_CurVTiming.PCLK > 320000) {
		iTE6805_DATA.VBO_LaneCount = 3;
		HDMIRX_DEBUG_PRINT("!!!!!! Set VBO_LaneCount = 8 Lan (variable iTE6805_DATA.VBO_LaneCount = 3)!!!!!!\n");
	} else if (iTE6805_CurVTiming.PCLK > 160000) {
		iTE6805_DATA.VBO_LaneCount = 2;
		HDMIRX_DEBUG_PRINT("!!!!!! Set VBO_LaneCount = 4 Lan (variable iTE6805_DATA.VBO_LaneCount = 2)!!!!!!\n");
	} else if (iTE6805_CurVTiming.PCLK > 100000) {
		iTE6805_DATA.VBO_LaneCount = 1;
		HDMIRX_DEBUG_PRINT("!!!!!! Set VBO_LaneCount = 2 Lan (variable iTE6805_DATA.VBO_LaneCount = 1)!!!!!!\n");
	} else {
		iTE6805_DATA.VBO_LaneCount = 0;
		HDMIRX_DEBUG_PRINT("!!!!!! Set VBO_LaneCount = 1 Lan (variable iTE6805_DATA.VBO_LaneCount = 0)!!!!!!\n");
	}

	chgbank(1);
	if (iTE6805_DATA.VBO_LaneCount == 0)
		hdmirxset(0xBD, 0x30, 0x00);
	else
		hdmirxset(0xBD, 0x30, 0x10);
	chgbank(0);


    // Get ColorDepth
	chgbank(0);
#if (iTE6807_Force_ByteMode == 0)
	iTE6805_CurVTiming.ColorDepth = (hdmirxrd(0x98) & 0xF0);
	VIDEOTIMNG_DEBUG_PRINTF("\n Input ColorDepth = ");
	switch (iTE6805_CurVTiming.ColorDepth) {
	case 0x00:
	case 0x40:
		iTE6805_DATA.VBO_ByteMode = 0;
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set ByteMode = 3 (variable iTE6805_DATA.VBO_ByteMode = 0)\n");
		break;
	case 0x50:
		iTE6805_DATA.VBO_ByteMode = 1;
		VIDEOTIMNG_DEBUG_PRINTF("10 b, set ByteMode = 4 (variable iTE6805_DATA.VBO_ByteMode = 1)\n");
		break;
	case 0x60:
		iTE6805_DATA.VBO_ByteMode = 2;
		VIDEOTIMNG_DEBUG_PRINTF("12 b, set ByteMode = 5 (variable iTE6805_DATA.VBO_ByteMode = 2)\n");
		break;
	default:
		iTE6805_DATA.VBO_ByteMode = 0;
		VIDEOTIMNG_DEBUG_PRINTF("8 b, set ByteMode = 3 (variable iTE6805_DATA.VBO_ByteMode = 0)\n");
		break;
	}
#else
	#if (Force_ByteMode == 3)
	iTE6805_DATA.VBO_ByteMode = 0;
	VIDEOTIMNG_DEBUG_PRINTF("Force ByteMode = 3B (variable iTE6805_DATA.VBO_ByteMode = 0)\n");
	#elif (Force_ByteMode == 4)
	iTE6805_DATA.VBO_ByteMode = 1;
	VIDEOTIMNG_DEBUG_PRINTF("Force ByteMode = 4B (variable iTE6805_DATA.VBO_ByteMode = 1)\n");
	#elif (Force_ByteMode == 5)
	iTE6805_DATA.VBO_ByteMode = 2;
	VIDEOTIMNG_DEBUG_PRINTF("Force ByteMode = 5B (variable iTE6805_DATA.VBO_ByteMode = 2)\n");
	#endif
#endif

	chgbank(5);
	hdmirxset(0xC0, 0xF0, (iTE6805_DATA.VBO_LaneCount<<4) + (iTE6805_DATA.VBO_ByteMode<<6));

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
	} while (!(Reg5DB&0x10));

	Reg5D8 = hdmirxrd(0xD8);
	Reg5D9 = hdmirxrd(0xD9);
	Reg5DA = hdmirxrd(0xDA);

	//HDMIRX_DEBUG_PRINT("VBOTX Reg5D8=%2X , Reg5D9 =%2X\n",(int) Reg5D8,(int) Reg5D9);
	//HDMIRX_DEBUG_PRINT("VBOTX StateA=%2X , StateB=%2X\n",(int) (Reg5DA&0x0F),(int) (Reg5DA&0xF0)>>4);
	//HDMIRX_DEBUG_PRINT("VBOTX Reg5DB=%2X \n",(int) Reg5DB);

	VBOCLKSpd = (Reg5DB&0x0C)>>2;

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

	#if (iTE6807 == 1) && (iTE6807_EnSSC == 1)
	iTE6807_Set_EnSSC();
	#endif
}
#endif

// Block Enable
void iTE6805_Enable_Video_Output(void)
{
	iTE_s8 REG_RX_PIXCLK_SPEED;
	_CSCMtx_Type CSCMtx_Type = CSCMtx_Unknown;

	//  return;
	//iTE6805_DATA.DumpREG = TRUE;
	// REG6B[5:4]: Reg_ColMod_Set Input color mode set 00: RGB mode - 01: YUV422 mode, 10: YUV444 mode, 11: YUV420 mode
	chgbank(0);
	if (iTE6805_Check_HDMI_OR_DVI_Mode(iTE6805_DATA.CurrentPort) == MODE_HDMI) {
		HDMIRX_DEBUG_PRINT("---- CSC HDMI mode ----\n");
		iTE6805_Get_AVIInfoFrame_Info();
		iTE6805_Show_AVIInfoFrame_Info();
		iTE6805_GET_InputType();   // add for sometimes getting DVI mode
		hdmirxset(0x6B, 0x30, iTE6805_DATA.AVIInfoFrame_Input_ColorFormat << 4);// seting input format by info frame ??? do not need ???
		hdmirxset(0x6B, 0x01, 0x00);											// 0: Input color mode auto detect , 1: Force input color mode as bit[3:2] setting
	} else {
		iTE6805_DATA.Flag_InputMode = MODE_DVI;
		HDMIRX_DEBUG_PRINT("---- CSC DVI mode ----\n");
		hdmirxset(0x6B, 0x30, 0x10); 											// seting input format to RGB
		hdmirxset(0x6B, 0x01, 0x01);											// 0: Input color mode auto detect , 1: Force input color mode as bit[3:2] setting

		if (iTE6805_DATA.CurrentPort == 1)
			chgbank(4);
		REG_RX_PIXCLK_SPEED = hdmirxrd(0x48);
		chgbank(0);

		// Set Colormetry for CSC , come from 6802 code
		if (REG_RX_PIXCLK_SPEED < 0x34)
			iTE6805_DATA.AVIInfoFrame_Colorimetry = Colormetry_ITU709;
		else
			iTE6805_DATA.AVIInfoFrame_Colorimetry = Colormetry_ITU601;
	}

	if (iTE6805_DATA.US_Flag_PYPASS_CSC == TRUE) {
		// force ouput color = input color , equal to bypass csc , do not powerdown CSC because powerdown CSC also powerdown downscale
		iTE6805_DATA.US_Output_ColorFormat = iTE6805_DATA.AVIInfoFrame_Input_ColorFormat;

		#if (ENABLE_YUV420_CONVERT_TO_RGB_WHEN_CSC_BYPASS == TRUE)
		if (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420) {
			iTE6805_DATA.US_Output_ColorFormat = Color_Format_RGB;
			HDMIRX_DEBUG_PRINT("---- CSC BYPASS mode and Input = YUV420, need to force to RGB444 ----\n");
		}
		#endif
		iTE6805_Set_TTL_Video_Path(); // for setting output color space
	}

	chgbank(0);
	hdmirxwr(0x6E, 0xE0);	// default bit4 = 0 = setting to full range, 20190403 fix to set 0xE0 or 12b low bit may cut off.
	chgbank(1);
	hdmirxwr(0x86, 0x00);
	chgbank(0);
	HDMIRX_DEBUG_PRINT("US_Output_ColorFormat = %d \n", (int) iTE6805_DATA.US_Output_ColorFormat);
	HDMIRX_DEBUG_PRINT("AVIInfoFrame_Input_ColorFormat = %d \n", (int) iTE6805_DATA.AVIInfoFrame_Input_ColorFormat);
	if ((iTE6805_DATA.US_Output_ColorFormat == Color_Format_RGB) &&
			(iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV422 ||
			 iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV444 ||
			 iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420)) {
		HDMIRX_DEBUG_PRINT("---- input YUV to output RGB ----\n");
		// input YUV to output RGB
		hdmirxset(0x6C, 0x03, 0x03);	// CSC Sel 00: bypass CSC 10: RGB to YUV 11: YUV to RGB

		#if (ENABLE_DETECT_DRM_PKT == TRUE)
		if ((iTE6805_DATA.Flag_HAVE_DRM_PKT == TRUE) && ((iTE6805_DATA.DRM_DB[1] & 0x07) > 0)) {
			CSCMtx_Type = CSCMtx_YUV2RGB_BT2020_00_255;	// for BT.2020 CSC
		} else
		#endif
		{
			if (iTE6805_DATA.AVIInfoFrame_Colorimetry == Colormetry_ITU709) {
				CSCMtx_Type = CSCMtx_YUV2RGB_ITU709_00_255;	// when 709 format always to RGB full range
			} else if (iTE6805_DATA.AVIInfoFrame_Colorimetry == Colormetry_Extend && (iTE6805_DATA.AVIInfoFrame_ExtendedColorimetry == 0x05 || iTE6805_DATA.AVIInfoFrame_ExtendedColorimetry == 0x06)) {
																									// Extend Colorimetry = BT2020  						// Extend Colorimetry = BT2020
				// this Matrix is BT2020 YUV to BT2020 RGB, not normal limit/full range RGB
				CSCMtx_Type = CSCMtx_YUV2RGB_BT2020_00_255;	// for BT.2020 CSC
			} else {
				// Colormetry_ITU601
				CSCMtx_Type = CSCMtx_YUV2RGB_ITU601_00_255;
			}
		}
	} else if (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_RGB &&
			(iTE6805_DATA.US_Output_ColorFormat == Color_Format_YUV422 ||
			 iTE6805_DATA.US_Output_ColorFormat == Color_Format_YUV444 ||
			 iTE6805_DATA.US_Output_ColorFormat == Color_Format_YUV420)) {
		HDMIRX_DEBUG_PRINT("---- input RGB to output YUV ----\n");
		// input RGB to output YUV
		hdmirxset(0x6C, 0x03, 0x02);	// CSC Sel 00: bypass CSC 10: RGB to YUV 11: YUV to RGB
		if (iTE6805_DATA.AVIInfoFrame_Colorimetry == Colormetry_ITU709) {
			if (iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange == RGB_RANGE_LIMIT) {
				CSCMtx_Type = CSCMtx_RGB2YUV_ITU709_16_235;
			} else {
				CSCMtx_Type = CSCMtx_RGB2YUV_ITU709_00_255;
			}
		} else {
			// Colormetry_ITU601
			if (iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange == RGB_RANGE_LIMIT) {
				CSCMtx_Type = CSCMtx_RGB2YUV_ITU601_16_235;
			} else {
				CSCMtx_Type = CSCMtx_RGB2YUV_ITU601_00_255;
			}
		}

	} else {
		// YUV to YUV    or   RGB to RGB
		HDMIRX_DEBUG_PRINT("---- YUV to YUV or RGB to RGB ----\n");
		hdmirxset(0x6C, 0x03, 0x00);	// CSC Sel 00: bypass CSC 10: RGB to YUV 11: YUV to RGB

#if ENABLE_RGB_LIMIT_TO_RGB_FULL_RANGE

		// do not need RGB limit to FULL range anymore, this gray color stage may not expect
		if (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_RGB) {
			if (iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange == RGB_RANGE_LIMIT) {
				// RGB Limit Range to Full Range
				iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange = RGB_RANGE_FULL;
				HDMIRX_DEBUG_PRINT("using CSCMtx_RGB_16_235_RGB_00_255\r\n\r\n\r\n");
				CSCMtx_Type = CSCMtx_RGB_16_235_RGB_00_255;
				hdmirxset(0x6E, 0x80, 0x00);	// need to disable hardware auto csc select or hardware will auto set to BYPASS
				hdmirxset(0x6C, 0x03, 0x02);	// CSC Sel 00: bypass CSC 10: RGB to YUV 11: YUV to RGB ??? come from 6802
				chgbank(1);
				hdmirxwr(0x86, 0x0F); // more setting only for RGB Full Range
				chgbank(0);
			}
		}
#endif


		// Do not need YUV Limit/Full Change
	}

	if (CSCMtx_Type != CSCMtx_Unknown) {
		HDMIRX_DEBUG_PRINT("--- CSC_TABLE != NULL --- \n");
		HDMIRX_DEBUG_PRINT("--- Using CSC_TABLE = %d --- \n", (int) CSCMtx_Type);
		chgbank(1);
		hdmirxbwr(0x70, sizeof(CSC_Matrix[0]), (iTE_u8 *)&CSC_Matrix[CSCMtx_Type][0]);
		chgbank(0);
	} else {
		// mark it lool the command blow //hdmirxset(0x4F,BIT6,BIT6); //1: power down color space conversion logic
		chgbank(1);
		hdmirxwr(0x85, 0x00); // Clear Reg_CBOffset
		chgbank(0);
	}

	//Set Video Output CD by input CD
	iTE6805_Set_ColorDepth();

	// follow 6802 (Input mode is YUV444/Input mode is YUV420 and Output mode is YUV444) or Input/Output = RGB
	// 20170119 can't power down CSC,
	// if power down CSC, Dualpixel mode LR mode/OddEven Mode will be shut down because CSC shut down
	// so mark this section.
	/*
	if (((iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV444 ||
		iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420 ) &&
		iTE6805_DATA.US_Output_ColorFormat == Color_Format_YUV444) ||
		(iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_RGB &&
		iTE6805_DATA.US_Output_ColorFormat == Color_Format_RGB)) {
		hdmirxset(0x4F, BIT6, BIT6); // power down CSC
	} else {
		hdmirxset(0x4F, BIT6, 0);	// power on CSC
	}*/

	//iTE6805_Reset_Video_FIFO();
	//iTE6805_DATA.DumpREG = FALSE;

	#if (ENABLE_YUV420_CONVERT_TO_RGB_WHEN_CSC_BYPASS == TRUE)
	if (iTE6805_DATA.US_Flag_PYPASS_CSC == TRUE && iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420) {
		iTE6805_DATA.AVIInfoFrame_Input_ColorFormat = Color_Format_RGB;
		iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange = RGB_RANGE_FULL;
	}
	#endif
}

// Block Enable End


// AudioFsCal
void iTE6805_Enable_Audio_Output(void)
{
	iTE_u16 SW_Sampling_Frequency;
	iTE_u8 Audio_CH_Status, resetAudio = 0;
	iTE_u32 sum = 0;

	chgbank(0);

	// RegForce_CTSMode : need to set to 1 for get the CTS in the PKT, 0 repersent nothing (HW using)
	// so set to 1 to get CTS in the REG2C1/2C2/2C0
	hdmirxset(0x86, BIT0, BIT0);
	chgbank(2);
	iTE6805_CurVTiming.N	= ((iTE_u32)hdmirxrd(0xBE)<<12) + ((iTE_u32)hdmirxrd(0xBF)<<4) + ((iTE_u32)hdmirxrd(0xC0)&0x0F);
	iTE6805_CurVTiming.CTS = hdmirxrd(0xC0) >> 4;
	iTE6805_CurVTiming.CTS += ((iTE_u32)hdmirxrd(0xC1)) << 12;
	iTE6805_CurVTiming.CTS += ((iTE_u32)hdmirxrd(0xC2)) << 4;

	iTE6805_Get_TMDS(10);
	HDMIRX_AUDIO_PRINTF("N = %ld \n", (long int)(iTE6805_CurVTiming.N));
	HDMIRX_AUDIO_PRINTF("CTS = %ld \n", (long int)(iTE6805_CurVTiming.CTS));
	HDMIRX_AUDIO_PRINTF("TMDSCLK = %ld \n", (long int)(iTE6805_CurVTiming.TMDSCLK));
	chgbank(0);

    sum = (iTE6805_CurVTiming.N * (iTE6805_CurVTiming.TMDSCLK));
	HDMIRX_AUDIO_PRINTF("sum = %lu \n", (long unsigned int)sum);
	// fs = N * TMDSCLK / 128 * CTS , in hdmi2.0 page 85

	if (iTE6805_CurVTiming.CTS == 0) {
		return;
	}
	SW_Sampling_Frequency = (iTE_u16) (sum/(128*iTE6805_CurVTiming.CTS));
	HDMIRX_AUDIO_PRINTF("SW caulate SW_Sampling_Frequency = %d \r\n", (int) SW_Sampling_Frequency);

	if (SW_Sampling_Frequency > 25 && SW_Sampling_Frequency <= 38) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_32K;
	} else if (SW_Sampling_Frequency > 38 && SW_Sampling_Frequency <= 45) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_44P1K;
	} else if (SW_Sampling_Frequency > 45 && SW_Sampling_Frequency <= 58) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_48K;
	} else if (SW_Sampling_Frequency > 58 && SW_Sampling_Frequency <= 78) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_64K;
	} else if (SW_Sampling_Frequency > 78 && SW_Sampling_Frequency <= 91) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_88P2K;
	} else if (SW_Sampling_Frequency > 91 && SW_Sampling_Frequency <= 106) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_96K;
	} else if (SW_Sampling_Frequency > 106 && SW_Sampling_Frequency <= 166) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_128K;
	} else if (SW_Sampling_Frequency > 166 && SW_Sampling_Frequency <= 182) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_176P4K;
	} else if (SW_Sampling_Frequency > 182 && SW_Sampling_Frequency <= 202) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_192K;
	} else if (SW_Sampling_Frequency > 224 && SW_Sampling_Frequency <= 320) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_256K;
	} else if (SW_Sampling_Frequency > 320 && SW_Sampling_Frequency <= 448) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_384K;
	} else if (SW_Sampling_Frequency > 448 && SW_Sampling_Frequency <= 638) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_512K;
	} else if (SW_Sampling_Frequency > 638 && SW_Sampling_Frequency <= 894) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_768K;
	} else if (SW_Sampling_Frequency > 894 && SW_Sampling_Frequency <= 1324) {
		iTE6805_DATA.Force_Sampling_Frequency = Audio_Sampling_1024K;
	}

	chgbank(0);
	// in the hdmi2.0 page 84, need bit 24, 25, 26, 27, 30, 31
	Audio_CH_Status = ((hdmirxrd(0xB5) & 0xC0) >> 2) + (hdmirxrd(0xB5) & 0x0F);	// Audio_CH_Status : Audio Channel status decoder value[31:24] and bit[24:27] = Audio Sampling Rate

    HDMIRX_AUDIO_PRINTF("Audio_CH_Status[24:27 - 30:31][bit0~bit5] = %x ,", (int) Audio_CH_Status);
    HDMIRX_AUDIO_PRINTF("iTE6805_DATA.Force_Sampling_Frequency %x\r\n", (int) (iTE6805_DATA.Force_Sampling_Frequency));

	// SW caulate Sampling Frequency equal to 60958-3 SPEC channel status, no need to enable Force FS mode
	if (Audio_CH_Status == iTE6805_DATA.Force_Sampling_Frequency) {
		HDMIRX_AUDIO_PRINTF("Audio_CH_Status == iTE6805_DATA.Force_Sampling_Frequency reset Audio \r\n");
		if (hdmirxrd(0x81) & BIT6)
			resetAudio = 1; // If Already Force FS Mode, Need to reset Audio;

		hdmirxset(0x81, BIT6, 0x00);	// RegForce_FS : 0: Disable Force Audio FS mode

		iTE6805_DATA.Audio_Frequency = Audio_CH_Status;

		if (resetAudio)
			iTE6805_Reset_Audio_Logic();
		Current_AudioSamplingFreq_ErrorCount = 0;
		return;
	}

	Current_AudioSamplingFreq_ErrorCount++;
    HDMIRX_AUDIO_PRINTF("Current_AudioSamplingFreq_ErrorCount=%d \r\n", (int) Current_AudioSamplingFreq_ErrorCount);

	// exceed max error count , enable Force Sampling Mode
	if (Current_AudioSamplingFreq_ErrorCount > Max_AudioSamplingFreq_ErrorCount) {
		hdmirxset(0x81, BIT6, BIT6);	// RegForce_FS : Force Audio FS mode
		hdmirxset(0x8A, 0x3F, iTE6805_DATA.Force_Sampling_Frequency); // RegFS_Set[5:0] : Software set sampling frequency

		#if defined(Enable_Audio_Compatibility) && (Enable_Audio_Compatibility == 1)
		if (SW_Sampling_Frequency <= 182) {
			hdmirxset(0x89, 0x0C, 0x04);
			hdmirxset(0x86, 0x0C, 0x0C);
		} else {
			hdmirxset(0x89, 0x0C, 0x0C);
			hdmirxset(0x86, 0x0C, 0x04);
		}
		#endif

		iTE6805_DATA.Audio_Frequency = iTE6805_DATA.Force_Sampling_Frequency;

		Current_AudioSamplingFreq_ErrorCount = 0;
		iTE6805_Reset_Audio_Logic();
		HDMIRX_AUDIO_PRINTF("ForceAudio Mode\r\n");
	}
}

//iTE_u8 LAST_5V_STATE = MODE_5V_OFF;

void iTE6805_INT_5VPWR_Chg(iTE_u8 ucport)
{

	//if( LAST_5V_STATE == iTE6805_Check_5V_State(ucport)) return;

	#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
	if (iTE6805_Check_PORT0_IS_MHL_Mode(PORT0)) {
		iTE6805_DEBUG_INT_PRINTF("#P0 = MHL Mode#\n");
		if (iTE6805_DATA.CurrentPort == PORT1) {
			iTE6805_DEBUG_INT_PRINTF("#Main Port = P1#\n");

			#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
			iTE6805_DEBUG_INT_PRINTF("# Power MODE_POWER_STANDBY #\n");
			iTE6805_Set_Power_Mode(MODE_POWER_STANDBY);
			iTE6805_POWER_MODE = MODE_POWER_STANDBY;
			#endif

			iTE6805_OCLK_Set(MODE_HDMI);
		}
	}
	#endif


	if (iTE6805_Check_5V_State(ucport)) {
		#if (_ENABLE_6805_AS_HDCP_REPEATER_CODE_ == TRUE)
		if (iTE6805_DATA.Flag_EDIDReady == 0 && iTE6805_DATA.ENABLE_6805_AS_HDCP_REPEATER == 1) {
			return;
		}
		#endif

		#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
		if (iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) == MODE_HDMI && iTE6805_POWER_MODE == MODE_POWER_SAVING) {
			EQ_DEBUG_PRINTF("# Port 0 Bus Mode : HDMI #\n");
			iTE6805_DEBUG_INT_PRINTF("# Power MODE_POWER_STANDBY #\n");
			iTE6805_Set_Power_Mode(MODE_POWER_STANDBY);
			iTE6805_POWER_MODE = MODE_POWER_STANDBY;
		}
		Flag_Need_Power_Normal_Mode_Setting = TRUE;
		#endif

		iTE6805_DEBUG_INT_PRINTF("# Power 5V ON #\n");
		//iTE6805_DATA.DumpREG = TRUE;
		iTE6805_vid_chg(STATEV_WaitSync);
		iTE6805_Set_HPD_Ctrl(ucport, HPD_HIGH);
	} else {
		iTE6805_DEBUG_INT_PRINTF("# Power 5V OFF #\n");
		iTE6805_Set_HPD_Ctrl(ucport, HPD_LOW);
		iTE6805_vid_chg(STATEV_Unplug);

		#if (_ENABLE_AUTO_EQ_ == TRUE)
		// Because add STATEEQ_KeepEQStateUntil5VOff STATE for ECC Error ,
		// need using iTE6805_DATA.STATEEQ = STATEEQ_Off; for leave that state.
		iTE6805_DATA.STATEEQ = STATEEQ_Off;
		iTE6805_EQ_chg(STATEEQ_Off);
		#endif

		#if (ENABLE_6805_POWER_SAVING_MODE == TRUE)
		iTE6805_DEBUG_INT_PRINTF("#  Power MODE_POWER_SAVING #\n");
		iTE6805_Set_Power_Mode(MODE_POWER_SAVING);
		iTE6805_POWER_MODE = MODE_POWER_SAVING;
		#endif

		#if (_ENABLE_IT6805_MHL_FUNCTION_ == TRUE)
		// fix for 20times may occur can't trigger HDMI/MHL mode change INT
		if (iTE6805_DATA.CurrentPort == PORT0 && iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) == MODE_HDMI) {
			iTE6805_OCLK_Set(MODE_HDMI);
		}
		#endif
	}
	//LAST_5V_STATE = !LAST_5V_STATE;
}

// hdmirx_INT_HDMIMode_Chg
void iTE6805_INT_HDMI_DVI_Mode_Chg(iTE_u8 ucport)
{

	if (iTE6805_Check_HDMI_OR_DVI_Mode(ucport) == MODE_HDMI) {
		if (iTE6805_DATA.STATEV == STATEV_VidStable) {
			iTE6805_Enable_Video_Output();
			iTE6805_aud_chg(STATEA_RequestAudio);
		}

		iTE6805_DEBUG_INT_PRINTF("# HDMI/DVI Mode : HDMI #\n");
	} else {
		iTE6805_aud_chg(STATEA_AudioOff);
		if (iTE6805_DATA.STATEV == STATEV_VidStable) {
			iTE6805_Enable_Video_Output();
		}
		iTE6805_DEBUG_INT_PRINTF("# HDMI/DVI Mode : DVI #\n");
	}
}

// hdmirx_INT_P0_ECC
void iTE6805_INT_ECC_ERROR(void)
{
	if ((Current_ECCError_Count++) > TIMEOUT_ECC_ERROR) {
		#if (_ENABLE_AUTO_EQ_ == TRUE)
		Current_ECCError_Count = 0;
		Current_ECCAbnormal_Count++;
		if (Current_ECCAbnormal_Count == 3) {
			Current_ECCAbnormal_Count = 0;
			// force release to EQ off ! because lock by STATEEQ_KeepEQStateUntil5VOff
			iTE6805_DEBUG_INT_PRINTF("++++++++++++++ECC ERROR TIMEOUT ++++++++++++++ Force EQ STATEEQ_Off \n");
			iTE6805_DATA.STATEEQ = STATEEQ_Off;
			iTE6805_EQ_chg(STATEEQ_Off);
			iTE6805_Reset_ALL_Logic(iTE6805_DATA.CurrentPort);
			return;
		}

		iTE6805_DEBUG_INT_PRINTF("++++++++++++++ECC ERROR TIMEOUT ++++++++++++++ Force EQ STATEEQ_KeepEQStateUntil5VOff \n");
		// Force EQ do not change anymore, only using iTE6805_DATA.STATEEQ = STATEEQ_KeepEQStateUntil5VOff here
		// other code need using iTE6805_EQ_chg(); to change state
		iTE6805_DATA.STATEEQ = STATEEQ_KeepEQStateUntil5VOff;

		// Force EQ in this state and set HPD
		iTE6805_Set_HPD_Ctrl(iTE6805_DATA.CurrentPort, HPD_LOW);
		delay1ms(100);
		iTE6805_Set_HPD_Ctrl(iTE6805_DATA.CurrentPort, HPD_HIGH);
		#endif
    }
}

iTE_u8 LAST_SCDT_STATE = SCDT_OFF;
// hdmirx_INT_SCDT_Chg
void iTE6805_INT_SCDT_Chg(void)
{
	// kuro add may add a time to check stable really change ?
	if (LAST_SCDT_STATE == iTE6805_Check_SCDT())
		return;
	LAST_SCDT_STATE = iTE6805_Check_SCDT();
    if (LAST_SCDT_STATE) {
		// emily 20170718 add for HDCP CTS
		chgbank(0);
		hdmirxset(0x40, 0x03, 0x00);
		iTE6805_DEBUG_INT_PRINTF("# SCDT ON #\n");
		iTE6805_vid_chg(STATEV_CheckSync);
	} else {
		// emily 20170718 add for HDCP CTS
		chgbank(0);
		hdmirxset(0x40, 0x03, 0x02);

		iTE6805_DEBUG_INT_PRINTF("# SCDT OFF #\n");
		iTE6805_vid_chg(STATEV_WaitSync);
		iTE6805_aud_chg(STATEA_AudioOff);
	}
}

// only using for MHL
void iTE6805_INT_HDMI_Mode_Chg(void)
{

	if (iTE6805_DATA.CurrentPort != PORT0) {
		return;
	}

	if (iTE6805_Check_PORT0_IS_MHL_Mode(PORT0) == MODE_MHL) {
		chgbank(3);
		hdmirxset(0x3A, 0x06, 0x04); // set Reg_ENMEQ
		chgbank(0);
		iTE6805_aud_chg(STATEA_AudioOff);
	} else {
		chgbank(3);
		hdmirxset(0x3A, 0x06, 0x02); // set Reg_ENHEQ
		chgbank(0);
	}
}

iTE_u8 iTE6805_Identify_Chip(void)
{
	iTE_u8 REG00;
	iTE_u8 REG01;
	iTE_u8 REG02;
	iTE_u8 REG03;

	REG00 = hdmirxrd(0x00);
	REG01 = hdmirxrd(0x01);
	REG02 = hdmirxrd(0x02);
	REG03 = hdmirxrd(0x03);
	if (REG00 != 0x54 ||
			REG01 != 0x49 ||
			(!(REG02 == 0x05 || REG02 == 0x07)) ||
			REG03 != 0x68) {
		HDMIRX_DEBUG_PRINT("This is not iTE6805/iTE6807 chip !!!\n");
		HDMIRX_DEBUG_PRINT("REG00 = %X !!!\n", (int)REG00);
		HDMIRX_DEBUG_PRINT("REG01 = %X !!!\n", (int)REG01);
		HDMIRX_DEBUG_PRINT("REG02 = %X !!!\n", (int)REG02);
		HDMIRX_DEBUG_PRINT("REG03 = %X !!!\n", (int)REG03);
		return 0;
	} else {
		HDMIRX_DEBUG_PRINT("REG00 = %X !!!\n", (int)REG00);
		HDMIRX_DEBUG_PRINT("REG01 = %X !!!\n", (int)REG01);
		HDMIRX_DEBUG_PRINT("REG02 = %X !!!\n", (int)REG02);
		HDMIRX_DEBUG_PRINT("REG03 = %X !!!\n", (int)REG03);
		return 1;
	}
}

#if (DYNAMIC_HDCP_ENABLE_DISABLE == TRUE)
void iTE6805_HDCP_Detect(void)
{
	if (iTE6805_DATA.STATE_HDCP == iTE6805_DATA.STATE_HDCP_FINAL) {
		return;
	}
	iTE6805_DATA.STATE_HDCP1 = 0;
	iTE6805_DATA.STATE_HDCP2 = 0;
	iTE6805_Set_HPD_Ctrl(iTE6805_DATA.CurrentPort, HPD_LOW);
	iTE6805_Set_HDCP(iTE6805_DATA.STATE_HDCP);
	delay1ms(300); //HPD OFF spec need at least 100ms
	iTE6805_Set_HPD_Ctrl(iTE6805_DATA.CurrentPort, HPD_HIGH);
	iTE6805_DATA.STATE_HDCP_FINAL = iTE6805_DATA.STATE_HDCP;
}
#endif

#if (ENABLE_DETECT_DRM_PKT == TRUE)
#define MAX_NO_DRM_PKT_RECEIVE_COUNT 0x06
iTE_u8 Current_count_no_DRM_PKT;
void iTE6805_DRM_Detect(void)
{
	iTE_u8 i, Reg11h, DRM_HB0, Reg12h, DRM_StatusChange = 0;
	chgbank(0);
	Reg11h = hdmirxrd(0x11) & BIT6;
	hdmirxwr(0x11, Reg11h);

	if (iTE6805_DATA.STATEV != STATEV_VidStable) {
		return;
	}

	chgbank(2);
	DRM_HB0 = hdmirxrd(0x24);
	chgbank(0);
	if (Reg11h || (DRM_HB0 == 0x00)) {

		iTE6805_DATA.Flag_HAVE_DRM_PKT = FALSE;
		Current_count_no_DRM_PKT = 0;
		//iTE6805_DEBUG_INT_PRINTF("\n\nno DRM PKT RECEIVE Reg11h = 0x%02X !!\n",(int)Reg11h);
		return;
	}

	// If check 5 times Reg11h[6] = 0 , then that's really have DRM PKT, or it not
	if (!Reg11h && iTE6805_DATA.Flag_HAVE_DRM_PKT == FALSE) {
		Current_count_no_DRM_PKT++;
		//iTE6805_DEBUG_INT_PRINTF("add Current_count_no_DRM_PKT!!\n");
		if (Current_count_no_DRM_PKT > MAX_NO_DRM_PKT_RECEIVE_COUNT) {
			// check 5 times really no Reg11h[6]
			iTE6805_DEBUG_INT_PRINTF("DRM PKT RECEIVED!\n");
			Current_count_no_DRM_PKT = 0;
			iTE6805_DATA.Flag_HAVE_DRM_PKT = TRUE;
			DRM_StatusChange = 1;
		}
	}

	if (iTE6805_DATA.Flag_HAVE_DRM_PKT) {
		Reg12h = hdmirxrd(0x12);
		hdmirxwr(0x12, Reg12h&0x20);
		if ((Reg12h&0x20) || DRM_StatusChange) {
			HDMIRX_DEBUG_PRINT("# New DRM PKT Received #\n");

			iTE6805_DATA.Flag_NEW_DRM_PKT_RECEIVE = TRUE;

			chgbank(2);
			iTE6805_DATA.DRM_HB[0] = hdmirxrd(0x24);
			iTE6805_DATA.DRM_HB[1] = hdmirxrd(0x25);
			iTE6805_DATA.DRM_HB[2] = hdmirxrd(0x26);	// HB2 = DB Length

			iTE6805_DEBUG_INT_PRINTF("DRM PKT RECEIVE !!\n");
			iTE6805_DEBUG_INT_PRINTF(" 0x%02X ", (int) iTE6805_DATA.DRM_HB[0]);
			iTE6805_DEBUG_INT_PRINTF(" 0x%02X ", (int) iTE6805_DATA.DRM_HB[1]);
			iTE6805_DEBUG_INT_PRINTF(" 0x%02X ", (int) iTE6805_DATA.DRM_HB[2]);
			for (i = 0; i < 28; i++) {
				iTE6805_DATA.DRM_DB[i] = hdmirxrd(0x27 + i);
				iTE6805_DEBUG_INT_PRINTF(" 0x%02X", (int) iTE6805_DATA.DRM_DB[i]);
			}
			iTE6805_DEBUG_INT_PRINTF("\n\n");

			chgbank(0);
		}

	}

}


void iTE6805_GET_DRMInfoframe(iTE_u8 *pInfoframe)
{
	// HB = 3, 0x224~226
	// PB= 28, 0x227~242
	// total = 31

	iTE_u8 i;
	*pInfoframe		= iTE6805_DATA.DRM_HB[0];
	*(pInfoframe+1) = iTE6805_DATA.DRM_HB[1];
	*(pInfoframe+2) = iTE6805_DATA.DRM_HB[2];

	for (i = 0; i < 28 ; i++) {
		*(pInfoframe + 3 + i) = iTE6805_DATA.DRM_DB[i];
	}
}

void iTE6805_CHECK_DRMInfoframe(iTE_u8 *pFlag_HAVE_DRM_PKT, iTE_u8 *pFlag_NEW_DRM_PKT_RECEIVE)
{
	*pFlag_HAVE_DRM_PKT = iTE6805_DATA.Flag_HAVE_DRM_PKT;
	*pFlag_NEW_DRM_PKT_RECEIVE = iTE6805_DATA.Flag_NEW_DRM_PKT_RECEIVE;

	if (iTE6805_DATA.Flag_HAVE_DRM_PKT == TRUE) {
		iTE6805_DATA.Flag_NEW_DRM_PKT_RECEIVE = FALSE;
	}
}

#endif

/* do not using this anymore and multi-thread may occur error
void iTE6805_GET_AVIInfoframe(iTE_u8 *pInfoframe)
{
	iTE_u8 i;
	chgbank(2);
	// AVI_Length - AVI_Version - AVI_PB0~AVI_PB15
	for(i = 0 ; i < 18 ; i++)
	{
		*(pInfoframe+i) = hdmirxrd(0x12+i);	// AVI infoFrame Length
	}
	chgbank(0);
}

void iTE6805_GET_AudioInfoframe(iTE_u8 *pInfoframe)
{
	iTE_u8 i;
	chgbank(2);
	*(pInfoframe)	= hdmirxrd(0x4A);	// Audio infoFrame Lengt
	*(pInfoframe+1) = hdmirxrd(0x43);	// AVI infoFrame Length
	// AVI_Length - AVI_Version - AVI_PB0~AVI_PB15
	for(i = 0 ; i < 6 ; i++)
	{
		*(pInfoframe+i+2) = hdmirxrd(0x44+i+2);	// AVI infoFrame Length
	}
	chgbank(0);
}

void iTE6805_GET_SPDInfoframe(iTE_u8 *pInfoframe)
{
	chgbank(2);
	*(pInfoframe)	= hdmirxrd(0x10);	// SPD infoFrame Version
	*(pInfoframe+1) = hdmirxrd(0x11);	// SPD infoFrame Data Byte 25
	chgbank(0);
}

void iTE6805_GET_VendorSpecificInfoframe(iTE_u8 *pInfoframe)
{
	iTE_u8 i;
	chgbank(2);
	// Vendor_Ver - Vendor_Length - Vendor_PB0~Vendor_PB27
	for(i = 0 ; i < 30 ; i++)
	{
		*(pInfoframe+i) = hdmirxrd(0xDA+i);
	}
	chgbank(0);
}
*/

void iTE6805_GET_DownScale_Flag(iTE_u8 *pFlag_DownScale)
{
	*pFlag_DownScale = iTE6805_DATA.Flag_DownScale;
}

void iTE6805_GET_HDMI_ColorType(iTE_u8 *pFlag_ColorType)
{
	//RGB444 = 0,
	//YUV422 = 1,
	//YUV444 = 2,
	*pFlag_ColorType = (iTE_u8)iTE6805_DATA.US_Output_ColorFormat;
}

void iTE6805_GET_Pixel_Mode(iTE_u8 *pFlag_Pixel_Mode)
{
	// Single Pixel Mode = 0;
	// Dual Pixel Mode = 1;
	*pFlag_Pixel_Mode = iTE6805_DATA.Flag_Pixel_Mode ;
}

void iTE6805_GET_Input_Type(iTE_u8 *pFlag_Input_Type)
{
    // change for multi-thread Issue
    *pFlag_Input_Type = iTE6805_DATA.Flag_InputMode;

    /*
	iTE_u8 INPUT_TYPE = MODE_HDMI;
	if(iTE6805_DATA.CurrentPort == PORT0)
	{
		if(iTE6805_Check_PORT0_IS_MHL_Mode(PORT0))
		{
			INPUT_TYPE = MODE_MHL;
		}
		else
		{
			if(iTE6805_Check_HDMI_OR_DVI_Mode(iTE6805_DATA.CurrentPort) == MODE_HDMI)
			{
				INPUT_TYPE = MODE_HDMI;
			}
			else
			{
				INPUT_TYPE = MODE_DVI;
			}
		}
	}
	else
	{
		if(iTE6805_Check_HDMI_OR_DVI_Mode(iTE6805_DATA.CurrentPort) == MODE_HDMI)
		{
			INPUT_TYPE = MODE_HDMI;
		}
		else
		{
			INPUT_TYPE = MODE_DVI;
		}
	}
	*pFlag_Input_Type = INPUT_TYPE;
     */
}

void iTE6805_GET_Input_Color(iTE_u8 *pFlag_Input_Color)
{
	//Color_Format_RGB		0
	//Color_Format_YUV422	1
	//Color_Format_YUV444	2
	//Color_Format_YUV420	3
	*pFlag_Input_Color = (iTE_u8)iTE6805_DATA.AVIInfoFrame_Input_ColorFormat;
}

void iTE6805_GET_Input_HActive(iTE_u32 *pFlag_Input_HActive)
{
	// If Below parameter need to send to customer upper layer, need to x = x*2 when YUV420
	//(iTE6805_CurVTiming.HFrontPorch)
	//(iTE6805_CurVTiming.HSyncWidth)
	//(iTE6805_CurVTiming.HBackPorch)
	//(iTE6805_CurVTiming.HActive)
	if (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420 || iTE6805_DATA.Flag_IS_YUV420 == TRUE) {
		*pFlag_Input_HActive = iTE6805_CurVTiming.HActive*2;
	} else {
		*pFlag_Input_HActive = iTE6805_CurVTiming.HActive;
	}

}

void iTE6805_GET_EQ_Result(iTE_u8 *pEQ_B_Channel, iTE_u8 *pEQ_G_Channel, iTE_u8 *pEQ_R_Channel)
{
	*pEQ_B_Channel = iTE6805_DATA.EQ_Result[0];
	*pEQ_G_Channel = iTE6805_DATA.EQ_Result[1];
	*pEQ_R_Channel = iTE6805_DATA.EQ_Result[2];
}

iTE_u8 iTE6805_GET_Input_Is_LimitRange(void)
{
	if (iTE6805_DATA.Flag_InputMode == MODE_HDMI) {
		if (iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV422 ||
				iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV444 ||
				iTE6805_DATA.AVIInfoFrame_Input_ColorFormat == Color_Format_YUV420) {
			if (iTE6805_DATA.AVIInfoFrame_YUVQuantizationRange == YUV_RANGE_FULL) {
				return FALSE;
			} else {
				return TRUE;
			}
		} else {
			if (iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange == RGB_RANGE_FULL) {
				return FALSE;
			} else {
				return TRUE;
			}
		}
	} else {
		return FALSE;
	}
}

#if ENABLE_DETECT_VSIF_PKT
void iTE6805_Detect3DFrame(void)
{
	DIG_DE3DFRAME s_Current3DFr;

    chgbank(2);
    s_Current3DFr.VIC = hdmirxrd(0x18);	// AVI_PB4
    s_Current3DFr.HB0 = 0x81;
    s_Current3DFr.HB1 = hdmirxrd(0xDA);	// Vendor_Ver
    s_Current3DFr.HB2 = hdmirxrd(0xDB);	// Vendor_Length
    s_Current3DFr.PB0 = hdmirxrd(0xDC);	// Vendor_PB0
    s_Current3DFr.PB1 = hdmirxrd(0xDD);
    s_Current3DFr.PB2 = hdmirxrd(0xDE);
    s_Current3DFr.PB3 = hdmirxrd(0xDF);
    s_Current3DFr.PB4 = hdmirxrd(0xE0);
    s_Current3DFr.PB5 = hdmirxrd(0xE1);
    s_Current3DFr.PB6 = hdmirxrd(0xE2);
    s_Current3DFr.PB7 = hdmirxrd(0xE3);
    chgbank(0);

	//HDMIRX_VIDEO_PRINTF("HDMI: VSIF:");
	//HDMIRX_VIDEO_PRINTF("%X", s_Current3DFr.PB1);
	//HDMIRX_VIDEO_PRINTF("%X", s_Current3DFr.PB2);
	//HDMIRX_VIDEO_PRINTF("%X", s_Current3DFr.PB3);
	////HDMIRX_VIDEO_PRINTF("%X", s_Current3DFr.PB4);

	// return VSDB to Customer using call back, detect by Customer
	//if ((s_Current3DFr.PB1 == 0x03) && (s_Current3DFr.PB2 == 0x0C) && (s_Current3DFr.PB3 == 0x00)) {

		//if ((s_Current3DFr.PB4&0x40) == 0x40) {
			// Customer call back function here , 3D detect

			/***********************
			 * Customer callback function setting here for detect 3D info by spec
			 * //DIG_InfoFramePacketHandler(s_Current3DFr);
			 ***********************/
		//}
	//}
	return;
}
#endif

iTE_u8 iTE6805_Check_ColorChange(void)
{
	iTE_u8 temp ;
	temp = iTE6805_DATA.Flag_AVI_ColorChange;
	if (iTE6805_DATA.Flag_AVI_ColorChange != 0x00) {
		HDMIRX_VIDEO_PRINTF("iTE6805_DATA.Flag_AVI_ColorChange = 0x%02X !\n", iTE6805_DATA.Flag_AVI_ColorChange);
		iTE6805_DATA.Flag_AVI_ColorChange = 0x00;
	}
	return temp;
}

iTE_u8 bAVIInfoFrame_Input_ColorFormat;
iTE_u8 bAVIInfoFrame_Colorimetry;
iTE_u8 bAVIInfoFrame_ExtendedColorimetry;
iTE_u8 bAVIInfoFrame_RGBQuantizationRange ;
iTE_u8 bAVIInfoFrame_YUVQuantizationRange;

void iTE6805_Check_ColorChange_Update(void)
{
	iTE6805_DATA.Flag_AVI_ColorChange = 0x00;

	if (bAVIInfoFrame_Input_ColorFormat != iTE6805_DATA.AVIInfoFrame_Input_ColorFormat) {
		iTE6805_DATA.Flag_AVI_ColorChange |= 0x01;
	}
	if (bAVIInfoFrame_Colorimetry != iTE6805_DATA.AVIInfoFrame_Colorimetry) {
		iTE6805_DATA.Flag_AVI_ColorChange |= 0x02;
	}
	if (bAVIInfoFrame_ExtendedColorimetry != iTE6805_DATA.AVIInfoFrame_ExtendedColorimetry) {
		iTE6805_DATA.Flag_AVI_ColorChange |= 0x04;
	}
	if (bAVIInfoFrame_RGBQuantizationRange != iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange) {
		iTE6805_DATA.Flag_AVI_ColorChange |= 0x08;
	}
	if (bAVIInfoFrame_YUVQuantizationRange != iTE6805_DATA.AVIInfoFrame_YUVQuantizationRange) {
		iTE6805_DATA.Flag_AVI_ColorChange |= 0x10;
	}

	if (iTE6805_DATA.Flag_AVI_ColorChange != 0x00) {
		HDMIRX_VIDEO_PRINTF("iTE6805_DATA.Flag_AVI_ColorChange = 0x%02X !\n", (int)iTE6805_DATA.Flag_AVI_ColorChange);
		bAVIInfoFrame_Input_ColorFormat		= iTE6805_DATA.AVIInfoFrame_Input_ColorFormat;
		bAVIInfoFrame_Colorimetry			= iTE6805_DATA.AVIInfoFrame_Colorimetry;
		bAVIInfoFrame_RGBQuantizationRange	= iTE6805_DATA.AVIInfoFrame_RGBQuantizationRange;
		bAVIInfoFrame_YUVQuantizationRange	= iTE6805_DATA.AVIInfoFrame_YUVQuantizationRange;
		bAVIInfoFrame_ExtendedColorimetry	= iTE6805_DATA.AVIInfoFrame_ExtendedColorimetry;
	}
}

#if (_ENABLE_EXTERN_EQ_CTRL_ == TRUE)
void iTE6805_Set_EQ_LEVEL(iTE_u8 PORT, EQ_LEVEL EQ)
{
    if (PORT != PORT0 && PORT != PORT1) {
		return;
	}

    iTE6805_DATA.EQ_EnableChange = 1;
    iTE6805_DATA.EQ_US_PORT = PORT;
    iTE6805_DATA.EQ_US_EQ_LEVEL = EQ;
}


void iTE6805_Set_EQ_LEVEL_Body(iTE_u8 PORT, EQ_LEVEL EQ)
{
    if (iTE6805_DATA.EQ_EnableChange == 0) {
		return;
	}
	iTE6805_DATA.EQ_EnableChange == 0;

	chgbank(0);
	iTE6805_DATA.EQ_Customer_Setting[PORT] = (iTE_u8) EQ;

	// Force Reset
	iTE6805_DATA.STATEEQ = STATEEQ_Off;
	iTE6805_EQ_chg(STATEEQ_Off);

	if (iTE6805_DATA.CurrentPort != PORT) {
		return;
	}

	if (iTE6805_DATA.EQ_Customer_Setting[PORT] != EQ_AUTO) {
		EQ_DEBUG_PRINTF("******EQ change to STATEEQ_KeepEQStateUntil5VOff state because Customer not set to EQ_AUTO ******\n");
		iTE6805_DATA.STATEEQ = STATEEQ_KeepEQStateUntil5VOff;	// keep it until EQ Done
	}

	EQ_DEBUG_PRINTF("****** RESET AFE ******\n");
	if (iTE6805_DATA.CurrentPort == PORT0) {
		hdmirxset(0x23, 0x01, 0x01);
		delay1ms(250);
		hdmirxset(0x23, 0x01, 0x00);
	} else {
		hdmirxset(0x2B, 0x01, 0x01);
		delay1ms(250);
		hdmirxset(0x2B, 0x01, 0x00);
	}
}
#endif

// for Customer if need to debug ..., printf all register
void printf_regall(void)
{
	iTE_u8 j, k, ucData;
	iTE_u16 i;

	for (k = 0; k <= 7; k++) {
		chgbank(k);
		HDMIRX_DEBUG_PRINT("change to bank %d \n", (int) k);
		HDMIRX_DEBUG_PRINT("       00 01 02 03 : 04 05 06 07 : 08 09 0A 0B : 0C 0D 0E 0F\r\n");
		for (i = 0x00; i < 0xFF; i += 16) {
			if (i%64 == 0) {
				HDMIRX_DEBUG_PRINT("       -----------------------------------------------------\r\n");
			}
			HDMIRX_DEBUG_PRINT("[%02X]  ", i);
			for (j = 0; j < 16; j++) {
				ucData = hdmirxrd((iTE_u8)((i+j)&0xFF));
				HDMIRX_DEBUG_PRINT(" %02X", (int) ucData);
				if ((j == 3) || (j == 7) || (j == 11)) {
					HDMIRX_DEBUG_PRINT(" :");
				}
			}
			HDMIRX_DEBUG_PRINT("\n");
		}
		HDMIRX_DEBUG_PRINT("\n\n");
	}
	chgbank(0);
}

void printf_reg(void)
{
	iTE_u8 j, ucData;
	iTE_u16 i;


	HDMIRX_DEBUG_PRINT("       00 01 02 03 : 04 05 06 07 : 08 09 0A 0B : 0C 0D 0E 0F\r\n");
	for (i = 0x00; i < 0xFF; i += 16) {
		if (i%64 == 0) {
			HDMIRX_DEBUG_PRINT("       -----------------------------------------------------\r\n");
		}
		HDMIRX_DEBUG_PRINT("[%02X]  ", i);
		for (j = 0; j < 16; j++) {
			ucData = hdmirxrd((iTE_u8)((i + j)&0xFF));
			HDMIRX_DEBUG_PRINT(" %02X", (int) ucData);
			if ((j == 3) || (j == 7) || (j == 11)) {
				HDMIRX_DEBUG_PRINT(" :");
			}
		}
		HDMIRX_DEBUG_PRINT("\n");
	}
	HDMIRX_DEBUG_PRINT("\n\n");
}

