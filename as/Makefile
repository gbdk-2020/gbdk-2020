PRJDIR = .
include $(PRJDIR)/Makefile.common

PORTS = gbz80

all:
	$(MAKE) -C z80 _as-gbz80 E=$(E) BUILDDIR=../bin/

install: all install-doc
	$(INSTALL) $(PRJDIR)/bin/as-gbz80 `echo $(BUILDDIR)/bin/as-gbz80|sed '$(transform)'`
	$(STRIP) `echo $(BUILDDIR)/bin/as-gbz80.exe|sed '$(transform)'`

install-doc:
	$(INSTALL) -d $(docdir)/aslink

include clean.mk
