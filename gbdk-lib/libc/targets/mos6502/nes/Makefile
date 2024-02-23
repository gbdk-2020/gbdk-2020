# NES specific Makefile

TOPDIR = ../../../..

THIS = nes
PORT = mos6502

CSRC = crlf.c

ASSRC =	f_ibm_full.s f_ibm_sh.s f_italic.s f_min.s f_spect.s \
	font.s font_color.s set_data.s set_native_data.s set_1bit_data.s color.s mode.s \
	metasprites.s metasprites_flipx.s metasprites_flipy.s metasprites_flipxy.s \
	metasprites_common.s \
	metasprites_hide.s metasprites_hide_spr.s set_sprite.s \
	vram_transfer_buffer.s \
	set_tile.s set_bk_ts.s set_tile_submap.s fill_rect_bk.s \
	set_attribute.s set_bk_attributes.s set_tile_submap_attributes.s flush_attributes.s \
	nes_palettes.s \
	pad.s pad_ex.s \
	rle_decompress.s \
	far_ptr.s sdcc_bcall.s mapper.s \
	lcd.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_6500)

include ../../../Makefile.platform
