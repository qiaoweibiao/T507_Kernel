# ccflags-y += -DDEBUG
# ccflags-y += -DKERNEL_VERSION_414
ccflags-y += -I$(srctree)/drivers/net/wireless/uwe5622/sprdwcn/include
ccflags-y += -DOTT_UWE

sprdbt_tty-objs := \
	tty.o \
	rfkill.o \
	lpm.o \
	alignment/sitm.o \
	dump.o

obj-$(CONFIG_TTY_OVERY_SDIO) += sprdbt_tty.o

