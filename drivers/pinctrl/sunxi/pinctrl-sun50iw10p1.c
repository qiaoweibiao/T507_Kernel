/*
 * Allwinner sun50iw10p1 SoCs pinctrl driver.
 *
 * Copyright(c) 2012-2016 Allwinnertech Co., Ltd.
 * Author: huangshuosheng <huangshuosheng@allwinnertech.com>
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/pinctrl/pinctrl.h>

#include "pinctrl-sunxi.h"

static const struct sunxi_desc_pin sun50iw10p1_pins[] = {
	//Register Name: PB_CFG0
#if defined (CONFIG_FPGA_V4_PLATFORM) || defined(CONFIG_FPGA_V7_PLATFORM)
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 0),
		SUNXI_FUNCTION(0x4, "twi0"),		/* SCK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 0),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 1),
		SUNXI_FUNCTION(0x4, "twi0"),		/* SDA */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 1),
		SUNXI_FUNCTION(0x7, "io_disabled")),
#else
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* TX */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CS */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /*  MS  */
		SUNXI_FUNCTION(0x5, "Vdevice"),     /* For Test */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 0),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* RX */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CLK */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* CK  */
		SUNXI_FUNCTION(0x5, "Vdevice"),     /* For Test */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 1),
		SUNXI_FUNCTION(0x7, "io_disabled")),
