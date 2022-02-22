# GB specific Makefile

TOPDIR = ../../../..

THIS = msxdos
PORT = z80

CSRC = crlf.c 

ASSRC =	bdos_putchar.s bdos_getchar.s bdos_gets.s \
	heap.s \
	__sdcc_bcall.s __sdcc_bcall_abc.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
