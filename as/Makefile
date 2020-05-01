PRJDIR = .
include $(PRJDIR)/Makefile.common

PORTS = gbz80

ifeq ($(OS),Windows_NT)
	EXT = .exe
endif

all:
	@echo Building as-gbz80
	@$(MAKE) -C z80 _as-gbz80 E=$(E) BUILDDIR=../bin/

install: all
	@echo Installing link-gbz80
	@$(INSTALL) $(PRJDIR)/bin/as-gbz80 `echo $(BUILDDIR)/bin/as-gbz80|sed '$(transform)'`
	@$(STRIP) `echo $(BUILDDIR)/bin/as-gbz80$(EXT)|sed '$(transform)'`

include clean.mk
