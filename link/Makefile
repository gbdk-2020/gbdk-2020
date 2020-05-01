PRJDIR = ./

LINK_DIR = $(shell pwd)

include Makefile.common

PORTS = gbz80

ifeq ($(OS),Windows_NT)
	EXT = .exe
endif

all:
	@echo Building link-gbz80
	@$(MAKE) -C z80 _link-gbz80 E=$(E) BUILDDIR=../bin/

install: all
	@echo Installing link-gbz80
	@$(INSTALL) $(PRJDIR)/bin/link-gbz80 `echo $(BUILDDIR)/bin/link-gbz80|sed '$(transform)'`
	@$(STRIP) `echo $(BUILDDIR)/bin/link-gbz80$(EXT)|sed '$(transform)'`

include clean.mk
