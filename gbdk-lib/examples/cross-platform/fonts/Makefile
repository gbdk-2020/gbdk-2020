# Note: "override" ensures that appending to the variables still works if the variable was set from the command line
#
# If you move this project you can change the directory 
# to match your GBDK root directory (ex: GBDK_HOME = "C:/GBDK/"
GBDK_HOME = ../../../
LCC = $(GBDK_HOME)bin/lcc 

# Set platforms to build here (spaced separated). Possible are: gb gbc pocket sms gg
# These are found in the separate Makefile.targets
TARGETS=gb pocket sms gg

# MBC + Autobanking related flags
override LCCFLAGS += -Wl-j -Wm-yoA -Wm-ya4 -autobank -Wb-ext=.rel -Wb-v
# override LCCFLAGS += -debug # Uncomment to enable debug output
# override LCCFLAGS += -v     # Uncomment for lcc verbose output

# You can set the name of the ROM file here
PROJECTNAME = Example

# EXT?=gb # Only sets extension to default (game boy .gb) if not populated
SRCDIR      = src
OBJDIR      = obj/$(EXT)
RESDIR      = res
BINDIR      = build/$(EXT)
MKDIRS      = $(OBJDIR) $(BINDIR) # See bottom of Makefile for directory auto-creation

BINS	    = $(OBJDIR)/$(PROJECTNAME).$(EXT)
CSOURCES    = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.c))) $(foreach dir,$(RESDIR),$(notdir $(wildcard $(dir)/*.c)))
ASMSOURCES  = $(foreach dir,$(SRCDIR),$(notdir $(wildcard $(dir)/*.s)))
OBJS       = $(CSOURCES:%.c=$(OBJDIR)/%.o) $(ASMSOURCES:%.s=$(OBJDIR)/%.o)

# Builds all targets sequentially
all: $(TARGETS)

# Compile .c files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .c files in "res/" to .o object files
$(OBJDIR)/%.o:	$(RESDIR)/%.c
	$(LCC) $(CFLAGS) -c -o $@ $<

# Compile .s assembly files in "src/" to .o object files
$(OBJDIR)/%.o:	$(SRCDIR)/%.s
	$(LCC) $(CFLAGS) -c -o $@ $<

# If needed, compile .c files i n"src/" to .s assembly files
# (not required if .c is compiled directly to .o)
$(OBJDIR)/%.s:	$(SRCDIR)/%.c
	$(LCC) $(CFLAGS) -S -o $@ $<

# Link the compiled object files into a .gb ROM file
# Then copy the ROM into a binary directory
$(BINS):	$(OBJS)
	@echo $(LCCFLAGS)
	$(LCC) $(LCCFLAGS) $(CFLAGS) -o $(BINDIR)/$(PROJECTNAME).$(EXT) $(OBJS)

clean:
	@echo Cleaning
	@for target in $(TARGETS); do \
		$(MAKE) $$target-clean; \
	done

cleanbin:
	rm -f  $(BINDIR)/*.*

# Include available build targets
include Makefile.targets


# create necessary directories after Makefile is parsed but before build
# info prevents the command from being pasted into the makefile
ifneq ($(strip $(EXT)),)           # Only make the directories if EXT has been set by a target
$(info $(shell mkdir -p $(MKDIRS)))
endif