#endif
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* RTS */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MOSI */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* DO  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 2),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart2"),		/* CTS */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MISO */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* DI  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 3),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1"),		/* SCK */
		SUNXI_FUNCTION(0x3, "h_i2s0"),		/* MCLK */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* MS_GPU  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 4),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1"),		/* SDA */
		SUNXI_FUNCTION(0x3, "h_i2s0"),		/* BCLK */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* CK_GPU  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 5),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	/* for spdif */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spdif"),
		SUNXI_FUNCTION(0x3, "h_i2s0"),		/* LRCK */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* DO_GPU  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 6),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 7),              /* spdif */
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spdif"),		/* DIN */
		SUNXI_FUNCTION(0x3, "h_i2s0"),		/* DOUT0 */
		SUNXI_FUNCTION(0x4, "h_i2s0"),	        /* DIN1  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 7),
		SUNXI_FUNCTION(0x7, "io_disabled")),
#if defined (CONFIG_FPGA_V4_PLATFORM) || defined (CONFIG_FPGA_T7_PLATFORM)
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 8),
		SUNXI_FUNCTION(0x0, "uart0"),		/* TXD */
		SUNXI_FUNCTION(0x7, "uart0")),		/* TXD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 9),
		SUNXI_FUNCTION(0x1, "gpio_out"),	/* gpbo[9] */
		SUNXI_FUNCTION(0x7, "uart0")),		/* RXD */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 23),             /* BCLK */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 24),             /* LRCK */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 25),             /* DOUT0 */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 26),             /* DOUT1 */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 27),             /* DOUT2 */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 28),             /* DOUT3 */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 29),             /* MCLK */
		SUNXI_FUNCTION(0x2, "i2s0"),
		SUNXI_FUNCTION(0x3, "i2s1"),
		SUNXI_FUNCTION(0x4, "i2s2"),
		SUNXI_FUNCTION(0x5, "i2s3"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
#else
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spdif"),		/* DOUT */
		SUNXI_FUNCTION(0x3, "h_i2s0"),		/* DIN0 */
		SUNXI_FUNCTION(0x4, "h_i2s0"),	        /* DOUT1  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 8),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart0"),		/* TX */
		SUNXI_FUNCTION(0x3, "twi0"),		/* SCK */
		SUNXI_FUNCTION(0x4, "jtag0"),	        /* DI_GPU  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 9),
		SUNXI_FUNCTION(0x7, "io_disabled")),
#endif
	SUNXI_PIN(SUNXI_PINCTRL_PIN(B, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart0"),		/* RX */
		SUNXI_FUNCTION(0x3, "twi0"),		/* SDA */
		SUNXI_FUNCTION(0x4, "pwm1"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 0, 10),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	/* HOLE */
	//Register Name: PC_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* WE */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* DS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 0),  /* PC_EINT0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* ALE */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* RST */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 1),  /* PC_EINT1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CLE */
		SUNXI_FUNCTION(0x4, "spi0"),		/* MOSI */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 2),  /* PC_EINT2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CE1 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* CS0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 3),  /* PC_EINT3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* CE0 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* MISO */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 4),  /* PC_EINT4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RE */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* CLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 5),  /* PC_EINT5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RB0 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* CMD */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 6),  /* PC_EINT6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* RB1 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* CS1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 7),  /* PC_EINT7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	//Register Name: PC_CFG1
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ7 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 8),  /* PC_EINT8 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ6 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D4 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 9),  /* PC_EINT9 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ5 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 10),  /* PC_EINT10 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ4 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D5 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 11),  /* PC_EINT11 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQS */
		SUNXI_FUNCTION(0x4, "spi0"),		/* CLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 12),  /* PC_EINT12 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ3 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 13),  /* PC_EINT13 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ2 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D6 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 14),  /* PC_EINT14 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ1 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D2 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* WP */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 15),  /* PC_EINT15 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(C, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "nand0"),		/* DQ0 */
		SUNXI_FUNCTION(0x3, "sdc2"),		/* D7 */
		SUNXI_FUNCTION(0x4, "spi0"),		/* HOLD */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 1, 16),  /* PC_EINT16 */
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	//Register Name: PD_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D2 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP0 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DP0 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 0),  /* PD_EINT0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D3 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN0 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DM0 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 1),  /* PD_EINT1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D4 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP1 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DP1 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 2),  /* PD_EINT2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D5 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN1 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DM1 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 3),  /* PD_EINT3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D6 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP2 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* CKP */
		SUNXI_FUNCTION(0x5, "eink"),		/* D4 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 4),  /* PD_EINT4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D7 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN2 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* CKM */
		SUNXI_FUNCTION(0x5, "eink"),		/* D5 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 5),  /* PD_EINT5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D10 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VPC */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DP2 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D6 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 6),  /* PD_EINT6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D11 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VNC */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DM2 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D7 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 7),  /* PD_EINT7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	//Register Name: PD_CFG1
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D12 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VP3 */
		SUNXI_FUNCTION(0x4, "dsi0"),		/* DP3 */
		SUNXI_FUNCTION(0x5, "eink"),		/* D8 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 8),  /* PD_EINT8 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D13 */
		SUNXI_FUNCTION(0x3, "lvds0"),		/* VN3 */
		SUNXI_FUNCTION(0x4, "dsi0"), 		/* DM3 */
		SUNXI_FUNCTION(0x5, "eink"), 		/* D9 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 9),  /* PD_EINT9 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D14 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP0 */
		SUNXI_FUNCTION(0x4, "spi1"), 		/* CS */
		SUNXI_FUNCTION(0x5, "eink"), 		/* D10 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 10),  /* PD_EINT10 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D11 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN0 */
		SUNXI_FUNCTION(0x4, "spi1"), 		/* CLK */
		SUNXI_FUNCTION(0x5, "eink"), 		/* D11 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 11),  /* PD_EINT11 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D12 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP1 */
		SUNXI_FUNCTION(0x4, "spi1"), 		/* MOSI */
		SUNXI_FUNCTION(0x5, "eink"), 		/* D12 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 12),  /* PD_EINT12 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D13 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN1 */
		SUNXI_FUNCTION(0x4, "spi1"),		/* MISO */
		SUNXI_FUNCTION(0x5, "eink"),		/* D13 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 13),  /* PD_EINT13 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D14 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP2 */
		SUNXI_FUNCTION(0x4, "uart3"),		/* TX */
		SUNXI_FUNCTION(0x5, "eink"),		/* D14 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 14),  /* PD_EINT14 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D15 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN2 */
		SUNXI_FUNCTION(0x4, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x5, "eink"),		/* D15 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 15),  /* PD_EINT15 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	//Register Name: PD_CFG2
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D18 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VPC */
		SUNXI_FUNCTION(0x4, "uart3"),		/* RTS */
		SUNXI_FUNCTION(0x5, "eink"),		/* OEH */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 16),  /* PD_EINT16 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D19 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VNC */
		SUNXI_FUNCTION(0x4, "uart3"),		/* CTS */
		SUNXI_FUNCTION(0x5, "eink"),		/* LEH */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 17),  /* PD_EINT17 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 18),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D20 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VP3 */
		SUNXI_FUNCTION(0x4, "uart4"),		/* TX */
		SUNXI_FUNCTION(0x5, "eink"),		/* CKH */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 18),  /* PD_EINT18 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 19),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D21 */
		SUNXI_FUNCTION(0x3, "lvds1"),		/* VN3 */
		SUNXI_FUNCTION(0x4, "uart4"),		/* TX */
		SUNXI_FUNCTION(0x5, "eink"),		/* CKH */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 19),  /* PD_EINT19 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 20),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D22 */
		SUNXI_FUNCTION(0x3, "pwm2"),		/* PWM */
		SUNXI_FUNCTION(0x4, "uart4"),		/* RTS */
		SUNXI_FUNCTION(0x5, "eink"),		/* CKV */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 20),  /* PD_EINT20 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 21),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd0"),		/* D23 */
		SUNXI_FUNCTION(0x3, "pwm3"),		/* PWM */
		SUNXI_FUNCTION(0x4, "uart4"),		/* CTS */
		SUNXI_FUNCTION(0x5, "eink"),		/* MODE */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 21),  /* PD_EINT21 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 22),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "pwm1"),		/* PWM */
		SUNXI_FUNCTION(0x4, "twi0"),		/* SCK */
		SUNXI_FUNCTION(0x5, "eink"),		/* STV */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 22),  /* PD_EINT22 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(D, 23),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "pwm0"),		/* PWM */
		SUNXI_FUNCTION(0x4, "twi0"),		/* SDA */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 2, 23),  /* PD_EINT23 */
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	//Register Name: PE_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_mclk0"),  /*  MCLK  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 0),  /* PE_EINT0 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi2"),  /*  SCK  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 1),  /* PE_EINT1 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi2"),  /*  SDA  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 2),  /* PE_EINT2 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi3"),  /*  SCK  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 3),  /* PE_EINT3 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi3"),  /*  SDA  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 4),  /* PE_EINT4 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi_mclk1"),  /*  MCLK  */
		SUNXI_FUNCTION(0x3, "pll0"),  /* LOCK_DBG   */
		SUNXI_FUNCTION(0x4, "h_i2s2"),  /*  MCLK  */
		SUNXI_FUNCTION(0x5, "ledc"),  /*  LEDC  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 5),  /* PE_EINT5 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "bist0"),  /*  RESULT0  */
		SUNXI_FUNCTION(0x4, "h_i2s2"),  /*  BCLK  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 6),  /* PE_EINT6 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "csi1"),  /*  SM_VS  */
		SUNXI_FUNCTION(0x3, "bist0"),  /* RESULT1   */
		SUNXI_FUNCTION(0x4, "h_i2s2"),  /*  LRCK  */
		SUNXI_FUNCTION(0x5, "tcon0"),  /*  TRIG  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 7),  /* PE_EINT7 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	//Register Name: PE_CFG1
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "bist0"),  /* RESULT2   */
		SUNXI_FUNCTION(0x4, "h_i2s2"),  /*  DOUT0  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 8),  /* PE_EINT8 */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(E, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "bist0"),  /* RESULT3   */
		SUNXI_FUNCTION(0x4, "h_i2s2"),  /*  DIN0  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 3, 9),  /* PE_EINT9 */
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	//Register Name: PF_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),   /*  D1  */
		SUNXI_FUNCTION(0x3, "jtag0"),   /*  MS1  */
		SUNXI_FUNCTION(0x4, "jtag0"),   /*  MS_GPU  */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 0),  /*  PF_EINT0  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D0 */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* DI1 */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* DI_GPU */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 1),  /*  PF_EINT1  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* CLK */
		SUNXI_FUNCTION(0x3, "uart0"),		/* TX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 2),  /*  PF_EINT2  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* CMD */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* DO1 */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* DO_GPU */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 3),  /*  PF_EINT3  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D3 */
		SUNXI_FUNCTION(0x3, "uart0"),		/* RX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 4),  /*  PF_EINT4  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc0"),		/* D2 */
		SUNXI_FUNCTION(0x3, "jtag0"),		/* CK1 */
		SUNXI_FUNCTION(0x4, "jtag0"),		/* CK_GPU */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 5),  /*  PF_EINT5  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 4, 6),  /*  PF_EINT6  */
		SUNXI_FUNCTION(0x7, "io_disabled")),

