# Top level Makefile for GBDK that combines the builds for sdcc,
# gbdk-lib, and gbdk-support
#
# 2001  Michael Hope <michaelh@juju.net.nz>
# $Id: Makefile,v 1.4 2001/11/04 18:43:51 michaelh Exp $
#
TOPDIR = $(shell pwd)

# Package name, used for tarballs and cvs
PKG = gbdk
# Version, used for tarballs
VER = 3.00
# Short version, used for cvs tags
SHORTVER = 300

# Prefix to add to the standard tools.  Usefull for a standard gcc
# cross-compile.
TOOLSPREFIX =

TARGETCC = $(TOOLSPREFIX)gcc
TARGETRANLIB = $(TOOLSPREFIX)ranlib
TARGETAR = $(TOOLSPREFIX)ar
TARGETCXX = $(TOOLSPREFIX)g++
TARGETSTRIP = $(TOOLSPREFIX)strip
# Add extra flags here.  g++ 2.95.4 requires -fdollars-in-identifiers
TARGETCXXFLAGS =

# Directory containing the source to gbdk-lib
GBDKLIBDIR = $(TOPDIR)/gbdk-lib
# Directory containing the source to gbdk-support
GBDKSUPPORTDIR = $(TOPDIR)/gbdk-support
# Directory containing the source to maccer
MACCERDIR = $(TOPDIR)/maccer
# Directory containing the source to linker
LINKERDIR = $(TOPDIR)/link
# Directory containing the source to assembler
ASDIR = $(TOPDIR)/as

# Base setup
# Extension to add to executables
EXEEXTENSION = 
# Host operating system identifier.  The one below should work for
# most systems.
HOSTOS = ppc-unknown-linux2.2
# Target operating system identifier.  Used in the output zip name.
TARGETOS = ppc-unknown-linux2.2

# Directory that gbdk should finally end up in
TARGETDIR = /opt/gbdk
# Directory that gbdk for this target is built into.
BUILDDIR = $(TOPDIR)/build/gbdk

NOISELOG = $(TOPDIR)/noise.log

all: native-build

clean: maccer-clean linker-clean gbdk-support-clean gbdk-lib-clean

distclean: clean build-dir-clean

# Build rule for michaelh's machine to spin a release
sapphire-full-build: native-build binary cross-clean cross-linux-mingw32-build

# Cross-compilation targets
FIXUPMASKS = *.c *.h .bat *.s ChangeLog README

native-build: gbdk-build gbdk-install

cross-clean: sdcc-clean gbdk-support-clean

cross-build: gbdk-build gbdk-install cross-cleanup

cross-cleanup:
ifdef MSDOSLINEENDS
	for i in $(FIXUPMASKS); do \
		find $(BUILDDIR) -name $$i -exec unix2dos {} \; ; \
	done
endif

cross-linux-mingw32-build:
	$(MAKE) TARGETOS=i586-mingw32msvc \
		TOOLSPREFIX=i586-mingw32msvc- \
		EXEEXTENSION=.exe \
		CROSSCOMPILING=1 \
		MSDOSLINEENDS=1 \
		TARGETDIR=/sdcc \
		ARCHIVETYPE=zip \
		cross-build gbdk-lib-examples-makefile binary

# Source code drop
src: clean
	rm -rf gbdk
	mkdir -p gbdk
	cp -r Makefile sdcc gbdk-lib gbdk-support gbdk
	rm -rf `find gbdk -name CVS`
	tar czf gbdk-$(VER).tar.gz gbdk

# Base rules
gbdk-build: maccer-build linker-build gbdk-support-build gbdk-lib-build 

gbdk-install: $(BUILDDIR)/bin linker-install gbdk-support-install gbdk-lib-install sdcc-install

# Directories
$(BUILDDIR)/bin:
	@echo Creating dir $(BUILDDIR)/bin
	@mkdir -p $@

build-dir-clean:
	rm -r $(BUILDDIR)

# Setup rules
CVSFLAGS = -r $(PKG)-$(SHORTVER)

setup-from-local:
	rm -rf sdcc gbdk-lib gbdk-support
	ln -s ../sdcc
	ln -s ../gbdk-lib
	ln -s ../gbdk-support

setup-from-cvs:
	cvs -d :pserver:anonymous@cvs.sdcc.sourceforge.net:/cvsroot/sdcc -q co $(CVSFLAGS) sdcc
	cvs -d :pserver:anonymous@cvs.gbdk.sourceforge.net:/cvsroot/gbdk -q co $(CVSFLAGS) gbdk-lib
	cvs -d :pserver:anonymous@cvs.gbdk.sourceforge.net:/cvsroot/gbdk -q co $(CVSFLAGS) gbdk-support

