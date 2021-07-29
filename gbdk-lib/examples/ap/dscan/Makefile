CC	= ../../../bin/lcc -mgbz80:ap -Wl-j -Wm-yS

BINS	= dscan.pocket

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

%.pocket:	%.o
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.pocket *.ihx *.sym *.cdb *.adb *.asm

# Link file, and write 0x80 at position 0x143 in header
dscan.pocket:	dscan.o
	$(CC) -Wm-yc -o $@ $<