#if defined (CONFIG_FPGA_V4_PLATFORM) || defined(CONFIG_FPGA_V7_PLATFORM)
	/* for dmic */
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 9),
		SUNXI_FUNCTION(0x2, "dmic"),		/* DMIC_CLK */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 24),
		SUNXI_FUNCTION(0x5, "spi0"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 25),
		SUNXI_FUNCTION(0x5, "spi0"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 26),
		SUNXI_FUNCTION(0x5, "spi0"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 29),
		SUNXI_FUNCTION(0x5, "spi0"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 30),
		SUNXI_FUNCTION(0x5, "spi0"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(F, 31),
		SUNXI_FUNCTION(0x5, "spi0"),
		SUNXI_FUNCTION(0x7, "io_disabled")),
#endif
	/* HOLE */
	//Register Name: PG_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* CLK */
#if defined (CONFIG_FPGA_V4_PLATFORM) || defined(CONFIG_FPGA_V7_PLATFORM)
		SUNXI_FUNCTION(0x5, "ledc"),
#endif
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 0),  /*  PG_EINT0  */
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* CMD */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 1),  /*  PG_EINT1	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 2),  /*  PG_EINT2	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 3),  /*  PG_EINT3	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 4),  /*  PG_EINT4	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "sdc1"),		/* D3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 5),  /*  PG_EINT5	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),  /* TX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 6),  /*  PG_EINT6	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* RX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 7),  /*  PG_EINT7	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* RTS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 8),  /*  PG_EINT8	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart1"),		/* CTS */
		SUNXI_FUNCTION(0x3, "h_i2s1"),		/* MCLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 9),  /*  PG_EINT9	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "h_i2s1"),		/* BCLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 10),  /*  PG_EINT10	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "h_i2s1"),		/* LRCK */
