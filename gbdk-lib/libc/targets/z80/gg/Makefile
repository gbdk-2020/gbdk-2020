# GB specific Makefile

TOPDIR = ../../../..

THIS = gg
PORT = z80

CSRC = 

ASSRC =	outi.s vmemcpy.s \
	set_data.s set_2bpp_data.s \
	set_tile_map.s set_tile_map_xy.s set_tile_map_compat.s set_tile_map_xy_compat.s \
	metasprites.s metasprites_hide.s metasprites_hide_spr.s \
	palette.s \
	pad.s pad_ex.s \
	int.s nmi.s \
	__sdcc_bcall.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
