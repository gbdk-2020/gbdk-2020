# GB specific Makefile

TOPDIR = ../../../..

THIS = ap
PORT = sm83

CSRC = crlf.c digits.c gprint.c gprintf.c gprintln.c gprintn.c

ASSRC =	cgb.s cgb_palettes.s cgb_compat.s \
	cpy_data.s \
	drawing.s drawing_isr.s color.s \
	../../f_ibm_full.s ../../f_ibm_sh.s ../../f_italic.s ../../f_min.s ../../f_spect.s \
	get_bk_t.s get_data.s get_tile.s \
	get_wi_t.s get_xy_t.s \
	get_addr.s \
	hiramcpy.s init_tt.s input.s \
	pad.s \
	serial.s set_bk_t.s set_tile.s \
	set_data.s set_prop.s set_spr.s set_wi_t.s set_xy_t.s \
	set_1bit_data.s \
	sgb.s font.s font_color.s delay.s \
	emu_debug.s emu_debug_printf.s \
	nowait.s far_ptr.s \
	lcd.s joy.s tim.s tim_nested.s tim_common.s \
	crash_handler.s \
	___sdcc_bcall_ehl.s ___sdcc_bcall.s \
	mv_spr.s \
	pad_ex.s \
	mode.s clock.s \
	get_t.s set_t.s init_vram.s \
	fill_rect.s fill_rect_bk.s fill_rect_wi.s \
	metasprites.s metasprites_flip.s metasprites_hide.s metasprites_hide_spr.s \
	set_tile_submap.s set_win_tile_submap.s \
	gb_decompress.s gb_decompress_tiles.s \
	rle_decompress.s \
	heap.s \
	sfr.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_SM83)

include ../../../Makefile.platform
