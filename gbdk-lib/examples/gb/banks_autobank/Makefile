LCC	= lcc -Wa-l -Wl-m

BINS = autobanks.gb
CSOURCES = $(wildcard src/*.c)
OBJS = $(addprefix obj/, $(notdir $(CSOURCES:.c=.o)))

all: obj $(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

obj:
	mkdir obj

obj/%.o: src/%.c
	@echo $(LCC)
	$(LCC) -c -o $@ $<


# Process: .c and .s files -> .o -> (bankpack) -> .rel -> (linker) ... -> .gb
#
# -autobank    : Tells lcc to call bankpack for the compiled object (.o) files before linking
# -Wb-ext=.rel : Set output extension for autobanked files to .rel
# -Wb-v        : Prints out assigned autobank info (optional)
# -Wl-yoA      : Have makebin automatically calculate required number of ROM banks (otherwise use -Wl-yo4 in this example)
# -Wl-ya4      : Use 4 RAM banks
# -Wl-yt19     : Use MBC5 cartridge type
$(BINS): $(OBJS)
	$(LCC) -autobank -Wb-ext=.rel -Wb-v -Wl-yt19 -Wl-yoA -Wl-ya4 -o $(BINS) $(OBJS)

clean:
	rm -f obj/* $(BINS) *.ihx *.map *.noi

# For lcc linker option: -Wl-ytN where N is one of the numbers below
#
# ROM Byte 0147: Cartridge type:
# 0-ROM ONLY            12-ROM+MBC3+RAM
# 1-ROM+MBC1            13-ROM+MBC3+RAM+BATT
# 2-ROM+MBC1+RAM        19-ROM+MBC5
# 3-ROM+MBC1+RAM+BATT   1A-ROM+MBC5+RAM
# 5-ROM+MBC2            1B-ROM+MBC5+RAM+BATT
# 6-ROM+MBC2+BATTERY    1C-ROM+MBC5+RUMBLE
# 8-ROM+RAM             1D-ROM+MBC5+RUMBLE+SRAM
# 9-ROM+RAM+BATTERY     1E-ROM+MBC5+RUMBLE+SRAM+BATT
# B-ROM+MMM01           1F-Pocket Camera
# C-ROM+MMM01+SRAM      FD-Bandai TAMA5
# D-ROM+MMM01+SRAM+BATT FE - Hudson HuC-3
# F-ROM+MBC3+TIMER+BATT FF - Hudson HuC-1
# 10-ROM+MBC3+TIMER+RAM+BATT
# 11-ROM+MBC3

