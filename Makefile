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

# Directory with docs config and output (via doxygen)
GBDKDOCSDIR = $(TOPDIR)/docs

# Doxygen command and version check info
DOXYGENCMD = doxygen
DOXYGEN_VER_REQ = 1.8.17
DOXYGEN_VER_HAS = $(shell doxygen -v)


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

clean: gbdk-support-clean gbdk-lib-clean

distclean: clean build-dir-clean

docs: doxygen-generate

docsclean: doxygen-clean

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
gbdk-build: gbdk-support-build gbdk-lib-build

gbdk-install: $(BUILDDIR)/bin gbdk-support-install gbdk-lib-install sdcc-install

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
	mkdir -p $(BUILDDIR)/lib/small/asxxxx/gb/
	mkdir -p $(BUILDDIR)/lib/small/asxxxx/gbz80/
	@cp $(GBDKLIBDIR)/build/small/asxxxx/gb/crt0.o $(BUILDDIR)/lib/small/asxxxx/gb/crt0.o
	@cp $(GBDKLIBDIR)/build/small/asxxxx/gb/gb.lib $(BUILDDIR)/lib/small/asxxxx/gb/gb.lib
	@cp $(GBDKLIBDIR)/build/small/asxxxx/gbz80/gbz80.lib $(BUILDDIR)/lib/small/asxxxx/gbz80/gbz80.lib
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

#SDDC copy
sdcc-install: check-SDCCDIR
	@echo Installing SDCC
ifeq ($(OS),Windows_NT)
		@cp -r $(SDCCDIR)/bin $(BUILDDIR)
else
#		@cp $(SDCCDIR)/bin/{as2gbmap,makebin,packihx,sdar,sdasgb,sdcc,sdcdb,sdcdb{,src}.el,sdcpp,sdldgb,sdnm,sdobjcopy,sdranlib,sz80} $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/as2gbmap $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/makebin $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/packihx $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdar $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdasgb $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdcc $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdcdb $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdcpp $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdldgb $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdnm $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdobjcopy $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sdranlib $(BUILDDIR)/bin/
		@cp $(SDCCDIR)/bin/sz80 $(BUILDDIR)/bin/
endif

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

# First purge doxygen output directory to clear potentially stale output.
# Next change working dir so "include" is the root doxygen works in.
# That prevents it from including the path leading up to there in the
# output, even though it's instructed to only process starting at "include".
doxygen-generate:
ifeq ($(shell expr "$(DOXYGEN_VER_HAS)" \< "$(DOXYGEN_VER_REQ)"), 1)
	$(error Doxygen version $(DOXYGEN_VER_HAS) is too old! Minimum version is $(DOXYGEN_VER_REQ))
endif
	rm -rf $(GBDKDOCSDIR)/api; \
	  cd "$(GBDKLIBDIR)/include"; \
	  GBDKDOCSDIR="$(GBDKDOCSDIR)" GBDKLIBDIR="$(GBDKLIBDIR)" $(DOXYGENCMD) "$(GBDKDOCSDIR)/config/gbdk-2020-doxyfile"

doxygen-clean:
	rm -rf $(GBDKDOCSDIR)/api