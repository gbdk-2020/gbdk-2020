# GB specific Makefile

TOPDIR = ../..

THIS = gb
PORT = gbz80

CSRC = digits.c gprint.c gprintf.c gprintln.c gprintn.c

ASSRC =	cgb.s cpy_data.s drawing.s f_ibm_sh.s \
	f_italic.s f_min.s f_spect.s get_bk_t.s get_data.s \
	get_wi_t.s get_xy_t.s global.s \
	hiramcpy.s ibmfixed.s init_tt.s input.s \
	pad.s sample.s \
	serial.s set_bk_t.s \
	set_data.s set_prop.s set_spr.s set_wi_t.s set_xy_t.s \
	set_1bit_data.s \
	sgb.s font.s delay.s \
	rand.s arand.s \
	bgb_emu.s \
	nowait.s far_ptr.s \
	lcd.s joy.s tim.s \
	crash_handler.s \
	___sdcc_bcall_ehl.s ___sdcc_bcall.s \
	mv_spr.s \
	pad_ex.s \
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

#OBJ = 	crt0.o putchar.o font.o ibmfixed.o drawing.o init_tt.o \
#	cpy_data.o
