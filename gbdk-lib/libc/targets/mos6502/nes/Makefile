# NES specific Makefile

TOPDIR = ../../../..

THIS = nes
PORT = mos6502

CSRC = crlf.c

ASSRC =	f_ibm_full.s f_ibm_sh.s f_italic.s f_min.s f_spect.s \
	font.s font_color.s set_data.s set_1bit_data.s color.s mode.s \
	metasprites.s metasprites_hide.s metasprites_hide_spr.s \
	fill_rect_bk.s \
	pad.s pad_ex.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_6500)

include ../../../Makefile.platform
