EXTRA_CFLAGS += $(USER_EXTRA_CFLAGS)
EXTRA_CFLAGS += -O1
#EXTRA_CFLAGS += -O3
#EXTRA_CFLAGS += -Wall
#EXTRA_CFLAGS += -Wextra
#EXTRA_CFLAGS += -Werror
#EXTRA_CFLAGS += -pedantic
#EXTRA_CFLAGS += -Wshadow -Wpointer-arith -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes

EXTRA_CFLAGS += -Wno-unused-variable
EXTRA_CFLAGS += -Wno-unused-value
EXTRA_CFLAGS += -Wno-unused-label
EXTRA_CFLAGS += -Wno-unused-parameter
EXTRA_CFLAGS += -Wno-unused-function
EXTRA_CFLAGS += -Wno-unused
#EXTRA_CFLAGS += -Wno-uninitialized

GCC_VER_49 := $(shell echo `$(CC) -dumpversion | cut -f1-2 -d.` \>= 4.9 | bc )
ifeq ($(GCC_VER_49),1)
	EXTRA_CFLAGS += -Wno-date-time	# Fix compile error && warning on gcc 4.9 and later
endif

EXTRA_CFLAGS += -I$(src)/include

EXTRA_LDFLAGS += --strip-debug

CONFIG_AUTOCFG_CP = n

########################## WIFI IC ############################
CONFIG_MULTIDRV = n
CONFIG_RTL8188E = n
CONFIG_RTL8812A = n
CONFIG_RTL8821A = n
CONFIG_RTL8192E = n
CONFIG_RTL8723B = n
CONFIG_RTL8814A = n
CONFIG_RTL8723C = n
CONFIG_RTL8188F = n
CONFIG_RTL8188GTV = n
CONFIG_RTL8822B = n
CONFIG_RTL8723D = y
CONFIG_RTL8821C = n
CONFIG_RTL8710B = n
CONFIG_RTL8192F = n
######################### Interface ###########################
CONFIG_USB_HCI = n
CONFIG_PCI_HCI = n
CONFIG_SDIO_HCI = y
CONFIG_GSPI_HCI = n
########################## Features ###########################
CONFIG_MP_INCLUDED = y
CONFIG_POWER_SAVING = y
CONFIG_USB_AUTOSUSPEND = n
CONFIG_HW_PWRP_DETECTION = n
CONFIG_BT_COEXIST = y
CONFIG_INTEL_WIDI = n
CONFIG_WAPI_SUPPORT = n
CONFIG_EFUSE_CONFIG_FILE = y
CONFIG_EXT_CLK = n
CONFIG_TRAFFIC_PROTECT = n
CONFIG_LOAD_PHY_PARA_FROM_FILE = y
CONFIG_TXPWR_BY_RATE_EN = y
CONFIG_TXPWR_LIMIT_EN = n
CONFIG_RTW_CHPLAN = 0xFF
CONFIG_RTW_ADAPTIVITY_EN = disable
CONFIG_RTW_ADAPTIVITY_MODE = normal
CONFIG_SIGNAL_SCALE_MAPPING = n
CONFIG_80211W = n
CONFIG_REDUCE_TX_CPU_LOADING = n
CONFIG_BR_EXT = y
CONFIG_TDLS = n
CONFIG_WIFI_MONITOR = n
CONFIG_MCC_MODE = n
CONFIG_APPEND_VENDOR_IE_ENABLE = n
CONFIG_RTW_NAPI = y
CONFIG_RTW_GRO = y
CONFIG_RTW_NETIF_SG = y
CONFIG_TX_CSUM_OFFLOAD = n
CONFIG_RTW_IPCAM_APPLICATION = n
CONFIG_RTW_REPEATER_SON = n
CONFIG_RTW_WIFI_HAL = y
CONFIG_ICMP_VOQ = n
CONFIG_IP_R_MONITOR = n #arp VOQ and high rate
########################## Debug ###########################
CONFIG_RTW_DEBUG = y
# default log level is _DRV_INFO_ = 4,
# please refer to "How_to_set_driver_debug_log_level.doc" to set the available level.
CONFIG_RTW_LOG_LEVEL = 3
######################## Wake On Lan ##########################
CONFIG_WOWLAN = y
#bit2: deauth, bit1: unicast, bit0: magic pkt.
CONFIG_WAKEUP_TYPE = 0x7
#bit0: disBBRF off, #bit1: Wireless remote controller (WRC)
CONFIG_SUSPEND_TYPE = 0
CONFIG_WOW_STA_MIX = n
CONFIG_GPIO_WAKEUP = y
CONFIG_WAKEUP_GPIO_IDX = default
CONFIG_HIGH_ACTIVE_DEV2HST = n
######### only for USB #########
CONFIG_ONE_PIN_GPIO = n
CONFIG_HIGH_ACTIVE_HST2DEV = n
CONFIG_PNO_SUPPORT = n
CONFIG_PNO_SET_DEBUG = n
CONFIG_AP_WOWLAN = n
######### Notify SDIO Host Keep Power During Syspend ##########
CONFIG_RTW_SDIO_PM_KEEP_POWER = y
###################### MP HW TX MODE FOR VHT #######################
CONFIG_MP_VHT_HW_TX_MODE = n
###################### Platform Related #######################
CONFIG_PLATFORM_I386_PC = n
CONFIG_PLATFORM_ANDROID_X86 = n
CONFIG_PLATFORM_ANDROID_INTEL_X86 = n
CONFIG_PLATFORM_JB_X86 = n
CONFIG_PLATFORM_ARM_S3C2K4 = n
CONFIG_PLATFORM_ARM_PXA2XX = n
CONFIG_PLATFORM_ARM_S3C6K4 = n
CONFIG_PLATFORM_MIPS_RMI = n
CONFIG_PLATFORM_RTD2880B = n
CONFIG_PLATFORM_MIPS_AR9132 = n
CONFIG_PLATFORM_RTK_DMP = n
CONFIG_PLATFORM_MIPS_PLM = n
CONFIG_PLATFORM_MSTAR389 = n
CONFIG_PLATFORM_MT53XX = n
CONFIG_PLATFORM_ARM_MX51_241H = n
CONFIG_PLATFORM_FS_MX61 = n
CONFIG_PLATFORM_ACTIONS_ATJ227X = n
CONFIG_PLATFORM_TEGRA3_CARDHU = n
CONFIG_PLATFORM_TEGRA4_DALMORE = n
CONFIG_PLATFORM_ARM_TCC8900 = n
CONFIG_PLATFORM_ARM_TCC8920 = n
CONFIG_PLATFORM_ARM_TCC8920_JB42 = n
CONFIG_PLATFORM_ARM_TCC8930_JB42 = n
CONFIG_PLATFORM_ARM_RK2818 = n
CONFIG_PLATFORM_ARM_RK3066 = n
CONFIG_PLATFORM_ARM_RK3188 = n
CONFIG_PLATFORM_ARM_URBETTER = n
CONFIG_PLATFORM_ARM_TI_PANDA = n
CONFIG_PLATFORM_MIPS_JZ4760 = n
CONFIG_PLATFORM_DMP_PHILIPS = n
CONFIG_PLATFORM_MSTAR_TITANIA12 = n
CONFIG_PLATFORM_MSTAR = n
CONFIG_PLATFORM_SZEBOOK = n
CONFIG_PLATFORM_ARM_SUNxI = y
CONFIG_PLATFORM_ARM_SUN6I = n
CONFIG_PLATFORM_ARM_SUN7I = n
CONFIG_PLATFORM_ARM_SUN8I_W3P1 = n
CONFIG_PLATFORM_ARM_SUN8I_W5P1 = n
CONFIG_PLATFORM_ACTIONS_ATM702X = n
CONFIG_PLATFORM_ACTIONS_ATV5201 = n
CONFIG_PLATFORM_ACTIONS_ATM705X = n
CONFIG_PLATFORM_ARM_SUN50IW1P1 = n
CONFIG_PLATFORM_ARM_RTD299X = n
CONFIG_PLATFORM_ARM_LGE = n
CONFIG_PLATFORM_ARM_SPREADTRUM_6820 = n
CONFIG_PLATFORM_ARM_SPREADTRUM_8810 = n
CONFIG_PLATFORM_ARM_WMT = n
CONFIG_PLATFORM_TI_DM365 = n
CONFIG_PLATFORM_MOZART = n
CONFIG_PLATFORM_RTK119X = n
CONFIG_PLATFORM_RTK119X_AM = n
CONFIG_PLATFORM_RTK129X = n
CONFIG_PLATFORM_RTK390X = n
CONFIG_PLATFORM_NOVATEK_NT72668 = n
CONFIG_PLATFORM_HISILICON = n
CONFIG_PLATFORM_HISILICON_HI3798 = n
CONFIG_PLATFORM_NV_TK1 = n
CONFIG_PLATFORM_NV_TK1_UBUNTU = n
CONFIG_PLATFORM_RTL8197D = n
CONFIG_PLATFORM_AML_S905 = n
CONFIG_PLATFORM_ZTE_ZX296716 = n
########### CUSTOMER ################################
CONFIG_CUSTOMER_HUAWEI_GENERAL = n

