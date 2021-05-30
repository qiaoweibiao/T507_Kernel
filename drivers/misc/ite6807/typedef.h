///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <typedef.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/

///*****************************************
//  Copyright (C) 2009-2014
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <typedef.h>
//   @author Jau-Chih.Tseng@ite.com.tw
//   @date   2014/12/30
//   @fileversion: ITE_MHLRX_SAMPLE_V1.16
//******************************************/

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

// WIN32 auto define by complier in PC
#ifndef WIN32
//#define _MCU_8051_
#endif


//////////////////////////////////////////////////
// MCU 8051data type
//////////////////////////////////////////////////

#define _HPDMOS_
#ifdef _HPDMOS_
    #define PORT1_HPD_ON	0
    #define PORT1_HPD_OFF	1
#else
    #define PORT1_HPD_ON	1
    #define PORT1_HPD_OFF	0
#endif


#define FALSE 		0
#define TRUE 		1


#ifndef NULL
	#define NULL ((void *) 0)
#endif


typedef char iTE_s8, *piTE_s8;
typedef unsigned char iTE_u8, *piTE_u8;
typedef short iTE_s16, *piTE_s16;
typedef unsigned short iTE_u16, *piTE_u16;

#ifdef _MCU_8051_

	typedef unsigned long iTE_u32, *piTE_u32;
	typedef long iTE_s32, *piTE_s32;

	typedef bit iTE_u1 ;
	#define _CODE code
    #define _BIT       bit
    #define _BDATA     bdata
    #define _DATA      data
    #define _IDATA     idata
    #define _XDATA     xdata
    #define _FAR_       far
    #define _REENTRANT reentrant
#else

	typedef unsigned int  iTE_u32, *piTE_u32;
	typedef int iTE_s32, *piTE_s32;

    #ifndef BOOL_FLAG
    typedef unsigned char BOOL_FLAG ;
    #endif
    typedef BOOL_FLAG iTE_u1;

	#define _CODE const
    #define _BIT       BOOL
    #define _BDATA
    #define _DATA
    #define _IDATA
    #define _XDATA
    #define _FAR
    #define _REENTRANT
#endif




#endif // _TYPEDEF_H_
