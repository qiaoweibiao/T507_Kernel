ccflags-y := \
	-I$(src)/include \
	-I$(src)/boot \
	-I$(src)/platform \
	-I$(src)/pcie \
	-I$(src)/platform/rf

ccflags-y += -I$(src)/port

ccflags-$(CONFIG_CPLOG_DEBUG) += \
	-DCONFIG_WCN_LOOPCHECK \
	-DCONFIG_CPLOG_DEBUG

wcn_bsp-y := wcn_bus.o
wcn_bsp-y += port/wcn_port.o

# CONFIG_WCN_SLP
wcn_bsp-$(CONFIG_WCN_SLP) += \
	sleep/sdio_int.o \
	sleep/slp_mgr.o \
	sleep/slp_sdio.o \
	sleep/slp_test.o

# CONFIG_SDIOHAL
wcn_bsp-$(CONFIG_SDIOHAL) += \
	platform/mem_pd_mgr.o \
	sdio/sdiohal_main.o \
	sdio/sdiohal_common.o \
	sdio/sdiohal_tx.o \
	sdio/sdiohal_rx.o \
	sdio/sdio_v3.o

ifdef CONFIG_DEBUG_FS
wcn_bsp-$(CONFIG_SDIOHAL) += \
	sdio/sdiohal_ctl.o
endif

# CONFIG_WCN_PCIE
wcn_bsp-$(CONFIG_WCN_PCIE) += \
	platform/pcie_boot.o

# CONFIG_WCN_UTILS
# wcn_bsp-$(CONFIG_WCN_UTILS) += \
#	platform/wcn_debugfs.o

# CONFIG_WCN_BOOT
wcn_bsp-$(CONFIG_WCN_BOOT) += \
	platform/wcn_boot.o \
	platform/wcn_dump.o \
	platform/bufring.o \
	platform/wcn_txrx.o \
	platform/wcn_op.o \
	platform/wcn_parn_parser.o \
	platform/rdc_debug.o \
	platform/wcn_log.o \
	platform/wcn_procfs.o \
	platform/loopcheck.o

# CONFIG_WCN_PLATFORM
wcn_bsp-$(CONFIG_WCN_PLATFORM) += \
	platform/wcn_parn_parser.o \
	platform/wcn_procfs.o \
	platform/wcn_txrx.o \
	platform/rdc_debug.o \
	platform/wcn_log.o \
	platform/rf/rf.o \
	platform/bufring.o

# CONFIG_WCN_GNSS
wcn_bsp-$(CONFIG_WCN_GNSS) += \
	platform/gnss/gnss_common_ctl.o \
	platform/gnss/gnss_pmnotify_ctl.o \
	platform/gnss/gnss_dbg.o \
	platform/gnss/gnss_dump.o

# CONFIG_SC2342_INTEG
wcn_bsp-$(CONFIG_SC2342_INTEG) +=  \
	boot/wcn_integrate_dev.o \
	boot/wcn_integrate_boot.o \
	boot/wcn_integrate.o \
	platform/rf/rf.o \
	platform/wcn_misc.o \
	platform/wcn_dump_integrate.o \
	platform/wcn_misc.o

# CONFIG_ARCH_SUNXI
ifeq ($(CONFIG_ARCH_SUNXI),y)
WCN_BSP_TOP := $(dir $(lastword $(MAKEFILE_LIST)))
$(shell cp $(WCN_BSP_TOP)/platform/wcn_bind_verify.lib $(WCN_BSP_TOP)/platform/wcn_bind_verify.a)

wcn_bsp-$(CONFIG_ARCH_SUNXI) += \
	platform/wcn_bind_verify.a
endif

obj-m := wcn_bsp.o