#if defined (CONFIG_FPGA_V4_PLATFORM) || defined(CONFIG_FPGA_V7_PLATFORM)
		SUNXI_FUNCTION(0x5, "ir0"),		/* SYNC */
#endif
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 11),  /*  PG_EINT11	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "h_i2s1"),		/* DOUT0 */
		SUNXI_FUNCTION(0x4, "h_i2s1"),		/* DIN1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 12),  /*  PG_EINT12	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(G, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "h_i2s1"),		/* DIN0 */
		SUNXI_FUNCTION(0x4, "h_i2s1"),		/* DOUT1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 5, 13),  /*  PG_EINT13	*/
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	//Register Name: PH_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0"),	/* SCK */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* RXD1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 0),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi0"),	/* SDA */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* RXD0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 1),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1"),	/* SCK */
		SUNXI_FUNCTION(0x3, "cpu"),	/* CUR_W */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* RXCTL */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 2),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi1"),	/* SDA */
		SUNXI_FUNCTION(0x3, "ir0"),	/* OUT */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* CLKIN */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 3),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),	/* TX */
		SUNXI_FUNCTION(0x3, "spi1"),	/* CS */
		SUNXI_FUNCTION(0x4, "cpu"),	/* CUR_W */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* TXD1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 4),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x3, "spi1"),		/* CLK */
		SUNXI_FUNCTION(0x4, "ledc"),
		SUNXI_FUNCTION(0x5, "rgmii0"),		/* TXD0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 5),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* RTS */
		SUNXI_FUNCTION(0x3, "spi1"),		/* MOSI */
		SUNXI_FUNCTION(0x4, "spdif"),		/* IN */
		SUNXI_FUNCTION(0x5, "rgmii0"),		/* TXCK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 6),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* CTS */
		SUNXI_FUNCTION(0x3, "spi1"),		/* MISO */
		SUNXI_FUNCTION(0x4, "spdif"),		/* OUT */
		SUNXI_FUNCTION(0x5, "rgmii0"),		/* TXCTL */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 7),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	//Register Name: PH_CFG1
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "dmic"),		/* CLK */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CS */
		SUNXI_FUNCTION(0x4, "h_i2s2"),		/* MCLK */
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* DIN2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 8),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "dmic"),		/* DATA0 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CLK */
		SUNXI_FUNCTION(0x4, "h_i2s2"),		/* BCLK */
		SUNXI_FUNCTION(0x5, "mdc0"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 9),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "dmic"),		/* DATA1 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MOSI */
		SUNXI_FUNCTION(0x4, "h_i2s2"),	/* LRCK */
		SUNXI_FUNCTION(0x5, "mdio0"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 10),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "dmic"),		/* DATA2 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MISO */
		SUNXI_FUNCTION(0x4, "h_i2s2"),	/* DOUT0 */
		SUNXI_FUNCTION(0x5, "h_i2s2"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 11),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "dmic"),		/* DATA3 */
		SUNXI_FUNCTION(0x3, "twi3"),		/* SCK */
		SUNXI_FUNCTION(0x4, "h_i2s2"),	/* DIN0 */
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* DOUT1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 12),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "twi3"),		/* SCK */
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* MCLK */
		SUNXI_FUNCTION(0x5, "ephy0"),	/* 25 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 13),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* BCLK */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* RXD3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 14),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* LRCK */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* RXD2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 15),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "h_i2s3"),		/* DOUT0 */
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* DIN1 */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* RXCK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 16),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x3, "h_i2s3"),		/* DOUT1 */
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* DIN0 */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* TXD3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 17),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 18),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "cir"),		/* OUT */
		SUNXI_FUNCTION(0x3, "h_i2s3"),		/* DOUT2 */
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* DIN2 */
		SUNXI_FUNCTION(0x5, "rgmii0"),	/* TXD2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 18),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(H, 19),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "cir"),		/* IN */
		SUNXI_FUNCTION(0x3, "h_i2s3"),		/* DOUT3 */
		SUNXI_FUNCTION(0x4, "h_i2s3"),	/* DIN3 */
		SUNXI_FUNCTION(0x5, "ledc"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 6, 19),
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	//Register Name: PI_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi4"),		/* SCK */
		SUNXI_FUNCTION(0x3, "uart4"),		/* TX */
		SUNXI_FUNCTION(0x4, "pwm1"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 0),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi4"),		/* SDA */
		SUNXI_FUNCTION(0x3, "uart4"),		/* RX */
		SUNXI_FUNCTION(0x4, "pwm2"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 1),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart5"),		/* TX */
		SUNXI_FUNCTION(0x3, "spi1"),		/* CS */
		SUNXI_FUNCTION(0x4, "pwm3"),
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* BCLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 2),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart5"),		/* RX */
		SUNXI_FUNCTION(0x3, "spi1"),		/* CLK */
		SUNXI_FUNCTION(0x4, "pwm4"),
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* LRCK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 3),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart5"),		/* RTS */
		SUNXI_FUNCTION(0x3, "spi1"),		/* MOSI */
		SUNXI_FUNCTION(0x4, "pwm5"),
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* DOUT0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 4),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart5"),		/* CTS */
		SUNXI_FUNCTION(0x3, "spi1"),		/* MISO */
		SUNXI_FUNCTION(0x4, "pwm6"),
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* DIN0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 5),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart6"),		/* TX */
		SUNXI_FUNCTION(0x4, "pwm7"),
		SUNXI_FUNCTION(0x5, "spi2"),		/* CS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 6),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart6"),		/* RX */
		SUNXI_FUNCTION(0x4, "pwm8"),
		SUNXI_FUNCTION(0x5, "spi2"),		/* CLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 7),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	//Register Name: PI_CFG1
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi5"),		/* SCK */
		SUNXI_FUNCTION(0x3, "cir"),		/* IN */
		SUNXI_FUNCTION(0x4, "pwm9"),
		SUNXI_FUNCTION(0x5, "spi2"),		/* MOSI */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 8),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "twi5"),		/* SDA */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* CLK */
		SUNXI_FUNCTION(0x4, "pwm10"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 9),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "spdif"),		/* OUT */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* CMD */
		SUNXI_FUNCTION(0x4, "pwm11"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 10),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* TX */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* DO */
		SUNXI_FUNCTION(0x4, "pwm12"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 11),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D1 */
		SUNXI_FUNCTION(0x4, "pwm13"),
		SUNXI_FUNCTION(0x5, "spi2"),		/* MISO */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 12),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart6"),		/* CTS */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D2 */
		SUNXI_FUNCTION(0x4, "pwm14"),
		SUNXI_FUNCTION(0x5, "h_i2s2"),		/* MCLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 13),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(I, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "uart6"),		/* RTS */
		SUNXI_FUNCTION(0x3, "sdc3"),		/* D3 */
		SUNXI_FUNCTION(0x4, "pwm15"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 7, 14),
		SUNXI_FUNCTION(0x7, "io_disabled")),

	/* HOLE */
	//Register Name: PJ_CFG0
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 0),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D0 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* DOP */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D0 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* RXD1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 0),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 1),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D1 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* DON */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D1 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* RXD0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 1),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 2),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D2 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* DIP */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D2 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* RXCTL */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 2),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 3),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D3 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* D1N */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D3 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* CLKIN */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 3),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 4),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D4 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* D2P */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D4 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* TXD1 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 4),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 5),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D5 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* D2N */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D5 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* TXD0 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 5),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 6),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D6 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* CKP */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D6 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* TXCK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 6),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 7),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D7 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* CKN */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D7 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* TXCTL */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 7),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 8),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D8 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* D3P */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D8 */
		SUNXI_FUNCTION(0x5, "mdc1"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 8),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 9),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D9 */
		SUNXI_FUNCTION(0x3, "lvds2"),		/* D3N */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D9 */
		SUNXI_FUNCTION(0x5, "mdio1"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 9),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 10),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D10 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D0P */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D10 */
		SUNXI_FUNCTION(0x5, "ephy1"),
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 10),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 11),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D11 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D0N */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D11 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* RXD3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 10),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 12),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D12 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D1P */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D12 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* RXD2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 12),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 13),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D13 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D1N */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D13 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* RXCK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 13),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 14),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D14 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D2P */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D14 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* TXD3 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 14),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 15),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D15 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D2N */
		SUNXI_FUNCTION(0x4, "eink1"),		/* D15 */
		SUNXI_FUNCTION(0x5, "rgmii1"),		/* TXD2 */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 15),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 16),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D16 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* CKP */
		SUNXI_FUNCTION(0x4, "spi1"),		/* CS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 16),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 17),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D17 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* CKN */
		SUNXI_FUNCTION(0x4, "spi1"),		/* CLK */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 17),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 18),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D18 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D3P */
		SUNXI_FUNCTION(0x4, "spi1"),		/* MOSI */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 18),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 19),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D19 */
		SUNXI_FUNCTION(0x3, "lvds3"),		/* D3N */
		SUNXI_FUNCTION(0x4, "spi1"),		/* MISO */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 19),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 20),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D20 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CS */
		SUNXI_FUNCTION(0x4, "uart3"),		/* RTS */
		SUNXI_FUNCTION(0x5, "uart2"),		/* TX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 20),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 21),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D21 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* CLK */
		SUNXI_FUNCTION(0x4, "uart3"),		/* CTS */
		SUNXI_FUNCTION(0x5, "uart2"),		/* RX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 21),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 22),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D22 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MOSI */
		SUNXI_FUNCTION(0x4, "uart3"),		/* TX */
		SUNXI_FUNCTION(0x5, "uart2"),		/* RTS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 22),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 23),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* D23 */
		SUNXI_FUNCTION(0x3, "spi2"),		/* MISO */
		SUNXI_FUNCTION(0x4, "uart3"),		/* RX */
		SUNXI_FUNCTION(0x5, "uart2"),		/* CTS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 23),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 24),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* CLK */
		SUNXI_FUNCTION(0x3, "twi4"),		/* SCK */
		SUNXI_FUNCTION(0x4, "uart4"),		/* TX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 24),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 25),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* DE */
		SUNXI_FUNCTION(0x3, "twi4"),		/* SDA */
		SUNXI_FUNCTION(0x4, "uart4"),		/* RX */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 25),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 26),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* HSYNC */
		SUNXI_FUNCTION(0x3, "twi5"),		/* SCK */
		SUNXI_FUNCTION(0x4, "uart4"),		/* RTS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 26),
		SUNXI_FUNCTION(0x7, "io_disabled")),
	SUNXI_PIN(SUNXI_PINCTRL_PIN(J, 27),
		SUNXI_FUNCTION(0x0, "gpio_in"),
		SUNXI_FUNCTION(0x1, "gpio_out"),
		SUNXI_FUNCTION(0x2, "lcd1"),		/* VSYNC */
		SUNXI_FUNCTION(0x3, "twi5"),		/* SDA */
		SUNXI_FUNCTION(0x4, "uart4"),		/* CTS */
		SUNXI_FUNCTION_IRQ_BANK(0x6, 8, 27),
		SUNXI_FUNCTION(0x7, "io_disabled")),
};