CONFIG_DRVEXT_MODULE = n

export TopDIR ?= $(shell pwd)

########### COMMON  #################################
ifeq ($(CONFIG_GSPI_HCI), y)
HCI_NAME = gspi
endif

ifeq ($(CONFIG_SDIO_HCI), y)
HCI_NAME = sdio
endif

ifeq ($(CONFIG_USB_HCI), y)
HCI_NAME = usb
endif

ifeq ($(CONFIG_PCI_HCI), y)
HCI_NAME = pci
endif

_OS_INTFS_FILES := os_dep/osdep_service.o \
		os_dep/linux/os_intfs.o \
		os_dep/linux/$(HCI_NAME)_intf.o \
		os_dep/linux/$(HCI_NAME)_ops_linux.o \
		os_dep/linux/ioctl_linux.o \
		os_dep/linux/xmit_linux.o \
		os_dep/linux/mlme_linux.o \
		os_dep/linux/recv_linux.o \
		os_dep/linux/ioctl_cfg80211.o \
		os_dep/linux/rtw_cfgvendor.o \
		os_dep/linux/wifi_regd.o \
		os_dep/linux/rtw_android.o \
		os_dep/linux/rtw_proc.o \
		os_dep/linux/rtw_rhashtable.o

ifeq ($(CONFIG_MP_INCLUDED), y)
_OS_INTFS_FILES += os_dep/linux/ioctl_mp.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_OS_INTFS_FILES += os_dep/linux/custom_gpio_linux.o
_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
endif

ifeq ($(CONFIG_GSPI_HCI), y)
_OS_INTFS_FILES += os_dep/linux/custom_gpio_linux.o
_OS_INTFS_FILES += os_dep/linux/$(HCI_NAME)_ops_linux.o
endif

_HAL_INTFS_FILES := hal/hal_intf.o \
		hal/hal_com.o \
		hal/hal_com_phycfg.o \
		hal/hal_phy.o \
		hal/hal_dm.o \
		hal/hal_dm_acs.o \
		hal/hal_btcoex_wifionly.o \
		hal/hal_btcoex.o \
		hal/hal_mp.o \
		hal/hal_mcc.o \
		hal/hal_hci/hal_$(HCI_NAME).o \
		hal/led/hal_led.o \
		hal/led/hal_$(HCI_NAME)_led.o


EXTRA_CFLAGS += -I$(src)/platform
_PLATFORM_FILES := platform/platform_ops.o

EXTRA_CFLAGS += -I$(src)/hal/btc

########### HAL_RTL8188E #################################
ifeq ($(CONFIG_RTL8188E), y)
RTL871X = rtl8188e
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189es
endif

ifeq ($(CONFIG_GSPI_HCI), y)
MODULE_NAME = 8189es
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188eu
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8188ee
endif

EXTRA_CFLAGS += -DCONFIG_RTL8188E

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8188EPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_xmit.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8188e_s_fw.o \
		hal/$(RTL871X)/hal8188e_t_fw.o \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188E_SDIO.o
endif

endif

########### HAL_RTL8192E #################################
ifeq ($(CONFIG_RTL8192E), y)
RTL871X = rtl8192e
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8192es
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8192eu
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8192ee
endif
EXTRA_CFLAGS += -DCONFIG_RTL8192E
_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8192EPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_xmit.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8192e_fw.o \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8192E_SDIO.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8192e1ant.o \
		hal/btc/halbtc8192e2ant.o
endif

endif

########### HAL_RTL8812A_RTL8821A #################################

ifneq ($(CONFIG_RTL8812A)_$(CONFIG_RTL8821A), n_n)
RTL871X = rtl8812a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8812au
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8812ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8812as
endif

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8812PwrSeq.o \
		hal/$(RTL871X)/Hal8821APwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_xmit.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_RTL8812A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8812A_PCIE.o
endif
endif
ifeq ($(CONFIG_RTL8821A), y)
ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8821A_SDIO.o
endif
endif

ifeq ($(CONFIG_RTL8812A), y)
EXTRA_CFLAGS += -DCONFIG_RTL8812A
_HAL_INTFS_FILES += hal/rtl8812a/hal8812a_fw.o
endif

ifeq ($(CONFIG_RTL8821A), y)

ifeq ($(CONFIG_RTL8812A), n)

RTL871X = rtl8821a
ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_BT_COEXIST), y)
MODULE_NAME := 8821au
else
MODULE_NAME := 8811au
endif
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME := 8821ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME := 8821as
endif

endif

EXTRA_CFLAGS += -DCONFIG_RTL8821A

_HAL_INTFS_FILES += hal/rtl8812a/hal8821a_fw.o

endif

ifeq ($(CONFIG_BT_COEXIST), y)
ifeq ($(CONFIG_RTL8812A), y)
_BTC_FILES += hal/btc/halbtc8812a1ant.o \
		hal/btc/halbtc8812a2ant.o
endif
ifeq ($(CONFIG_RTL8821A), y)
_BTC_FILES += hal/btc/halbtc8821a1ant.o \
		hal/btc/halbtc8821a2ant.o
