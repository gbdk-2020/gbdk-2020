CC	= ../../../bin/lcc -mgbz80:ap -Wl-j -Wm-yS

BINS	= gb-dtmf.ap

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

%.s:	%.c
	$(CC) -S -o $@ $<

%.o:	%.c
	$(CC) -c -o $@ $<

%.o:	%.s
	$(CC) -c -o $@ $<

%.ap:	%.o
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.ap *.ihx *.sym *.cdb *.adb *.asm
