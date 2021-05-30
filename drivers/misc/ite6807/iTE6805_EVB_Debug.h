///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_EVB_Debug.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/30
//   @fileversion: iTE6805_IT6350_SAMPLE_1.43
//******************************************/
#ifndef _ITE6805_EVB_DEBUG_H_
#define _ITE6805_EVB_DEBUG_H_

	#if _MCU_8051_EVB_
		iTE_u32 iTE6805_EVB_Ca100ms(void);
		#if (iTE68051 == TRUE)

			void iTE6805_Init_6028LVDS(iTE_u8 chip);
		#endif

		void iTE6805_Port_Detect(void);
		void iTE6805_EVB_4K_SET_BY_PIN(void);
	#endif

	#if defined(_6350_EVB_) && _6350_EVB_ == 1
	void iTE6805_EVB_4K_SET_BY_PIN(void);
	#endif

#endif