endif
endif

endif

########### HAL_RTL8723B #################################
ifeq ($(CONFIG_RTL8723B), y)

RTL871X = rtl8723b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723bs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8723B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8723BPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8723b_fw.o

_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_PCIE.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723B_SDIO.o
endif

_BTC_FILES += hal/btc/halbtc8723bwifionly.o
ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8723b1ant.o \
		hal/btc/halbtc8723b2ant.o
endif

endif

########### HAL_RTL8814A #################################
ifeq ($(CONFIG_RTL8814A), y)
## ADD NEW VHT MP HW TX MODE ##
#EXTRA_CFLAGS += -DCONFIG_MP_VHT_HW_TX_MODE
#CONFIG_MP_VHT_HW_TX_MODE = y
##########################################
RTL871X = rtl8814a
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8814au
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8814ae
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8814as
endif

EXTRA_CFLAGS += -DCONFIG_RTL8814A

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8814PwrSeq.o \
		hal/$(RTL871X)/$(RTL871X)_xmit.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8814a_fw.o


_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
ifeq ($(CONFIG_GSPI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
endif
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8814A_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8814A_PCIE.o
endif

endif

########### HAL_RTL8723C #################################
ifeq ($(CONFIG_RTL8723C), y)

RTL871X = rtl8703b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723cu
MODULE_SUB_NAME = 8703bu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723ce
MODULE_SUB_NAME = 8703be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723cs
MODULE_SUB_NAME = 8703bs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8703B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8703BPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8703b_fw.o

_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8703B_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8703B_PCIE.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8703b1ant.o
endif

endif

########### HAL_RTL8723D #################################
ifeq ($(CONFIG_RTL8723D), y)

RTL871X = rtl8723d
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8723du
MODULE_SUB_NAME = 8723du
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8723de
MODULE_SUB_NAME = 8723de
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8723ds
MODULE_SUB_NAME = 8723ds
endif

EXTRA_CFLAGS += -DCONFIG_RTL8723D

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8723DPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8723d_fw.o \
		hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723D_USB.o
endif
ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8723D_PCIE.o
endif

ifeq ($(CONFIG_BT_COEXIST), y)
_BTC_FILES += hal/btc/halbtc8723d1ant.o \
		hal/btc/halbtc8723d2ant.o
endif

endif

########### HAL_RTL8188F #################################
ifeq ($(CONFIG_RTL8188F), y)

RTL871X = rtl8188f
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188fu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8188fe
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189fs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8188F

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8188FPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8188f_fw.o

_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188F_USB.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188F_SDIO.o
endif

endif

########### HAL_RTL8188GTV #################################
ifeq ($(CONFIG_RTL8188GTV), y)

RTL871X = rtl8188gtv
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8188gtvu
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8189gtvs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8188GTV

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8188GTVPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8188gtv_fw.o

_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188GTV_USB.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8188GTV_SDIO.o
endif

endif

########### HAL_RTL8822B #################################
ifeq ($(CONFIG_RTL8822B), y)
RTL871X := rtl8822b
ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_BT_COEXIST), n)
MODULE_NAME = 8812bu
else
MODULE_NAME = 88x2bu
endif
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 88x2be
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 88x2bs
endif

endif
########### HAL_RTL8821C #################################
ifeq ($(CONFIG_RTL8821C), y)
RTL871X := rtl8821c
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8821cu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8821ce
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8821cs
endif

endif

########### HAL_RTL8710B #################################
ifeq ($(CONFIG_RTL8710B), y)

RTL871X = rtl8710b
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8710bu
MODULE_SUB_NAME = 8710bu
endif

EXTRA_CFLAGS += -DCONFIG_RTL8710B

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8710BPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8710b_fw.o \
		hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES +=hal/efuse/$(RTL871X)/HalEfuseMask8710B_USB.o
endif

endif

########### HAL_RTL8192F #################################
ifeq ($(CONFIG_RTL8192F), y)

RTL871X = rtl8192f
ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME = 8192fu
MODULE_SUB_NAME = 8192fu
endif
ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME = 8192fe
MODULE_SUB_NAME = 8192fe
endif
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME = 8192fs
MODULE_SUB_NAME = 8192fs
endif

EXTRA_CFLAGS += -DCONFIG_RTL8192F

_HAL_INTFS_FILES += hal/HalPwrSeqCmd.o \
		hal/$(RTL871X)/Hal8192FPwrSeq.o\
		hal/$(RTL871X)/$(RTL871X)_sreset.o

_HAL_INTFS_FILES += hal/$(RTL871X)/$(RTL871X)_hal_init.o \
		hal/$(RTL871X)/$(RTL871X)_phycfg.o \
		hal/$(RTL871X)/$(RTL871X)_rf6052.o \
		hal/$(RTL871X)/$(RTL871X)_dm.o \
		hal/$(RTL871X)/$(RTL871X)_rxdesc.o \
		hal/$(RTL871X)/$(RTL871X)_cmd.o \
		hal/$(RTL871X)/hal8192f_fw.o \
		hal/$(RTL871X)/$(RTL871X)_lps_poff.o


_HAL_INTFS_FILES += \
		hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_halinit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_led.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_xmit.o \
		hal/$(RTL871X)/$(HCI_NAME)/rtl$(MODULE_SUB_NAME)_recv.o

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops_linux.o
else
_HAL_INTFS_FILES += hal/$(RTL871X)/$(HCI_NAME)/$(HCI_NAME)_ops.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_HAL_INTFS_FILES += hal/efuse/$(RTL871X)/HalEfuseMask8192F_SDIO.o
endif

ifeq ($(CONFIG_USB_HCI), y)
_HAL_INTFS_FILES += hal/efuse/$(RTL871X)/HalEfuseMask8192F_USB.o
endif

ifeq ($(CONFIG_PCI_HCI), y)
_HAL_INTFS_FILES += hal/efuse/$(RTL871X)/HalEfuseMask8192F_PCIE.o
endif

endif
########### AUTO_CFG  #################################

ifeq ($(CONFIG_AUTOCFG_CP), y)

