# GB specific Makefile

TOPDIR = ../../../..

THIS = gg
PORT = z80

CSRC =  crlf.c 

ASSRC =	set_interrupts.s \
	outi.s vmemcpy.s \
	sms_refresh_oam.s \
	sms_set_native_data.s sms_set_1bpp_data.s sms_set_2bpp_data.s \
	set_tile_map.s set_tile_map_xy.s set_tile_map_compat.s set_tile_map_xy_compat.s \
	set_tile_submap.s set_tile_submap_compat.s \
	coords_to_address.s \
	set_tile.s \
	sms_fill_rect.s sms_fill_rect_xy.s sms_fill_rect_compat.s sms_fill_rect_xy_compat.s \
	sms_metasprites.s sms_metasprites_hide.s sms_metasprites_hide_spr.s \
	../../f_ibm_full.s ../../f_ibm_sh.s ../../f_italic.s ../../f_min.s ../../f_spect.s \
	font.s color.s \
	putchar.s \
	scroll.s cls.s gotoxy.s \
	palette.s set_palette.s \
	pad.s pad_ex.s \
	sms_int.s nmi.s \
	mode.s clock.s \
	delay.s \
	emu_debug_printf.s \
	memset_small.s \
	far_ptr.s \
	gb_decompress.s \
	rle_decompress.s \
	heap.s \
	__sdcc_bcall.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
