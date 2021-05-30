///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_HDCP_Repeater_DEFINE.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/
// reentrant define for megawin code
// Unlike most 16-bit and 32-bit microcontrollers, the 8051 is not a stack based architecture.
// When parameters do not fit into the CPU registers,
// the Keil Cx51 Compiler by default uses direct memory locations for parameter passing.
// This technique generates very efficient code but limits the parameters that can be passed to indirectly called
// functions. When parameters passed to a function via a function pointer will not fit into registers, the compiler
// cannot determine where in memory to place the parameters since the function is not known at call-time.

#define reentrant reentrant

typedef void (*_iTE6805_Get_DownstreamHDCPStatus_CB_)(unsigned char *valid, unsigned char *HDCPStatus)	reentrant;
typedef void (*_iTE6805_Set_DownstreamHDCPVersion_CB_)(unsigned char *HDCP_Version)	reentrant;

typedef struct {
		_iTE6805_Get_DownstreamHDCPStatus_CB_	iTE6805_Get_DownstreamHDCPStatus_CB;
		_iTE6805_Set_DownstreamHDCPVersion_CB_	iTE6805_Set_DownstreamHDCPVersion_CB;
} _CB6805_HDCP_;
