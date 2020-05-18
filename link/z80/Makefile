PRJDIR = ..

include $(PRJDIR)/Makefile.common

OBJDIR	= obj/$(EXT)

SLIBSRC	= NewAlloc.c

SRC	= lkarea.c lkdata.c lkeval.c lkhead.c lkihx.c lklex.c \
	  lklibr.c lklist.c lkmain.c lkrloc.c lks19.c lksym.c \
	  lkgb.c lkgg.c 
	  
SRC_CPP = far_fixer.cpp

OBJS	= $(SRC:%.c=$(OBJDIR)/%.o) 
SLIBOBJS	= $(SLIBSRC:%.c=$(OBJDIR)/%.o) 
SCPPOBJS = $(SRC_CPP:%.cpp=$(OBJDIR)/%.o) 

BINS	= $(BUILDDIR)link$(EXT)

CFLAGS	+= $(CPPFLAGS) $(OPTS) -DINDEXLIB -DMLH_MAP -DUNIX -DSDK
CFLAGS	+= -funsigned-char -DUNIX
CFLAGS	+= -I$(PRJDIR)/as/$(PORT) -I$(SLIB) 

LDFLAGS += -lm $(EXTRALIBS) -static-libgcc -static-libstdc++

all:	$(BINS)

$(BINS): $(OBJDIR) $(BUILDDIR) $(OBJS) $(SLIBOBJS) $(SCPPOBJS)
	@echo Linking
	@g++ -static-libstdc++ -g -o $(BINS) $(OBJS) $(SLIBOBJS) $(SCPPOBJS) $(LDFLAGS)

$(OBJDIR):
	@echo Creating dir $(OBJDIR)
	@mkdir -p $(OBJDIR)
	
$(BUILDDIR):
	@echo Creating dir $(BUILDDIR)
	@mkdir -p $(BUILDDIR)

$(OBJDIR)/%.o:	%.c
	@echo compiling $<
	@$(CC) -c $(CFLAGS) -o $@ $<

$(OBJDIR)/%.o:	%.cpp
	@echo compiling $<
	@g++ -c -o $@ $<

$(OBJDIR)/%.o:	$(SLIB)/%.c
	@echo compiling $<
	@$(CC) -c $(CFLAGS) -o $@ $<

_link-z80:
	@$(MAKE) EXT=-z80$(E) PORT=z80

_link-gbz80:
	@$(MAKE) EXT=-gbz80$(E) OPTS=-DGAMEBOY PORT=z80

include clean.mk
