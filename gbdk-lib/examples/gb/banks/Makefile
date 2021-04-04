CC	= lcc -Wa-l -Wl-m -Wl-j

BINS = banks.gb

all: obj $(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

obj:
	mkdir obj

clean:
	rm -f obj/* $(BINS) *.ihx *.map *.noi

############################################################
# Multiple bank example

# Compile bank 0 (no ROM)
#      RAM bank 0 : -Wf-ba0
#
obj/bank_0.o:	src/bank_0.c
	$(CC) -Wf-ba0 -c -o $@ $<

# Compile bank 1 (ROM+RAM)
#      ROM bank 1 : -Wf-bo1
#      RAM bank 1 : -Wf-ba1
#
obj/bank_1.o:	src/bank_1.c
	$(CC) -Wf-bo1 -Wf-ba1 -c -o $@ $<

# Compile bank 2 (ROM+RAM)
#      ROM bank 2 : -Wf-bo2
#      RAM bank 2 : -Wf-ba2
#
obj/bank_2.o:	src/bank_2.c
	$(CC) -Wf-bo2 -Wf-ba2 -c -o $@ $<

# Compile bank 3 (ROM+RAM)
#      ROM bank 3 : -Wf-bo3
#      RAM bank 3 : -Wf-ba3
#
obj/bank_3.o:	src/bank_3.c
	$(CC) -Wf-bo3 -Wf-ba3 -c -o $@ $<

# Link banks
#      ROM+MBC1+RAM : -Wl-yt2
#      4 ROM banks  : -Wl-yo4
#      4 RAM banks  : -Wl-ya4
#
banks.gb: src/banks.c obj/bank_0.o obj/bank_1.o obj/bank_2.o obj/bank_3.o
	$(CC) -Wl-yt2 -Wl-yo4 -Wl-ya4 -o $@ $^

