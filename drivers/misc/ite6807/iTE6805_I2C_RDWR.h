///*****************************************
//  Copyright (C) 2009-2019
//  ITE Tech. Inc. All Rights Reserved
//  Proprietary and Confidential
///*****************************************
//   @file   <iTE6805_I2C_RDWR.h>
//   @author Kuro.Chung@ite.com.tw
//   @date   2019/09/25
//   @fileversion: iTE6805_MCUSRC_1.42
//******************************************/

typedef struct {
    iTE_u8 ucAddr;
    iTE_u8 andmask;
    iTE_u8 ucValue;
} iTE6805_INI_REG;


iTE_u1	edid_ram_wr(iTE_u8 offset, iTE_u8 byteno, iTE_u8 *p_data);
iTE_u8	lvdsrxwr(iTE_u8 RegAddr, iTE_u8 DataIn);
iTE_u8	lvdsrxset(iTE_u8 offset, iTE_u8 mask, iTE_u8 ucdata);
iTE_u8	hdmirxrd(iTE_u8 RegAddr);
iTE_u8	hdmirxwr(iTE_u8 RegAddr, iTE_u8 DataIn);
iTE_u8	hdmirxbrd(iTE_u8 RegAddr, iTE_u8 RegBytes, iTE_u8 *buffer);
iTE_u8  hdmirxset(iTE_u8 offset, iTE_u8 mask, iTE_u8 ucdata);
void	hdmirxbwr(iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata);
void	hdimrx_write_init(iTE6805_INI_REG _CODE *tdata);

#if (_ENABLE_IT6805_MHL_I2C_ == TRUE)
iTE_u8	mhlrxrd(iTE_u8 offset);
iTE_u8	mhlrxwr(iTE_u8 offset, iTE_u8 ucdata);
iTE_u8	mhlrxset(iTE_u8 offset, iTE_u8 mask, iTE_u8 ucdata);
void	mhlrxbrd(iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata);
void	mhlrxbwr(iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata);
void	mhlrx_write_init(iTE6805_INI_REG _CODE *tdata);
#endif


#if (_ENABLE_IT6805_CEC_I2C_ == TRUE)
iTE_u8	cecrd(iTE_u8 RegAddr);
iTE_u1	cecwr(iTE_u8 offset, iTE_u8 buffer);
iTE_u8	cecbrd(iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata);
void	cecbwr(iTE_u8 offset, iTE_u8 byteno, iTE_u8 *rddata);
iTE_u8	cecset(iTE_u8 offset, iTE_u8 mask, iTE_u8 ucdata);
#endif
