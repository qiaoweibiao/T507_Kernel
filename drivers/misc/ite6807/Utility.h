///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Utility.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/

#ifndef _Utility_h_

#define _Utility_h_
#ifdef _MCU_8051_
#include "Mcu.h"

#define CLOCK                   27000000L//22118400L//11059200L//24000000L////27000000L//12000000L //15000000L
#define MachineCycle            6//12
#define BAUD_RATE      			57600L	//57600L	//  115200L	//     19200L	//  57600L
#define BAUD_SETTING            (65536L - (CLOCK / (32L * BAUD_RATE)))


#define MICROSECONDS_TIMER(microseconds)	(((CLOCK / 1000000) * (microseconds)) / MachineCycle)
#define COUNTER1ms							MICROSECONDS_TIMER(1000)		 	// 1000 us = 1 ms
#define COUNTER10ms							MICROSECONDS_TIMER(10000)		 	// 10000 us = 10 ms
#define Tick100us							MICROSECONDS_TIMER(100)				//Timer 0 100us(IR sampling time)
#define Tick1ms								(65536 - COUNTER1ms)
#define Tick10ms							(65536 - COUNTER10ms)
#define IR_COUNTER_VALUE					(65536 - Tick100us)


void HoldSystem(void);
void DelayUS(iTE_u16 us);
void delay1ms(iTE_u16 ms);
void init_printf(void);
iTE_u16 getloopTicCount(void);
iTE_u16 getloopTicCount2(void);

iTE_u16 TimeOutCheck(iTE_u16 timer, iTE_u16 x);
iTE_u16 GetCurrentVirtualTime(void);

void initialTimer1(void);
#elif defined(WIN32)

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\\src\\usbi2c.h"

void delay1ms(iTE_u16 ms);
iTE_u8 i2c_write_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *wrdata, iTE_u8 DEV);
iTE_u8 i2c_read_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata, iTE_u8 DEV);


#else

#include <linux/delay.h>

void delay1ms(iTE_u16 ms);

#endif

#endif
