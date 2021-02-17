#
# Simple Makefile that compiles all .c and .s files in the same folder
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = "../../../"

LCC = $(GBDK_HOME)bin/lcc 

# You can uncomment the line below to turn on debug output
# LCC = $(LCC) -debug

# You can set the name of the .gb ROM file here
PROJECTNAME    = "Example"

BINS	    = $(PROJECTNAME).gb
CSOURCES   := $(wildcard *.c)
ASMSOURCES := $(wildcard *.s)

# Compiling C files will produce .o files
OBJS       = $(CSOURCES:%.c=%.o) $(ASMSOURCES:%.c=%.o)

all:	$(BINS)

make.bat: Makefile
	@echo "REM Automatically generated from Makefile" > make.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> make.bat

# Compile .c files to .o object files
%.o:	%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .s assembly files to .o object files
%.o:	%.s
	$(LCC) $(CFLAGS) -c -o $@ $<

# If needed, compile .c files to .s assembly files
# (not required if .c is compiled directly to .o)
 %.s:	%.c
	$(LCC) $(CFLAGS) -S -o $@ $<

# Link
$(BINS):	$(OBJS)
	$(LCC) $(CFLAGS) -o $(BINS) $(OBJS)

clean:
	rm -f *.o *.lst *.map *.gb *.ihx *.sym *.cdb *.adb *.asm

