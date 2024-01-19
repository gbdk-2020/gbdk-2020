LCC	= ../../../bin/lcc
PNG2ASSET = ../../../bin/png2asset

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif


all:
	$(PNG2ASSET) gb_border.png -map -bpp 4 -max_palettes 4 -pack_mode sgb -use_map_attributes -c border_data.c
	$(LCC) $(LCCFLAGS) -Wm-ys -o border.gb border.c sgb_border.c border_data.c

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ -p\/mkdir\/ | grep -v make >> compile.bat

clean:
	rm -f border_data.c border_data.h
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm *.noi *.rst
