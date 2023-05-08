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
OBJDIR      = obj
RESDIR      = res
BINS	    = $(OBJDIR)/$(PROJECTNAME).gb
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

all:	prepare png2asset $(BINS)

png2asset: prepare
	$(PNG2ASSET) res/mario.png -c res/Mario.c -px 12 -py 6 -spr8x16 -keep_palette_order -sw 24 -sh 32
	$(PNG2ASSET) res/map-tileset.png -c res/MapTileset.c -map -keep_palette_order -tileset_only
	$(PNG2ASSET) res/map-tileset2.png -c res/MapTileset2.c -map -keep_palette_order -tileset_only
	$(PNG2ASSET) res/map.png -c res/Map.c -map -keep_palette_order -tileset_only -source_tileset res/map-tileset.png
	$(PNG2ASSET) res/map2.png -c res/Map2.c -map -keep_palette_order -tileset_only -source_tileset res/map-tileset.png
	$(PNG2ASSET) res/map3.png -c res/Map3.c -map -keep_palette_order -tileset_only -source_tileset res/map-tileset2.png


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
	$(LCC) $(LCCFLAGS) -Ires -o $(BINS) $(OBJS)

prepare:
	mkdir -p $(OBJDIR)

clean:
#	rm -f  *.gb *.ihx *.cdb *.adb *.noi *.map
	rm -f  $(OBJDIR)/*.*

