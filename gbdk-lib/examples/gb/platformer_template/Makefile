#
# A Makefile that compiles all .c and .s files in "src" and "res" 
# subdirectories and places the output in a "obj" subdirectory
#

# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ../../../
endif

PNG2ASSET = $(GBDK_HOME)bin/png2asset 
LCC = $(GBDK_HOME)bin/lcc 

# You can set flags for LCC here
# For example, you can uncomment the line below to turn on debug output
# LCCFLAGS = -debug

# You can set the name of the .gb ROM file here
PROJECTNAME    = PlatformerTemplate

SRCDIR      = src
DISTDIR      = dist
OBJDIR      = obj
RESDIR      = res
GENDIR      = gen
BINS	    = $(DISTDIR)/$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(GENDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all:	prepare png2asset $(BINS)

png2asset: prepare
	$(PNG2ASSET) res\player-character.png -c gen\PlayerCharacter.c -px 12 -py 6 -spr8x16 -keep_palette_order -sw 24 -sh 32
	$(PNG2ASSET) res\world1-tileset.png -c gen\World1Tileset.c -noflip -map -tileset_only -keep_duplicate_tiles
	$(PNG2ASSET) res\world2-tileset.png -c gen\World2Tileset.c -noflip -map -tileset_only -keep_duplicate_tiles
	$(PNG2ASSET) res\world1-area1.png -c gen\World1Area1.c -noflip -map -maps_only -source_tileset res\world1-tileset.png
	$(PNG2ASSET) res\world1-area2.png -c gen\World1Area2.c -noflip -map -maps_only -source_tileset res\world1-tileset.png
	$(PNG2ASSET) res\world2-area1.png -c gen\World2Area1.c -noflip -map -maps_only -source_tileset res\world2-tileset.png



compile.bat: Makefile
	@echo "REM Automatically generated from Makefile" > compile.bat
	@make -sn | sed y/\\//\\\\/ | grep -v make >> compile.bat

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -Ires -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(LCCFLAGS) -Ires -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) -Ires -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) -Ires -S -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) -Igen -o $(BINS) $(OBJS)

prepare:
	mkdir -p $(OBJDIR)
	mkdir -p $(DISTDIR)
	mkdir -p $(GENDIR)

clean:
#	rm -f  *.gb *.ihx *.cdb *.adb *.noi *.map
	rm -f  $(OBJDIR)/*.*
	rm -f  $(DISTDIR)/*.*
	rm -f  $(GENDIR)/*.*