# Rules for sdcc


# Rules for gbdk-support
gbdk-support-build:
	@echo Building lcc
	@$(MAKE) -C $(GBDKSUPPORTDIR)/lcc TOOLSPREFIX=$(TOOLSPREFIX) TARGETDIR=$(TARGETDIR)/ --no-print-directory
	@echo

gbdk-support-install: gbdk-support-build $(BUILDDIR)/bin
	@echo Installing lcc
	@cp $(GBDKSUPPORTDIR)/lcc/lcc $(BUILDDIR)/bin/lcc$(EXEEXTENSION)
	@$(TARGETSTRIP) $(BUILDDIR)/bin/lcc*
	@cp $(GBDKSUPPORTDIR)/ChangeLog $(BUILDDIR)
	@cp $(GBDKSUPPORTDIR)/README $(BUILDDIR)
	@echo

gbdk-support-clean:
	@echo Cleaning lcc
	@$(MAKE) -C $(GBDKSUPPORTDIR)/lcc clean --no-print-directory
	@echo 

# Rules for gbdk-lib
gbdk-lib-build: check-SDCCDIR
ifndef CROSSCOMPILING
	@echo Building lib
	@$(MAKE) -C $(GBDKLIBDIR)/libc PORTS=gbz80 PLATFORMS=gb --no-print-directory
	@echo
endif

gbdk-lib-install: gbdk-lib-build
	@echo Installing Examples
	@cp -r $(GBDKLIBDIR)/include $(GBDKLIBDIR)/examples $(BUILDDIR)
	@echo
	@echo Installing lib
	@rm -rf $(BUILDDIR)/lib
	@cp -r $(GBDKLIBDIR)/build $(BUILDDIR)/lib
	@rm $(BUILDDIR)/lib/small/asxxxx/gb/*.asm
	@rm $(BUILDDIR)/lib/small/asxxxx/gb/*.lst
	@rm $(BUILDDIR)/lib/small/asxxxx/gb/*.sym
	@rm $(BUILDDIR)/lib/small/asxxxx/gbz80/*.asm
	@rm $(BUILDDIR)/lib/small/asxxxx/gbz80/*.lst
	@rm $(BUILDDIR)/lib/small/asxxxx/gbz80/*.sym
	@cp $(GBDKLIBDIR)/libc/gb/global.s $(BUILDDIR)/lib/small/asxxxx/global.s
	@echo Generating make.bat
	@$(MAKE) -C $(BUILDDIR)/examples/gb make.bat --no-print-directory
	@echo

gbdk-lib-clean:
	@echo Cleaning lib
	@$(MAKE) -C $(GBDKLIBDIR) clean
	@echo

gbdk-lib-examples-makefile:
	$(MAKE) -C $(BUILDDIR)/examples/gb make.bat
	unix2dos $(BUILDDIR)/examples/gb/make.bat

# Rules for maccer
maccer-build:
	@echo Building Maccer
	@$(MAKE) -C $(MACCERDIR) BUILDDIR=$(BUILDDIR) --no-print-directory
	@echo 
	
maccer-clean:
	@echo Cleaning Maccer
	@$(MAKE) -C $(MACCERDIR) clean --no-print-directory
	@echo
	
#rules for linker
linker-build:
	@echo Building Linker
	@$(MAKE) -C $(LINKERDIR) --no-print-directory
	@echo
	
linker-install:
	@echo Installing Linker
	@$(MAKE) -C $(LINKERDIR) install BUILDDIR=$(BUILDDIR) --no-print-directory
	@echo

linker-clean:
	@echo Cleaning Linker
	@$(MAKE) -C $(LINKERDIR) clean --no-print-directory
	@echo

#SDDC copy
sdcc-install: check-SDCCDIR
	@echo Installing SDCC
	@mkdir -p $(BUILDDIR)/bin/SDCC/bin
	@cp -r $(SDCCDIR)/bin $(BUILDDIR)/bin/SDCC

# Final binary
binary: binary-tidyup
ifeq ($(ARCHIVETYPE),zip)
	rm -f $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).zip
	cd $(BUILDDIR)/..; zip -9Xrq $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).zip gbdk
else
	rm -f $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).tar.gz
	cd $(BUILDDIR)/..; tar czf $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).tar.gz gbdk
endif

binary-tidyup:
	rm -rf `find $(BUILDDIR) -name CVS`

# Install
install: native-build
	mkdir -p $(TARGETDIR)
	cp -r $(BUILDDIR)/* $(TARGETDIR)
	
check-SDCCDIR:
ifndef SDCCDIR
	$(error SDCCDIR is undefined)
endif
