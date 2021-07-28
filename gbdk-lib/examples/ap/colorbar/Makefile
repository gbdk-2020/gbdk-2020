CC	= ../../../bin/lcc -mgbz80:ap -Wl-j -Wm-yS

# CFLAGS	=

BINS	= colorbar.ap

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

%.o:	%.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o:	%.s
	$(CC) $(CFLAGS) -c -o $@ $<

%.s:	%.c
	$(CC) $(CFLAGS) -S -o $@ $<

%.ap:	%.o
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.ap *.ihx *.sym *.cdb *.adb *.asm

# Link file, and write 0x80 at position 0x143 in header
colorbar.ap:	colorbar.o
	$(CC) $(CFLAGS) -Wm-yC -o colorbar.ap colorbar.o
