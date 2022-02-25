# GB specific Makefile

TOPDIR = ../../../..

THIS = msxdos
PORT = z80

CSRC = crlf.c 

ASSRC =	bdos_putchar.s bdos_getchar.s bdos_gets.s \
	msx_set_native_data.s msx_set_1bpp_data.s \
	msx_set_tile_map.s msx_set_tile_map_xy.s \
	msx_fill_rect.s msx_fill_rect_xy.s \
	color.s \
	heap.s \
	__sdcc_bcall.s __sdcc_bcall_abc.s \
	crt0.s

CRT0 =	crt0.s

include $(TOPDIR)/Makefile.common

AS = $(AS_Z80)

include ../../../Makefile.platform
