# GB specific Makefile

TOPDIR = ../../../..

THIS = gg
PORT = z80

CSRC =

ASSRC =	set_interrupts.s \
	outi.s vmemcpy.s \
	refresh_oam.s \
	set_data.s set_2bpp_data.s \
	set_tile_map.s set_tile_map_xy.s set_tile_map_compat.s set_tile_map_xy_compat.s \
	set_tile_submap.s set_tile_submap_compat.s \
	coords_to_address.s \
	set_tile.s \
	fill_rect.s fill_rect_xy.s fill_rect_compat.s fill_rect_xy_compat.s \
	metasprites.s metasprites_hide.s metasprites_hide_spr.s \
	f_ibm_full.s f_ibm_sh.s f_italic.s f_min.s f_spect.s \
	font.s \
	putchar.s \
	scroll.s cls.s gotoxy.s \
	palette.s set_palette.s \
	pad.s pad_ex.s \
	int.s nmi.s \
	mode.s \
	memset_small.s \
	far_ptr.s \
	__sdcc_bcall.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