ifeq ($(CONFIG_MULTIDRV), y)
$(shell cp $(TopDIR)/autoconf_multidrv_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else
ifeq ($(CONFIG_RTL8188E)$(CONFIG_SDIO_HCI),yy)
$(shell cp $(TopDIR)/autoconf_rtl8189e_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else ifeq ($(CONFIG_RTL8188F)$(CONFIG_SDIO_HCI),yy)
$(shell cp $(TopDIR)/autoconf_rtl8189f_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else ifeq ($(CONFIG_RTL8723C),y)
$(shell cp $(TopDIR)/autoconf_rtl8723c_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
else
$(shell cp $(TopDIR)/autoconf_$(RTL871X)_$(HCI_NAME)_linux.h $(TopDIR)/include/autoconf.h)
endif
endif

endif

########### END OF PATH  #################################

ifeq ($(CONFIG_USB_HCI), y)
ifeq ($(CONFIG_USB_AUTOSUSPEND), y)
EXTRA_CFLAGS += -DCONFIG_USB_AUTOSUSPEND
endif
endif

ifeq ($(CONFIG_MP_INCLUDED), y)
#MODULE_NAME := $(MODULE_NAME)_mp
EXTRA_CFLAGS += -DCONFIG_MP_INCLUDED
endif

ifeq ($(CONFIG_POWER_SAVING), y)
EXTRA_CFLAGS += -DCONFIG_POWER_SAVING
endif

ifeq ($(CONFIG_HW_PWRP_DETECTION), y)
EXTRA_CFLAGS += -DCONFIG_HW_PWRP_DETECTION
endif

ifeq ($(CONFIG_BT_COEXIST), y)
EXTRA_CFLAGS += -DCONFIG_BT_COEXIST
endif

ifeq ($(CONFIG_INTEL_WIDI), y)
EXTRA_CFLAGS += -DCONFIG_INTEL_WIDI
endif

ifeq ($(CONFIG_WAPI_SUPPORT), y)
EXTRA_CFLAGS += -DCONFIG_WAPI_SUPPORT
endif

ifeq ($(CONFIG_EFUSE_CONFIG_FILE), y)
EXTRA_CFLAGS += -DCONFIG_EFUSE_CONFIG_FILE

#EFUSE_MAP_PATH
USER_EFUSE_MAP_PATH ?=
ifneq ($(USER_EFUSE_MAP_PATH),)
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"$(USER_EFUSE_MAP_PATH)\"
else ifeq ($(MODULE_NAME), 8189es)
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8189e.map\"
else ifeq ($(MODULE_NAME), 8723bs)
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_8723bs.map\"
else
EXTRA_CFLAGS += -DEFUSE_MAP_PATH=\"/system/etc/wifi/wifi_efuse_$(MODULE_NAME).map\"
endif

#WIFIMAC_PATH
USER_WIFIMAC_PATH ?=
ifneq ($(USER_WIFIMAC_PATH),)
EXTRA_CFLAGS += -DWIFIMAC_PATH=\"$(USER_WIFIMAC_PATH)\"
else
EXTRA_CFLAGS += -DWIFIMAC_PATH=\"/data/wifimac.txt\"
endif

endif

ifeq ($(CONFIG_EXT_CLK), y)
EXTRA_CFLAGS += -DCONFIG_EXT_CLK
endif

ifeq ($(CONFIG_TRAFFIC_PROTECT), y)
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
endif

ifeq ($(CONFIG_LOAD_PHY_PARA_FROM_FILE), y)
EXTRA_CFLAGS += -DCONFIG_LOAD_PHY_PARA_FROM_FILE
#EXTRA_CFLAGS += -DREALTEK_CONFIG_PATH_WITH_IC_NAME_FOLDER
EXTRA_CFLAGS += -DREALTEK_CONFIG_PATH=\"/lib/firmware/\"
endif

ifeq ($(CONFIG_TXPWR_BY_RATE_EN), n)
EXTRA_CFLAGS += -DCONFIG_TXPWR_BY_RATE_EN=0
else ifeq ($(CONFIG_TXPWR_BY_RATE_EN), y)
EXTRA_CFLAGS += -DCONFIG_TXPWR_BY_RATE_EN=1
else ifeq ($(CONFIG_TXPWR_BY_RATE_EN), auto)
EXTRA_CFLAGS += -DCONFIG_TXPWR_BY_RATE_EN=2
endif

ifeq ($(CONFIG_TXPWR_LIMIT_EN), n)
EXTRA_CFLAGS += -DCONFIG_TXPWR_LIMIT_EN=0
else ifeq ($(CONFIG_TXPWR_LIMIT_EN), y)
EXTRA_CFLAGS += -DCONFIG_TXPWR_LIMIT_EN=1
else ifeq ($(CONFIG_TXPWR_LIMIT_EN), auto)
EXTRA_CFLAGS += -DCONFIG_TXPWR_LIMIT_EN=2
endif

ifneq ($(CONFIG_RTW_CHPLAN), 0xFF)
EXTRA_CFLAGS += -DCONFIG_RTW_CHPLAN=$(CONFIG_RTW_CHPLAN)
endif

ifeq ($(CONFIG_CALIBRATE_TX_POWER_BY_REGULATORY), y)
EXTRA_CFLAGS += -DCONFIG_CALIBRATE_TX_POWER_BY_REGULATORY
endif

ifeq ($(CONFIG_CALIBRATE_TX_POWER_TO_MAX), y)
EXTRA_CFLAGS += -DCONFIG_CALIBRATE_TX_POWER_TO_MAX
endif

ifeq ($(CONFIG_RTW_ADAPTIVITY_EN), disable)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_EN=0
else ifeq ($(CONFIG_RTW_ADAPTIVITY_EN), enable)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_EN=1
endif

ifeq ($(CONFIG_RTW_ADAPTIVITY_MODE), normal)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_MODE=0
else ifeq ($(CONFIG_RTW_ADAPTIVITY_MODE), carrier_sense)
EXTRA_CFLAGS += -DCONFIG_RTW_ADAPTIVITY_MODE=1
endif

ifeq ($(CONFIG_SIGNAL_SCALE_MAPPING), y)
EXTRA_CFLAGS += -DCONFIG_SIGNAL_SCALE_MAPPING
endif

ifeq ($(CONFIG_80211W), y)
EXTRA_CFLAGS += -DCONFIG_IEEE80211W
endif

ifeq ($(CONFIG_WOWLAN), y)
EXTRA_CFLAGS += -DCONFIG_WOWLAN -DRTW_WAKEUP_EVENT=$(CONFIG_WAKEUP_TYPE)
EXTRA_CFLAGS += -DRTW_SUSPEND_TYPE=$(CONFIG_SUSPEND_TYPE)
ifeq ($(CONFIG_WOW_STA_MIX), y)
EXTRA_CFLAGS += -DRTW_WOW_STA_MIX
endif
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_AP_WOWLAN), y)
EXTRA_CFLAGS += -DCONFIG_AP_WOWLAN
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_PNO_SUPPORT), y)
EXTRA_CFLAGS += -DCONFIG_PNO_SUPPORT
ifeq ($(CONFIG_PNO_SET_DEBUG), y)
EXTRA_CFLAGS += -DCONFIG_PNO_SET_DEBUG
endif
endif

ifeq ($(CONFIG_GPIO_WAKEUP), y)
EXTRA_CFLAGS += -DCONFIG_GPIO_WAKEUP
ifeq ($(CONFIG_ONE_PIN_GPIO), y)
EXTRA_CFLAGS += -DCONFIG_RTW_ONE_PIN_GPIO
endif
ifeq ($(CONFIG_HIGH_ACTIVE_DEV2HST), y)
EXTRA_CFLAGS += -DHIGH_ACTIVE_DEV2HST=1
else
EXTRA_CFLAGS += -DHIGH_ACTIVE_DEV2HST=0
endif
endif

ifeq ($(CONFIG_HIGH_ACTIVE_HST2DEV), y)
EXTRA_CFLAGS += -DHIGH_ACTIVE_HST2DEV=1
else
EXTRA_CFLAGS += -DHIGH_ACTIVE_HST2DEV=0
endif