static const unsigned sun50iw10p1_irq_bank_base[] = {
	SUNXI_PIO_BANK_BASE(PB_BASE, 0),
	SUNXI_PIO_BANK_BASE(PC_BASE, 1),
	SUNXI_PIO_BANK_BASE(PD_BASE, 2),
	SUNXI_PIO_BANK_BASE(PE_BASE, 3),
	SUNXI_PIO_BANK_BASE(PF_BASE, 4),
	SUNXI_PIO_BANK_BASE(PG_BASE, 5),
	SUNXI_PIO_BANK_BASE(PH_BASE, 6),
	SUNXI_PIO_BANK_BASE(PI_BASE, 7),
	SUNXI_PIO_BANK_BASE(PJ_BASE, 8),
};

static const unsigned sun50iw10p1_bank_base[] = {
	SUNXI_PIO_BANK_BASE(PB_BASE, 0),
	SUNXI_PIO_BANK_BASE(PC_BASE, 1),
	SUNXI_PIO_BANK_BASE(PD_BASE, 2),
	SUNXI_PIO_BANK_BASE(PE_BASE, 3),
	SUNXI_PIO_BANK_BASE(PF_BASE, 4),
	SUNXI_PIO_BANK_BASE(PG_BASE, 5),
	SUNXI_PIO_BANK_BASE(PH_BASE, 6),
	SUNXI_PIO_BANK_BASE(PI_BASE, 7),
	SUNXI_PIO_BANK_BASE(PJ_BASE, 8),
};

