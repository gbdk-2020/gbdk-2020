# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ../../../
endif

LCC = $(GBDK_HOME)bin/lcc
PNG2ASSETS = $(GBDK_HOME)bin/png2asset

# Set platforms to build here, spaced separated. (These are in the separate Makefile.targets)
# They can also be built/cleaned individually: "make gg" and "make gg-clean"
# Possible are: gb gbc pocket megaduck sms gg
TARGETS=gb gbc pocket megaduck sms gg nes

# You can set the name of the resulting ROM file here
PROJECTNAME = logo

# You can set the name of the ROM title embedded in the ROM header here
TITLE = LOGO # must be all upper case, maximum 15 characters

# Configure platform specific LCC flags here:
LCCFLAGS_gb      = -Wl-yt0x1B -autobank # Set an MBC for banking (1B-ROM+MBC5+RAM+BATT)
LCCFLAGS_pocket  = -Wl-yt0x1B -autobank # Usually the same as required for .gb
LCCFLAGS_duck    = -Wl-yt0x1B -autobank # Usually the same as required for .gb
LCCFLAGS_gbc     = -Wl-yt0x1B -Wm-yc -autobank # Same as .gb with: -Wm-yc (gb & gbc) or Wm-yC (gbc exclusive)
LCCFLAGS_sms     =
LCCFLAGS_gg      =
LCCFLAGS_nes     =

LCCFLAGS += $(LCCFLAGS_$(EXT)) # This adds the current platform specific LCC Flags

LCCFLAGS += -Wl-j -Wm-yoA -Wm-ya4 -Wb-ext=.rel -Wb-v # MBC + Autobanking related flags

LCCFLAGS += -Wm-yn"$(TITLE)" # Add title to rom cartridge header

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif


RESFLAGS += $(RESFLAGS_$(TYP))

CFLAGS += -I$(OBJDIR) -DSYSTEM_$(TYP)

# EXT?=gb # Only sets extension to default (game boy .gb) if not populated
SRCDIR      = src
OBJDIR      = obj/$(EXT)
RESDIR      = res/$(TYP)
BINDIR      = build/$(EXT)
MKDIRS      = $(OBJDIR) $(BINDIR) # See bottom of Makefile for directory auto-creation

BINS	    = $(OBJDIR)/$(PROJECTNAME).$(EXT)
IMAGES      = $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.png)))
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS        = $(IMAGES:%.png=$(OBJDIR)/%.o) $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

# Builds all targets sequentially
all: $(TARGETS)

.SECONDEXPANSION:
$(OBJDIR)/%.c: $(RESDIR)/%.png $$(wildcard $(RESDIR)/gfx/$(PLAT)/backgrounds/%.png.meta)
	$(PNG2ASSETS) $< `cat <$<.meta 2>/dev/null` -c $@

$(OBJDIR)/%.o:	$(OBJDIR)/%.c
	$(LCC) $(LCCFLAGS) $(CFLAGS) -c -o $@ $<

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) $(CFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(LCCFLAGS) $(CFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) $(CFLAGS) -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) $(CFLAGS) -S -o $@ $<

# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) $(CFLAGS) -o $(BINDIR)/$(PROJECTNAME).$(EXT) $(OBJS)

clean:
	@echo Cleaning
	@for target in $(TARGETS); do \
		$(MAKE) $$target-clean; \
	done

# Include available build targets
include Makefile.targets


# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
ifneq ($(strip $(EXT)),)           # Only make the directories if EXT has been set by a target
$(info $(shell mkdir -p $(MKDIRS)))
endif