ifneq ($(CONFIG_WAKEUP_GPIO_IDX), default)
EXTRA_CFLAGS += -DWAKEUP_GPIO_IDX=$(CONFIG_WAKEUP_GPIO_IDX)
endif

ifeq ($(CONFIG_RTW_SDIO_PM_KEEP_POWER), y)
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_SDIO_PM_KEEP_POWER
endif
endif

ifeq ($(CONFIG_REDUCE_TX_CPU_LOADING), y)
EXTRA_CFLAGS += -DCONFIG_REDUCE_TX_CPU_LOADING
endif

ifeq ($(CONFIG_BR_EXT), y)
BR_NAME = br0
EXTRA_CFLAGS += -DCONFIG_BR_EXT
EXTRA_CFLAGS += '-DCONFIG_BR_EXT_BRNAME="'$(BR_NAME)'"'
endif

ifeq ($(CONFIG_TDLS), y)
EXTRA_CFLAGS += -DCONFIG_TDLS
endif

ifeq ($(CONFIG_WIFI_MONITOR), y)
EXTRA_CFLAGS += -DCONFIG_WIFI_MONITOR
endif

ifeq ($(CONFIG_MCC_MODE), y)
EXTRA_CFLAGS += -DCONFIG_MCC_MODE
endif

ifeq ($(CONFIG_RTW_NAPI), y)
EXTRA_CFLAGS += -DCONFIG_RTW_NAPI
endif

ifeq ($(CONFIG_RTW_GRO), y)
EXTRA_CFLAGS += -DCONFIG_RTW_GRO
endif

ifeq ($(CONFIG_RTW_REPEATER_SON), y)
EXTRA_CFLAGS += -DCONFIG_RTW_REPEATER_SON
endif

ifeq ($(CONFIG_RTW_IPCAM_APPLICATION), y)
EXTRA_CFLAGS += -DCONFIG_RTW_IPCAM_APPLICATION
ifeq ($(CONFIG_WIFI_MONITOR), n)
EXTRA_CFLAGS += -DCONFIG_WIFI_MONITOR
endif
endif

ifeq ($(CONFIG_RTW_NETIF_SG), y)
EXTRA_CFLAGS += -DCONFIG_RTW_NETIF_SG
endif

ifeq ($(CONFIG_TX_CSUM_OFFLOAD), y)
EXTRA_CFLAGS += -DCONFIG_TX_CSUM_OFFLOAD
endif

ifeq ($(CONFIG_ICMP_VOQ), y)
EXTRA_CFLAGS += -DCONFIG_ICMP_VOQ
endif

ifeq ($(CONFIG_IP_R_MONITOR), y)
EXTRA_CFLAGS += -DCONFIG_IP_R_MONITOR
endif

ifeq ($(CONFIG_RTW_WIFI_HAL), y)
#EXTRA_CFLAGS += -DCONFIG_RTW_WIFI_HAL_DEBUG
EXTRA_CFLAGS += -DCONFIG_RTW_WIFI_HAL
EXTRA_CFLAGS += -DCONFIG_RTW_CFGVEDNOR_LLSTATS
EXTRA_CFLAGS += -DCONFIG_RTW_CFGVENDOR_RANDOM_MAC_OUI
EXTRA_CFLAGS += -DCONFIG_RTW_CFGVEDNOR_RSSIMONITOR
EXTRA_CFLAGS += -DCONFIG_RTW_CFGVENDOR_WIFI_LOGGER
endif

ifeq ($(CONFIG_MP_VHT_HW_TX_MODE), y)
EXTRA_CFLAGS += -DCONFIG_MP_VHT_HW_TX_MODE
ifeq ($(CONFIG_PLATFORM_I386_PC), y)
## For I386 X86 ToolChain use Hardware FLOATING
EXTRA_CFLAGS += -mhard-float
else
## For ARM ToolChain use Hardware FLOATING
EXTRA_CFLAGS += -mfloat-abi=hard
endif
endif

ifeq ($(CONFIG_APPEND_VENDOR_IE_ENABLE), y)
EXTRA_CFLAGS += -DCONFIG_APPEND_VENDOR_IE_ENABLE
endif

ifeq ($(CONFIG_RTW_DEBUG), y)
EXTRA_CFLAGS += -DCONFIG_RTW_DEBUG
EXTRA_CFLAGS += -DRTW_LOG_LEVEL=$(CONFIG_RTW_LOG_LEVEL)
endif

EXTRA_CFLAGS += -DDM_ODM_SUPPORT_TYPE=0x04

ifeq ($(CONFIG_PLATFORM_I386_PC), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH ?= $(SUBARCH)
MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
STAGINGMODDIR := /lib/modules/$(KVER)/kernel/drivers/staging
endif

ifeq ($(CONFIG_PLATFORM_NV_TK1), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_NV_TK1
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_P2P_IPS -DCONFIG_PLATFORM_ANDROID
# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK
EXTRA_CFLAGS += -DRTW_VENDOR_EXT_SUPPORT
EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
ARCH ?= arm

MODULE_NAME = wlan
endif

ifeq ($(CONFIG_PLATFORM_NV_TK1_UBUNTU), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_NV_TK1
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

ARCH ?= arm

MODDESTDIR := /lib/modules/$(KVER)/kernel/drivers/net/wireless/
INSTALL_PREFIX :=
endif

ifeq ($(CONFIG_PLATFORM_ACTIONS_ATM702X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_ACTIONS_ATM702X
#ARCH := arm
ARCH := $(R_ARCH)
#CROSS_COMPILE := arm-none-linux-gnueabi-
#KSRC := ../../../../build/out/kernel
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ACTIONS_ATM705X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
#EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
# default setting for Android 4.1, 4.2, 4.3, 4.4
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ACTIONS_ATM705X
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_arm_act_sdio.o
endif

ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN50IW1P1), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN50IW1P1
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_RESUME_IN_WORKQUEUE
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS

# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUN50IW1P1_sdio.o
endif

ARCH := arm64
# ===Cross compile setting for Android 5.1(64) SDK ===
endif

ifeq ($(CONFIG_PLATFORM_TI_AM3517), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_SHUTTLE
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_MSTAR_TITANIA12), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MSTAR -DCONFIG_PLATFORM_MSTAR_TITANIA12
ARCH:=mips
endif

ifeq ($(CONFIG_PLATFORM_MSTAR), y)
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_MSTAR
EXTRA_CFLAGS += -DCONFIG_PLATFORM_MSTAR_HIGH
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX -DCONFIG_FIX_NR_BULKIN_BUFFER
endif
ARCH:=arm
endif

ifeq ($(CONFIG_PLATFORM_ANDROID_X86), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH := $(SUBARCH)
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ANDROID_INTEL_X86), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ANDROID_INTEL_X86
EXTRA_CFLAGS += -DCONFIG_PLATFORM_INTEL_BYT
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_SKIP_SIGNAL_SCALE_MAPPING
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_RESUME_IN_WORKQUEUE
endif
endif

ifeq ($(CONFIG_PLATFORM_JB_X86), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
SUBARCH := $(shell uname -m | sed -e s/i.86/i386/)
ARCH := $(SUBARCH)
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_PXA2XX), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_ARM_S3C2K4), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_ARM_S3C6K4), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_RTD2880B), y)
EXTRA_CFLAGS += -DCONFIG_BIG_ENDIAN -DCONFIG_PLATFORM_RTD2880B
ARCH:=
endif

