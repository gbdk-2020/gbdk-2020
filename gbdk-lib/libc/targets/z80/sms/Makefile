# GB specific Makefile

TOPDIR = ../../../..

THIS = sms
PORT = z80

CSRC = 

ASSRC =	vmemcpy.s \
	int.s nmi.s \
	sfr.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
