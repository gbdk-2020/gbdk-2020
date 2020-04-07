PRJDIR = ./

LINK_DIR = $(shell pwd)

include Makefile.common

PORTS = gbz80

all: 
	$(MAKE) -C z80 _link-gbz80 E=$(E) BUILDDIR=../bin/

install: all
	$(INSTALL) $(PRJDIR)/bin/link-gbz80 `echo $(BUILDDIR)/bin/link-gbz80|sed '$(transform)'`
	$(STRIP) `echo $(BUILDDIR)/bin/link-gbz80.exe|sed '$(transform)'`

include clean.mk