ifeq ($(CONFIG_PLATFORM_MIPS_RMI), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH:=mips
endif

ifeq ($(CONFIG_PLATFORM_MIPS_PLM), y)
EXTRA_CFLAGS += -DCONFIG_BIG_ENDIAN
ARCH:=mips
endif

ifeq ($(CONFIG_PLATFORM_MSTAR389), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MSTAR389
ARCH:=mips
endif

ifeq ($(CONFIG_PLATFORM_MIPS_AR9132), y)
EXTRA_CFLAGS += -DCONFIG_BIG_ENDIAN
ARCH := mips
endif

ifeq ($(CONFIG_PLATFORM_DMP_PHILIPS), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DRTK_DMP_PLATFORM
ARCH := mips
#CROSS_COMPILE:=/usr/local/msdk-4.3.6-mips-EL-2.6.12.6-0.9.30.3/bin/mipsel-linux-
endif

ifeq ($(CONFIG_PLATFORM_RTK_DMP), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DRTK_DMP_PLATFORM  -DCONFIG_WIRELESS_EXT
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
_PLATFORM_FILES += platform/platform_RTK_DMP_usb.o
endif
ARCH:=mips
endif

ifeq ($(CONFIG_PLATFORM_MT53XX), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MT53XX
ARCH:= arm
endif

ifeq ($(CONFIG_PLATFORM_ARM_MX51_241H), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_WISTRON_PLATFORM
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_FS_MX61), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_ACTIONS_ATJ227X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ACTIONS_ATJ227X
ARCH := mips
endif

ifeq ($(CONFIG_PLATFORM_TI_DM365), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_TI_DM365
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_RX
EXTRA_CFLAGS += -DCONFIG_SINGLE_XMIT_BUF -DCONFIG_SINGLE_RECV_BUF
ARCH := arm
KERNELOUTPUT := ${PRODUCTDIR}/tmp
endif

ifeq ($(CONFIG_PLATFORM_MOZART), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_MOZART
ARCH := arm
endif

ifeq ($(CONFIG_PLATFORM_TEGRA3_CARDHU), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_TEGRA4_DALMORE), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8900), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8920), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8920_JB42), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RK2818), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_ROCKCHIPS
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RK3188), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_PLATFORM_ROCKCHIPS
# default setting for Android 4.1, 4.2, 4.3, 4.4
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
# default setting for Power control
EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
EXTRA_CFLAGS += -DRTW_SUPPORT_PLATFORM_SHUTDOWN
# default setting for Special function
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_RK3066), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_RK3066
EXTRA_CFLAGS += -DRTW_ENABLE_WIFI_CONTROL_FUNC
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DRTW_SUPPORT_PLATFORM_SHUTDOWN
endif
EXTRA_CFLAGS += -fno-pic
ARCH := arm
MODULE_NAME :=wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_URBETTER), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN #-DCONFIG_MINIMAL_MEMORY_USAGE
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_ARM_TI_PANDA), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN #-DCONFIG_MINIMAL_MEMORY_USAGE
ARCH := arm
#CROSS_COMPILE := /media/DATA-1/aosp/ics-aosp_20111227/prebuilt/linux-x86/toolchain/arm-eabi-4.4.3/bin/arm-eabi-
#KSRC := /media/DATA-1/aosp/android-omap-panda-3.0_20120104
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_MIPS_JZ4760), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_MINIMAL_MEMORY_USAGE
ARCH ?= mips
endif

ifeq ($(CONFIG_PLATFORM_SZEBOOK), y)
EXTRA_CFLAGS += -DCONFIG_BIG_ENDIAN
ARCH:=arm
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUNxI), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUNxI
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif

ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNxI_sdio.o
endif

endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN6I), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN6I
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2, 4.3, 4.4
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_QOS_OPTIMIZATION

EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
# default setting for A31-EVB mmc0
EXTRA_CFLAGS += -DCONFIG_A31_EVB
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif
endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN7I), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN7I
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2, 4.3, 4.4
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_QOS_OPTIMIZATION

EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN8I_W3P1), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN8I
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN8I_W3P1
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

endif

