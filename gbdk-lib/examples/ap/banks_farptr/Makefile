CC	= ../../../bin/lcc -mgbz80:ap -Wl-j -Wm-yS

BINS	= banks_farptr.pocket \

all:	$(BINS)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

%.o:	%.c
	$(CC) -c -o $@ $<

%.s:	%.c
	$(CC) -S -o $@ $<

%.o:	%.s
	$(CC) -c -o $@ $<

%.pocket:	%.o
	$(CC) -o $@ $<

clean:
	rm -f *.o *.lst *.map *.pocket *~ *.rel *.cdb *.ihx *.lnk *.sym *.asm *.noi

############################################################
# Banks using Far pointers example

bank2code.o:	bank2code.c
	$(CC) -c -o $@ $<

banks_farptr.pocket:	banks_farptr.o bank2code.o
	$(CC) -Wl-yt0x19 -Wl-yoA -o $@ bank2code.o banks_farptr.o