static const struct sunxi_pinctrl_desc sun50iw10p1_pinctrl_data = {
	.pins = sun50iw10p1_pins,
	.npins = ARRAY_SIZE(sun50iw10p1_pins),
	.pin_base = 0,
	.banks = ARRAY_SIZE(sun50iw10p1_bank_base),
	.bank_base = sun50iw10p1_bank_base,
	.irq_banks = ARRAY_SIZE(sun50iw10p1_irq_bank_base),
	.irq_bank_base = sun50iw10p1_irq_bank_base,
};

static int sun50iw10p1_pinctrl_probe(struct platform_device *pdev)
{
	return sunxi_pinctrl_init(pdev, &sun50iw10p1_pinctrl_data);
}

static struct of_device_id sun50iw10p1_pinctrl_match[] = {
	{ .compatible = "allwinner,sun50iw10p1-pinctrl", },
	{}
};
MODULE_DEVICE_TABLE(of, sun50iw10p1_pinctrl_match);

static struct platform_driver sun50iw10p1_pinctrl_driver = {
	.probe	= sun50iw10p1_pinctrl_probe,
	.driver	= {
		.name		= "sun50iw10p1-pinctrl",
		.owner		= THIS_MODULE,
		.pm		= &sunxi_pinctrl_pm_ops,
		.of_match_table	= sun50iw10p1_pinctrl_match,
	},
};

static int __init sun50iw10p1_pio_init(void)
{
	int ret;
	ret = platform_driver_register(&sun50iw10p1_pinctrl_driver);
	if (ret) {
		pr_err("register sun50iw10p1 pio controller failed\n");
		return -EINVAL;
	}
	return 0;
}
postcore_initcall(sun50iw10p1_pio_init);

MODULE_AUTHOR("Huangshuosheng<huangshuosheng@allwinnertech.com>");
MODULE_DESCRIPTION("Allwinner sun50iw10p1 pio pinctrl driver");
MODULE_LICENSE("GPL");