ifeq ($(CONFIG_PLATFORM_ARM_SUN8I_W5P1), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN8I
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN8I_W5P1
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK

EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

endif

ifeq ($(CONFIG_PLATFORM_ACTIONS_ATV5201), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ACTIONS_ATV5201
EXTRA_CFLAGS += -DCONFIG_SDIO_DISABLE_RXFIFO_POLLING_LOOP
ARCH := mips
endif

ifeq ($(CONFIG_PLATFORM_ARM_RTD299X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ifeq ($(CONFIG_ANDROID), y)
# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK
endif
#ARCH, CROSS_COMPILE, KSRC,and  MODDESTDIR are provided by external makefile
INSTALL_PREFIX :=
endif

ifeq ($(CONFIG_PLATFORM_ARM_RTD299X_LG), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DRTW_P2P_GROUP_INTERFACE=1
EXTRA_CFLAGS += -DCONFIG_IFACE_NUMBER=3
#EXTRA_CFLAGS += -DCONFIG_FIX_HWPORT
EXTRA_CFLAGS += -DLGE_PRIVATE
EXTRA_CFLAGS += -DPURE_SUPPLICANT
EXTRA_CFLAGS += -DCONFIG_CUSTOMIZED_COUNTRY_CHPLAN_MAP -DCONFIG_RTW_IOCTL_SET_COUNTRY
EXTRA_CFLAGS += -DDBG_RX_DFRAME_RAW_DATA
EXTRA_CFLAGS += -DRTW_REDUCE_SCAN_SWITCH_CH_TIME
ARCH ?= arm

ifneq ($(PLATFORM), WEBOS)
$(info PLATFORM is empty)
endif

INSTALL_PREFIX ?=
endif

ifeq ($(CONFIG_PLATFORM_HISILICON), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_HISILICON
ifeq ($(SUPPORT_CONCURRENT),y)
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
endif
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
ifeq ($(CROSS_COMPILE),)
endif
MODULE_NAME := rtl8192eu
ifeq ($(KSRC),)
endif
endif

ifeq ($(CONFIG_PLATFORM_HISILICON_HI3798), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_HISILICON
EXTRA_CFLAGS += -DCONFIG_PLATFORM_HISILICON_HI3798
#EXTRA_CFLAGS += -DCONFIG_PLATFORM_HISILICON_HI3798_MV200_HDMI_DONGLE
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211
EXTRA_CFLAGS += -DRTW_USE_CFG80211_STA_EVENT
# default setting for Android 5.x and later
#EXTRA_CFLAGS += -DCONFIG_RADIO_WORK

# If system could power on and recognize Wi-Fi SDIO automatically,
# platfrom operations are not necessary.
#ifeq ($(CONFIG_SDIO_HCI), y)
#EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
#_PLATFORM_FILES += platform/platform_hisilicon_hi3798_sdio.o
#EXTRA_CFLAGS += -DCONFIG_HISI_SDIO_ID=1
#endif

ARCH ?= arm
ifndef KSRC
endif

ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SDIO_HCI), y)
CONFIG_RTL8822BS ?= m
USER_MODULE_NAME := rtl8822bs
endif
endif

endif

# Platform setting
ifeq ($(CONFIG_PLATFORM_ARM_SPREADTRUM_6820), y)
ifeq ($(CONFIG_ANDROID_2X), y)
EXTRA_CFLAGS += -DANDROID_2X
endif
EXTRA_CFLAGS += -DCONFIG_PLATFORM_SPRD
EXTRA_CFLAGS += -DPLATFORM_SPREADTRUM_6820
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ifeq ($(RTL871X), rtl8188e)
EXTRA_CFLAGS += -DSOFTAP_PS_DURATION=50
endif
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_sprd_sdio.o
endif
endif

ifeq ($(CONFIG_PLATFORM_ARM_SPREADTRUM_8810), y)
ifeq ($(CONFIG_ANDROID_2X), y)
EXTRA_CFLAGS += -DANDROID_2X
endif
EXTRA_CFLAGS += -DCONFIG_PLATFORM_SPRD
EXTRA_CFLAGS += -DPLATFORM_SPREADTRUM_8810
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
ifeq ($(RTL871X), rtl8188e)
EXTRA_CFLAGS += -DSOFTAP_PS_DURATION=50
endif
ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_sprd_sdio.o
endif
endif

ifeq ($(CONFIG_PLATFORM_ARM_WMT), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_WMT_sdio.o
endif
ARCH := arm
MODULE_NAME :=8189es_kk
endif

ifeq ($(CONFIG_PLATFORM_RTK119X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
#EXTRA_CFLAGS += -DCONFIG_PLATFORM_ARM_SUN7I
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
#EXTRA_CFLAGS += -DCONFIG_QOS_OPTIMIZATION
EXTRA_CFLAGS += -DCONFIG_QOS_OPTIMIZATION

#EXTRA_CFLAGS += -DCONFIG_#PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
#_PLATFORM_FILES += platform/platform_ARM_SUNxI_usb.o
endif
ifeq ($(CONFIG_SDIO_HCI), y)
_PLATFORM_FILES += platform/platform_ARM_SUNnI_sdio.o
endif

ARCH := arm

# ==== Cross compile setting for Android 4.4 SDK =====
#CROSS_COMPILE := arm-linux-gnueabihf-
MODULE_NAME := 8192eu

endif

ifeq ($(CONFIG_PLATFORM_RTK119X_AM), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_RTK119X_AM
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE -DCONFIG_FULL_CH_IN_P2P_HANDSHAKE
EXTRA_CFLAGS += -DCONFIG_IFACE_NUMBER=3
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT

ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH := arm

#CROSS_COMPILE := arm-linux-gnueabihf-
#KSRC :=
endif

ifeq ($(CONFIG_PLATFORM_RTK129X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DRTK_129X_PLATFORM
EXTRA_CFLAGS += -DCONFIG_TRAFFIC_PROTECT
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
#EXTRA_CFLAGS += -DCONFIG_P2P_IPS -DCONFIG_QOS_OPTIMIZATION
EXTRA_CFLAGS += -DCONFIG_QOS_OPTIMIZATION
# Enable this for Android 5.0
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK
ifeq ($(CONFIG_RTL8821C)$(CONFIG_SDIO_HCI),yy)
EXTRA_CFLAGS += -DCONFIG_WAKEUP_GPIO_INPUT_MODE
EXTRA_CFLAGS += -DCONFIG_BT_WAKE_HST_OPEN_DRAIN
endif
EXTRA_CFLAGS += -Wno-error=date-time
# default setting for Android 7.0
ifeq ($(RTK_ANDROID_VERSION), nougat)
EXTRA_CFLAGS += -DRTW_P2P_GROUP_INTERFACE=1
endif
#EXTRA_CFLAGS += -DCONFIG_#PLATFORM_OPS
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH := arm64
endif

ifeq ($(CONFIG_PLATFORM_RTK390X), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_PLATFORM_RTK390X
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_RTW_NETIF_SG
ifeq ($(CONFIG_USB_HCI), y)
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
endif

ARCH:=rlx


endif

ifeq ($(CONFIG_PLATFORM_NOVATEK_NT72668), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_NOVATEK_NT72668
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_RX
EXTRA_CFLAGS += -DCONFIG_USE_USB_BUFFER_ALLOC_TX
ARCH ?= arm
#KSRC := $(KERNELDIR)
endif

ifeq ($(CONFIG_PLATFORM_ARM_TCC8930_JB42), y)
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android 4.1, 4.2
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT
ARCH := arm
MODULE_NAME := wlan
endif

ifeq ($(CONFIG_PLATFORM_RTL8197D), y)
EXTRA_CFLAGS += -DCONFIG_BIG_ENDIAN -DCONFIG_PLATFORM_RTL8197D
export DIR_LINUX=$(shell pwd)/../SDK/rlxlinux-sdk321-v50/linux-2.6.30
ARCH ?= rlx
endif

ifeq ($(CONFIG_PLATFORM_AML_S905), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_AML_S905
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN -fno-pic
# default setting for Android
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211
EXTRA_CFLAGS += -DRTW_USE_CFG80211_STA_EVENT
# default setting for Android 5.x and later
EXTRA_CFLAGS += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_SDIO_HCI), y)
EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
_PLATFORM_FILES += platform/platform_aml_s905_sdio.o
endif

ARCH ?= arm64
ifndef KSRC
# To locate output files in a separate directory.
endif

ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SDIO_HCI), y)
CONFIG_RTL8822BS ?= m
USER_MODULE_NAME := 8822bs
endif
endif

endif

ifeq ($(CONFIG_PLATFORM_ZTE_ZX296716), y)
EXTRA_CFLAGS += -Wno-error=date-time
EXTRA_CFLAGS += -DCONFIG_PLATFORM_ZTE_ZX296716
EXTRA_CFLAGS += -DCONFIG_LITTLE_ENDIAN
# default setting for Android
EXTRA_CFLAGS += -DCONFIG_CONCURRENT_MODE
EXTRA_CFLAGS += -DCONFIG_IOCTL_CFG80211
EXTRA_CFLAGS += -DRTW_USE_CFG80211_STA_EVENT
# default setting for Android 5.x and later
#EXTRA_CFLAGS += -DCONFIG_RADIO_WORK

ifeq ($(CONFIG_SDIO_HCI), y)
# mark this temporarily
#EXTRA_CFLAGS += -DCONFIG_PLATFORM_OPS
#_PLATFORM_FILES += platform/platform_zte_zx296716_sdio.o
endif

ifeq ($(CONFIG_RTL8822B), y)
ifeq ($(CONFIG_SDIO_HCI), y)
CONFIG_RTL8822BS ?= m
USER_MODULE_NAME := 8822bs
endif
endif

endif

########### CUSTOMER ################################
ifeq ($(CONFIG_CUSTOMER_HUAWEI_GENERAL), y)
CONFIG_CUSTOMER_HUAWEI = y
endif

ifeq ($(CONFIG_CUSTOMER_HUAWEI), y)
EXTRA_CFLAGS += -DCONFIG_HUAWEI_PROC
endif

ifeq ($(CONFIG_MULTIDRV), y)
ifeq ($(CONFIG_SDIO_HCI), y)
MODULE_NAME := rtw_sdio
endif

ifeq ($(CONFIG_USB_HCI), y)
MODULE_NAME := rtw_usb
endif

ifeq ($(CONFIG_PCI_HCI), y)
MODULE_NAME := rtw_pci
endif

endif

USER_MODULE_NAME ?=
ifneq ($(USER_MODULE_NAME),)
MODULE_NAME := $(USER_MODULE_NAME)
endif

ifneq ($(KERNELRELEASE),)

########### this part for *.mk ############################
include $(src)/hal/phydm/phydm.mk

########### HAL_RTL8822B #################################
ifeq ($(CONFIG_RTL8822B), y)
include $(src)/rtl8822b.mk
endif

########### HAL_RTL8821C #################################
ifeq ($(CONFIG_RTL8821C), y)
include $(src)/rtl8821c.mk
endif

rtk_core := core/rtw_cmd.o \
		core/rtw_security.o \
		core/rtw_debug.o \
		core/rtw_io.o \
		core/rtw_ioctl_query.o \
		core/rtw_ioctl_set.o \
		core/rtw_ieee80211.o \
		core/rtw_mlme.o \
		core/rtw_mlme_ext.o \
		core/rtw_mi.o \
		core/rtw_wlan_util.o \
		core/rtw_vht.o \
		core/rtw_pwrctrl.o \
		core/rtw_rf.o \
		core/rtw_chplan.o \
		core/rtw_recv.o \
		core/rtw_sta_mgt.o \
		core/rtw_ap.o \
		core/mesh/rtw_mesh.o \
		core/mesh/rtw_mesh_pathtbl.o \
		core/mesh/rtw_mesh_hwmp.o \
		core/rtw_xmit.o \
		core/rtw_p2p.o \
		core/rtw_rson.o \
		core/rtw_tdls.o \
		core/rtw_br_ext.o \
		core/rtw_iol.o \
		core/rtw_sreset.o \
		core/rtw_btcoex_wifionly.o \
		core/rtw_btcoex.o \
		core/rtw_beamforming.o \
		core/rtw_odm.o \
		core/rtw_rm.o \
		core/rtw_rm_fsm.o \
		core/efuse/rtw_efuse.o

ifeq ($(CONFIG_SDIO_HCI), y)
rtk_core += core/rtw_sdio.o
endif

$(MODULE_NAME)-y += $(rtk_core)

$(MODULE_NAME)-$(CONFIG_INTEL_WIDI) += core/rtw_intel_widi.o

$(MODULE_NAME)-$(CONFIG_WAPI_SUPPORT) += core/rtw_wapi.o \
		core/rtw_wapi_sms4.o

$(MODULE_NAME)-y += $(_OS_INTFS_FILES)
$(MODULE_NAME)-y += $(_HAL_INTFS_FILES)
$(MODULE_NAME)-y += $(_PHYDM_FILES)
$(MODULE_NAME)-y += $(_BTC_FILES)
$(MODULE_NAME)-y += $(_PLATFORM_FILES)

$(MODULE_NAME)-$(CONFIG_MP_INCLUDED) += core/rtw_mp.o

ifeq ($(CONFIG_RTL8723B), y)
$(MODULE_NAME)-$(CONFIG_MP_INCLUDED)+= core/rtw_bt_mp.o
endif

obj-$(CONFIG_RTL8723DS) := $(MODULE_NAME).o

else

export CONFIG_RTL8723DS = m
all: modules
modules:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KSRC) M=$(shell pwd)  modules

strip:
	$(CROSS_COMPILE)strip $(MODULE_NAME).ko --strip-unneeded

install:
	install -p -m 644 $(MODULE_NAME).ko  $(MODDESTDIR)
	/sbin/depmod -a ${KVER}

uninstall:
	rm -f $(MODDESTDIR)/$(MODULE_NAME).ko
	/sbin/depmod -a ${KVER}

backup_rtlwifi:
	@echo "Making backup rtlwifi drivers"
	ifneq (,$(wildcard $(STAGINGMODDIR)/rtl*))
	@tar cPf $(wildcard $(STAGINGMODDIR))/backup_rtlwifi_driver.tar $(wildcard $(STAGINGMODDIR)/rtl*)
	@rm -rf $(wildcard $(STAGINGMODDIR)/rtl*)
endif

ifneq (,$(wildcard $(MODDESTDIR)realtek))
	@tar cPf $(MODDESTDIR)backup_rtlwifi_driver.tar $(MODDESTDIR)realtek
	@rm -fr $(MODDESTDIR)realtek
endif
ifneq (,$(wildcard $(MODDESTDIR)rtl*))
	@tar cPf $(MODDESTDIR)../backup_rtlwifi_driver.tar $(wildcard $(MODDESTDIR)rtl*)
	@rm -fr $(wildcard $(MODDESTDIR)rtl*)
endif
	@/sbin/depmod -a ${KVER}
	@echo "Please reboot your system"

restore_rtlwifi:
	@echo "Restoring backups"
ifneq (,$(wildcard $(STAGINGMODDIR)/backup_rtlwifi_driver.tar))
	@tar xPf $(STAGINGMODDIR)/backup_rtlwifi_driver.tar
	@rm $(STAGINGMODDIR)/backup_rtlwifi_driver.tar
endif
ifneq (,$(wildcard $(MODDESTDIR)backup_rtlwifi_driver.tar))
	@tar xPf $(MODDESTDIR)backup_rtlwifi_driver.tar
	@rm $(MODDESTDIR)backup_rtlwifi_driver.tar
endif
ifneq (,$(wildcard $(MODDESTDIR)../backup_rtlwifi_driver.tar))
	@tar xPf $(MODDESTDIR)../backup_rtlwifi_driver.tar
	@rm $(MODDESTDIR)../backup_rtlwifi_driver.tar
endif
	@/sbin/depmod -a ${KVER}
	@echo "Please reboot your system"

config_r:
	@echo "make config"
	/bin/bash script/Configure script/config.in

.PHONY: modules clean

clean:
#$(MAKE) -C $(KSRC) M=$(shell pwd) clean
	cd hal ; rm -fr */*/*/*.mod.c */*/*/*.mod */*/*/*.o */*/*/.*.cmd */*/*/*.ko
	cd hal ; rm -fr */*/*.mod.c */*/*.mod */*/*.o */*/.*.cmd */*/*.ko
	cd hal ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
	cd hal ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd core ; rm -fr */*.mod.c */*.mod */*.o */.*.cmd */*.ko
	cd core ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd os_dep/linux ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd os_dep ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	cd platform ; rm -fr *.mod.c *.mod *.o .*.cmd *.ko
	rm -fr Module.symvers ; rm -fr Module.markers ; rm -fr modules.order
	rm -fr *.mod.c *.mod *.o .*.cmd *.ko *~
	rm -fr .tmp_versions
endif
