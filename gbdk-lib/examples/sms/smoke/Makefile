CC	= ../../../bin/lcc -mz80:sms -Wa-l -Wl-m -Wl-j

BINS	= smoketest.sms

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile and link single file in one pass
%.sms:	%.c
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.sms *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi

