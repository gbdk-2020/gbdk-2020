CC	= ../../../bin/lcc -mgbz80:ap -Wl-j -Wm-yS

BINS	= dscan.ap

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

%.o:	%.c
	$(CC) -c -o $@ $<

%.s:	%.c
	$(CC) -S -o $@ $<

%.o:	%.s
	$(CC) -c -o $@ $<

%.ap:	%.o
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.ap *.ihx *.sym *.cdb *.adb *.asm

# Link file, and write 0x80 at position 0x143 in header
dscan.ap:	dscan.o
	$(CC) -Wm-yc -o $@ $<
