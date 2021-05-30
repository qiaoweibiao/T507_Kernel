ccflags-y += -I$(srctree)/drivers/net/wireless/uwe5622/sprdwcn/include

####add cflag######
ccflags-y += -DIBSS_SUPPORT -DIBSS_RSN_SUPPORT
ccflags-y += -DNAN_SUPPORT
ccflags-y += -DRTT_SUPPORT
ccflags-y += -DACS_SUPPORT -DRX_HW_CSUM
ccflags-y += -DWMMAC_WFA_CERTIFICATION
ccflags-y += -DCOMPAT_SAMPILE_CODE
ccflags-y += -DRND_MAC_SUPPORT
ccflags-y += -DSOFTAP_HOOK
ccflags-y += -DATCMD_ASSERT
ccflags-y += -DTCPACK_DELAY_SUPPORT
ccflags-$(DEBUGMODE) += -DWL_CONFIG_DEBUG
ccflags-y += -DSPLIT_STACK
ccflags-y += -DOTT_UWE

#######add .o file#####
sprdwl_ng-objs += main.o cfg80211.o txrx.o cmdevt.o npi.o msg.o work.o vendor.o \
				  tcp_ack.o mm.o reorder.o wl_core.o tx_msg.o rx_msg.o \
				  wl_intf.o qos.o dbg_ini_util.o reg_domain.o
sprdwl_ng-objs += defrag.o
sprdwl_ng-objs += ibss.o
sprdwl_ng-objs += nan.o
sprdwl_ng-objs += tracer.o
sprdwl_ng-objs += rf_marlin3.o
sprdwl_ng-objs += rtt.o
sprdwl_ng-objs += api_version.o
sprdwl_ng-objs += rnd_mac_addr.o
sprdwl_ng-objs += softap_hook.o
sprdwl_ng-objs += debug.o
#sprdwl_ng-objs += 11h.o

obj-$(CONFIG_WLAN_UWE5622) += sprdwl_ng.o
