#
# Simple Makefile that compiles all .c and .s files in the same folder
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = ../../../

LCC = $(GBDK_HOME)bin/lcc 

# Flag to enable optional Super Game Boy support in the ROM Cartridge header
LCCFLAGS =  -Wm-ys

LCCFLAGS += -debug # Enable debug output
LCCFLAGS += -v     # Enable lcc verbose output
LCCFLAGS += -Wl-u  # Enable .lst -> .rst address update
LCCFLAGS += -Wl-w  # Enable wide listing in .map file

# You can set the name of the .gb ROM file here
PROJECTNAME    = gbtype

BINS	    = $(PROJECTNAME).gb
CSOURCES   := $(wildcard *.c)
ASMSOURCES := $(wildcard *.s)

all:	$(BINS)

compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | sed s/mkdir\ \-p/mkdir/ | grep -v make >> compile.bat

# Compile and link all source files in a single call to LCC
$(BINS):	$(CSOURCES) $(ASMSOURCES)
	$(LCC) $(LCCFLAGS) -o $@ $(CSOURCES) $(ASMSOURCES)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm

