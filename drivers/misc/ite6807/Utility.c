///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <Utility.c>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/

#include "config.h"
#include "typedef.h"

#ifdef _MCU_8051_
#include "Mcu.h"
#include "IO.h"
#endif

#include "Utility.h"


#ifdef _MCU_8051_
_IDATA iTE_u16 ucTickCount = 0;
_IDATA iTE_u16 loopTicCount = 0;
_IDATA iTE_u16 loopTicCount2 = 0;
_IDATA iTE_u16 prevTickCount;
_IDATA iTE_u16 MsdelayCnt = 0;

void init_printf(void)
{
	SCON = 0x52;
	T2CON = 0x34;
    TR2 = FALSE;
	TL2 = BAUD_SETTING;
	TH2 = BAUD_SETTING >> 8;
	RCAP2L = BAUD_SETTING;
	RCAP2H = BAUD_SETTING >> 8;

    TR2 = TRUE;
	TI = 1;
	RI = 0;
	//EA=FALSE;
}

void initialTimer0(void)
{
	TR0 = 0; // temporarily stop timer 0

	//TMOD &= 0x0F;	// Timer 1, Mode 0, 13 bit
	//TMOD |= 0x10;	// Timer 1, Mode 1, 16 bit

	TMOD &= 0xF0;	// Timer 0, Mode 0, 13 bit
	TMOD |= 0x01;	// Timer 0, Mode 1, 16 bit

	TH0 = Tick1ms / 256;
	TL0 = Tick1ms % 256;

	TR0 = 1; 	// restart the timer
	ET0 = 1;   	// Enable Timer Interrupt 0
}

void initialTimer1(void)
{
	TR1 = 0; // temporarily stop timer 1

	//TMOD &= 0x0F;	 //Timer 1, Mode 0, 13 bit
	//TMOD |= 0x11;	 //Timer 1, Mode 1, 16 bit

	TMOD = 0x11;	 //Timer 1, Mode 1, 16 bit

	TH1 = Tick1ms / 256;
	TL1 = Tick1ms % 256;

	ucTickCount = 0;

	TR1 = 1; 	// restart the timer
	ET1 = 1;   	// Enable Timer Interrupt 0
	EA = 1;
}

void system_tick(void)
{

	TR1 = 0; // temporarily stop timer 0

	TH1 = Tick1ms / 256;
	TL1 = Tick1ms % 256;

	ucTickCount++;

	TR1 = 1; // restart the timer
}


iTE_u16 getloopTicCount(void)
{
	_IDATA iTE_u16 loopms;

	if (loopTicCount > ucTickCount) {
		loopms = (0xffff - (loopTicCount - ucTickCount));
	} else {
		loopms = (ucTickCount - loopTicCount);
	}
	loopTicCount = ucTickCount;
	//    MHLRX_DEBUG_PRINTF(" loop ms  = %u\n",loopms));
	return  loopms;
}

iTE_u16 getloopTicCount2(void)
{
	_IDATA iTE_u16 loopms2;

	if (loopTicCount2 > ucTickCount) {
		loopms2 = (0xffff - (loopTicCount2 - ucTickCount));
	} else {
		loopms2 = (ucTickCount - loopTicCount2);
	}
	loopTicCount2 = ucTickCount;

	//printf(" loop ms  = %u\n",loopms2);
	return  loopms2;
}

iTE_u16 CalTimer(iTE_u16 SetupCnt)
{
	if (SetupCnt > ucTickCount) {
		return (0xffff - (SetupCnt - ucTickCount));
	} else {
		return (ucTickCount - SetupCnt);
	}
}

iTE_u16 TimeOutCheck(iTE_u16 timer, iTE_u16 x)
{
	if (CalTimer(timer) >= x) {
		return TRUE;
	}
	return FALSE;
}


iTE_u1 IsTimeOut(iTE_u16 x)
{
	if (CalTimer(prevTickCount) >= x) {
		prevTickCount = ucTickCount;
		return TRUE;
	}
	return FALSE;
}

iTE_u16 GetCurrentVirtualTime(void)
{
	return ucTickCount;
}


void delay1ms(iTE_u16 ms)
{
	iTE_u16 ucStartTickCount, diff;
	ucStartTickCount = ucTickCount;
	do {
		if (ucTickCount < ucStartTickCount) {
			diff = 0xffff - (ucStartTickCount - ucTickCount);
		} else {
			diff = ucTickCount - ucStartTickCount;
		}
	} while (diff < ms);
}

#elif defined(WIN32)

#include <windows.h>
#include <winioctl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "..\\src\\usbi2c.h"

void
delay1ms(unsigned short ms)
{
    LARGE_INTEGER Freq;
    LARGE_INTEGER Counter;
    LARGE_INTEGER count, limit;

    QueryPerformanceFrequency(&Freq) ;
    count.QuadPart = (ULONGLONG)ms * Freq.QuadPart / (ULONGLONG)1000 ;
    QueryPerformanceCounter(&Counter) ;
    limit.QuadPart = Counter.QuadPart + count.QuadPart ;

    while (limit.QuadPart > Counter.QuadPart) {
		QueryPerformanceCounter(&Counter) ;
	}

	return ;
}

iTE_u8 i2c_write_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *wrdata, iTE_u8 DEV)
{
	return i2c_writeN(address, offset, byteno, wrdata);
}
iTE_u8 i2c_read_byte(iTE_u8 address, iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata, iTE_u8 DEV)
{
	int i = 0, j[128], k;
	k = i2c_read(address, offset, byteno, j);

	if (k) {
		for (i = 0; i < byteno; i++) {
			 rddata[i] = j[i];
		}
	}
	return k;
}
#else
void delay1ms(iTE_u16 ms)
{
    mdelay(ms);
}

#endif
