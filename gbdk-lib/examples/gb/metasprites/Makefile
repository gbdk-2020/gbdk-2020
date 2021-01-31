CC	= ../../../bin/lcc
PNG2MTSPR = ../../../bin/png2mtspr

CFLAGS	=

BINS	   = metasprites.gb
PNGSOURCES = sprite.png
CSOURCES   = $(PNGSOURCES:%.png=%.c) $(wildcard *.c)

all: $(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat
	
sprite.c: sprite.png
	$(PNG2MTSPR) $< -sh 48 -spr8x16 -c $@ 

$(BINS): $(CSOURCES)
	$(CC) $(CFLAGS) -o $(BINS) $(CSOURCES)
	mv -f sprite.c sprice.pngc

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm $(PNGSOURCES:%.png=%.h) $(PNGSOURCES:%.png=%.c)