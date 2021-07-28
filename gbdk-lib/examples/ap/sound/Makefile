CC	= ../../../bin/lcc -mgbz80:ap -Wl-j -Wm-yS

BINS	= sound.ap

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile and link single file in one pass
%.ap:	%.c
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.ap *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi

