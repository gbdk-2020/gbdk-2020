CC	= ../../../bin/lcc -mz80:gg -Wa-l -Wl-m -Wl-j

BINS	= smoketest.gg

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile and link single file in one pass
%.gg:	%.c
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.gg *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi

