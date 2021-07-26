# GB specific Makefile

TOPDIR = ../..

THIS = gb
PORT = gbz80

CSRC = ../gbz80/digits.c ../gbz80/gprint.c ../gbz80/gprintf.c ../gbz80/gprintln.c ../gbz80/gprintn.c

ASSRC =	../gbz80/cgb.s ../gbz80/cgb_palettes.s ../gbz80/cgb_compat.s \
	../gbz80/cpy_data.s \
	../gbz80/drawing.s ../gbz80/drawing_isr.s ../gbz80/color.s \
	../gbz80/f_ibm_full.s ../gbz80/f_ibm_sh.s ../gbz80/f_italic.s ../gbz80/f_min.s ../gbz80/f_spect.s \
	../gbz80/get_bk_t.s ../gbz80/get_data.s ../gbz80/get_tile.s \
	../gbz80/get_wi_t.s ../gbz80/get_xy_t.s \
	../gbz80/get_addr.s \
	../gbz80/hiramcpy.s ../gbz80/init_tt.s ../gbz80/input.s \
	../gbz80/pad.s ../gbz80/sample.s \
	../gbz80/serial.s ../gbz80/set_bk_t.s ../gbz80/set_tile.s \
	../gbz80/set_data.s ../gbz80/set_prop.s ../gbz80/set_spr.s ../gbz80/set_wi_t.s ../gbz80/set_xy_t.s \
	../gbz80/set_1bit_data.s \
	../gbz80/sgb.s ../gbz80/font.s ../gbz80/delay.s \
	../gbz80/bgb_emu.s ../gbz80/bgb_emu_printf.s \
	../gbz80/nowait.s ../gbz80/far_ptr.s \
	../gbz80/lcd.s ../gbz80/joy.s ../gbz80/tim.s \
	../gbz80/crash_handler.s \
	../gbz80/___sdcc_bcall_ehl.s ../gbz80/___sdcc_bcall.s \
	../gbz80/mv_spr.s \
	../gbz80/pad_ex.s \
	../gbz80/mode.s ../gbz80/clock.s \
	../gbz80/get_t.s ../gbz80/set_t.s ../gbz80/init_vram.s \
	../gbz80/fill_rect.s ../gbz80/fill_rect_bk.s ../gbz80/fill_rect_wi.s \
	../gbz80/metasprites.s ../gbz80/metasprites_flip.s ../gbz80/metasprites_hide.s ../gbz80/metasprites_hide_spr.s \
	../gbz80/set_tile_submap.s ../gbz80/set_win_tile_submap.s \
	../gbz80/gb_decompress.s ../gbz80/gb_decompress_tiles.s \
	../gbz80/heap.s \
	crt0.s

ifeq ($(ASM),asxxxx)
ASSRC +=	sfr.s
endif
ifeq ($(ASM),rgbds)
ASSRC +=	stubs.s
endif

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_GBZ80)

include ../Makefile.platform
