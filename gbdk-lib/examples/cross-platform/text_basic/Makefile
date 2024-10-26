# If you move this project you can change the directory
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
ifndef GBDK_HOME
	GBDK_HOME = ../../../
endif

LCC = $(GBDK_HOME)bin/lcc
PNG2ASSET = $(GBDK_HOME)bin/png2asset 

# Set platforms to build here, spaced separated. (These are in the separate Makefile.targets)
# They can also be built/cleaned individually: "make gg" and "make gg-clean"
# Possible are: gb gbc pocket megaduck sms gg
TARGETS=gb pocket megaduck sms gg nes

# Configure platform specific LCC flags here:
LCCFLAGS_gb      = -Wm-ys -Wl-yt0x1B -autobank # Set an MBC for banking (1B-ROM+MBC5+RAM+BATT)
LCCFLAGS_pocket  = -Wm-ys -Wl-yt0x1B -autobank # Usually the same as required for .gb
LCCFLAGS_duck    = -Wm-ys -Wl-yt0x1B -autobank # Usually the same as required for .gb
LCCFLAGS_gbc     = -Wm-ys -Wl-yt0x1B -Wm-yc -autobank # Same as .gb with: -Wm-yc (gb & gbc) or Wm-yC (gbc exclusive)
LCCFLAGS_sms     = -autobank
LCCFLAGS_gg      = -autobank
LCCFLAGS_nes     = 

LCCFLAGS += $(LCCFLAGS_$(EXT)) # This adds the current platform specific LCC Flags

LCCFLAGS += -Wl-j -Wm-yoA -Wm-ya4 -Wb-ext=.rel -Wb-v # MBC + Autobanking related flags

# GBDK_DEBUG = ON
ifdef GBDK_DEBUG
	LCCFLAGS += -debug -v
endif


# You can set the name of the ROM file here
PROJECTNAME = text_basic

SRCDIR      = src
OBJDIR      = obj/$(EXT)
RESDIR      = res
DISTDIR      = dist
BINDIR      = build/$(EXT)
GENDIR      = gen/$(EXT)/src


MKDIRS      = $(OBJDIR) $(BINDIR) $(GENDIR) # See bottom of Makefile for directory auto-creation
GENSOURCES    = $(foreach dir,$(GENDIR), $(wildcard $(dir)/*.c))
BINS	    = $(OBJDIR)/$(PROJECTNAME).$(EXT)
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)


# png2asset settings for backgrounds
PNG2ASSET_BKG_SETTINGS_gg=-pack_mode sms -bpp 4
PNG2ASSET_BKG_SETTINGS_sms=-pack_mode sms -bpp 4
PNG2ASSET_BKG_SETTINGS_nes=-pack_mode nes -bpp 2
PNG2ASSET_BKG_SETTINGS_gb=
PNG2ASSET_BKG_SETTINGS_gbc=
PNG2ASSET_BKG_SETTINGS_duck=
PNG2ASSET_BKG_SETTINGS_pocket=

# Builds all targets sequentially
all:  $(TARGETS)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) $(CFLAGS) -I$(GENDIR) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(LCCFLAGS) $(CFLAGS) -I$(GENDIR) -c -o $@ $<

# If needed, compile .c files in "src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(LCCFLAGS) $(CFLAGS) -I$(GENDIR) -S -o $@ $<

	


png2asset:
	$(PNG2ASSET) $(RESDIR)/Font.png  $(PNG2ASSET_BKG_SETTINGS_$(EXT)) -c $(GENDIR)/Font.c -map -keep_palette_order  -noflip


# Link the compiled object files into a .gb ROM file
$(BINS):	$(OBJS)
	$(LCC) $(LCCFLAGS) $(CFLAGS) -I$(GENDIR) -o $(BINDIR)/$(PROJECTNAME).$(EXT) $(OBJS) $(GENSOURCES)



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
