# Top level Makefile for GBDK that combines the builds for sdcc,
# gbdk-lib, and gbdk-support
#
# 2001  Michael Hope <michaelh@juju.net.nz>
# $Id: Makefile,v 1.4 2001/11/04 18:43:51 michaelh Exp $
#
TOPDIR = $(shell pwd)

# Package name, used for tarballs
PKG = gbdk
# Version, used for tarballs
VER = 3.00

# Prefix to add to the standard tools.  Usefull for a standard gcc
# cross-compile.
TOOLSPREFIX =

TARGETCC = $(TOOLSPREFIX)gcc
TARGETRANLIB = $(TOOLSPREFIX)ranlib
TARGETAR = $(TOOLSPREFIX)ar
TARGETSTRIP = $(TOOLSPREFIX)strip

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
# Host operating system identifier.
HOSTOS = $(shell uname -s)
# Target operating system identifier.  Used in the output zip name.
TARGETOS ?= $(HOSTOS)

# Directory that gbdk should finally end up in
TARGETDIR = /opt/gbdk
# Directory that gbdk for this target is built into.
BUILDDIR = $(TOPDIR)/build/gbdk

NOISELOG = $(TOPDIR)/noise.log

all: native-build

clean: gbdk-support-clean gbdk-lib-clean

distclean: clean build-dir-clean

docs: doxygen-generate
docspdf: doxygen-generate-with-pdf
docsclean: doxygen-clean
docsreset: doxygen-reset
docstools: docs-toolchain-generate

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

# Rules for gbdk-support
gbdk-support-build:
	@echo Building lcc
ifdef BINDIR
	@$(MAKE) -C $(GBDKSUPPORTDIR)/lcc TOOLSPREFIX=$(TOOLSPREFIX) TARGETDIR=$(TARGETDIR)/ BINDIR=$(BINDIR)/ --no-print-directory
else
	@$(MAKE) -C $(GBDKSUPPORTDIR)/lcc TOOLSPREFIX=$(TOOLSPREFIX) TARGETDIR=$(TARGETDIR)/ --no-print-directory
endif
	@echo
	@echo Building ihxcheck
	@$(MAKE) -C $(GBDKSUPPORTDIR)/ihxcheck TOOLSPREFIX=$(TOOLSPREFIX) TARGETDIR=$(TARGETDIR)/ --no-print-directory
	@echo Building bankpack
	@$(MAKE) -C $(GBDKSUPPORTDIR)/bankpack TOOLSPREFIX=$(TOOLSPREFIX) TARGETDIR=$(TARGETDIR)/ --no-print-directory
	@echo Building png2mtspr
	@$(MAKE) -C $(GBDKSUPPORTDIR)/png2mtspr
	@echo Building gbcompress
	@$(MAKE) -C $(GBDKSUPPORTDIR)/gbcompress TOOLSPREFIX=$(TOOLSPREFIX) TARGETDIR=$(TARGETDIR)/ --no-print-directory
	@echo

gbdk-support-install: gbdk-support-build $(BUILDDIR)/bin
	@echo Installing lcc
	@cp $(GBDKSUPPORTDIR)/lcc/lcc $(BUILDDIR)/bin/lcc$(EXEEXTENSION)
	@$(TARGETSTRIP) $(BUILDDIR)/bin/lcc*
	@cp $(GBDKSUPPORTDIR)/ChangeLog $(BUILDDIR)
	@cp $(GBDKSUPPORTDIR)/README $(BUILDDIR)
	@cp $(GBDKDOCSDIR)/gbdk_manual.pdf $(BUILDDIR)
	@echo
	@echo Installing ihxcheck
	@cp $(GBDKSUPPORTDIR)/ihxcheck/ihxcheck $(BUILDDIR)/bin/ihxcheck$(EXEEXTENSION)
	@$(TARGETSTRIP) $(BUILDDIR)/bin/ihxcheck*
	@echo Installing bankpack
	@cp $(GBDKSUPPORTDIR)/bankpack/bankpack $(BUILDDIR)/bin/bankpack$(EXEEXTENSION)
	@$(TARGETSTRIP) $(BUILDDIR)/bin/bankpack*
	@echo Installing png2mtspr
	@cp $(GBDKSUPPORTDIR)/png2mtspr/png2mtspr$(EXEEXTENSION) $(BUILDDIR)/bin/png2mtspr$(EXEEXTENSION)
	@$(TARGETSTRIP) $(BUILDDIR)/bin/bankpack*
	@echo Installing gbcompress
	@cp $(GBDKSUPPORTDIR)/gbcompress/gbcompress $(BUILDDIR)/bin/gbcompress$(EXEEXTENSION)
	@$(TARGETSTRIP) $(BUILDDIR)/bin/gbcompress*
	@echo

gbdk-support-clean:
	@echo Cleaning lcc
	@$(MAKE) -C $(GBDKSUPPORTDIR)/lcc clean --no-print-directory
	@echo
	@echo Cleaning ihxcheck
	@$(MAKE) -C $(GBDKSUPPORTDIR)/ihxcheck clean --no-print-directory
	@echo Cleaning bankpack
	@$(MAKE) -C $(GBDKSUPPORTDIR)/bankpack clean --no-print-directory
	@echo Cleaning png2mtspr
	@$(MAKE) -C $(GBDKSUPPORTDIR)/png2mtspr clean
	@echo Cleaning gbcompress
	@$(MAKE) -C $(GBDKSUPPORTDIR)/gbcompress clean --no-print-directory
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

# Copy SDDC executable files
SDCC_BINS = makebin packihx sdar sdasgb sdcc sdcdb sdcpp sdldgb sdnm sdobjcopy sdranlib sz80
ifeq ($(OS),Windows_NT)
MINGW64_RUNTIME = \
	libgcc_s_seh-1.dll \
	libgcc_s_sjlj-1.dll \
	libstdc++-6.dll \
	libwinpthread-1.dll \
	readline5.dll
SDCC_BINS := $(addsuffix .exe, $(SDCC_BINS)) $(MINGW64_RUNTIME)
endif

sdcc-install: check-SDCCDIR
	@echo Installing SDCC
	@for i in $(SDCC_BINS); do \
	cp $(SDCCDIR)/bin/$$i $(BUILDDIR)/bin/ && echo "-> $$i" ; \
	done

# Final binary
binary:
ifeq ($(ARCHIVETYPE),zip)
	rm -f $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).zip
	cd $(BUILDDIR)/..; zip -9Xrq $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).zip gbdk
else
	rm -f $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).tar.gz
	cd $(BUILDDIR)/..; tar czf $(TOPDIR)/gbdk-$(VER)-$(TARGETOS).tar.gz gbdk
endif

# Install
install: native-build
	mkdir -p $(TARGETDIR)
	cp -r $(BUILDDIR)/* $(TARGETDIR)

# Make sure SDCCDIR is populated
# Swap WIN/MSDOS slashes to Unix style for MinGW (prevent some errors with cp)
check-SDCCDIR:
ifndef SDCCDIR
	$(error SDCCDIR is undefined)
endif
ifeq ($(OS),Windows_NT)
SDCCDIR := $(subst \,/,$(SDCCDIR))
endif

# First purge doxygen output directory to clear potentially stale output.
# Next change working dir so "include" is the root doxygen works in.
# That prevents it from including the path leading up to there in the
# output, even though it's instructed to only process starting at "include".
doxygen-generate:
ifeq ($(shell expr "$(DOXYGEN_VER_HAS)" \< "$(DOXYGEN_VER_REQ)"), 1)
	$(error Doxygen version $(DOXYGEN_VER_HAS) is too old! Minimum version is $(DOXYGEN_VER_REQ))
endif
#Run Doxygen	
	rm -rf $(GBDKDOCSDIR)/api; \
	  cd "$(GBDKLIBDIR)/include"; \
	  GBDKDOCSDIR="$(GBDKDOCSDIR)" GBDKLIBDIR="$(GBDKLIBDIR)" $(DOXYGENCMD) "$(GBDKDOCSDIR)/config/gbdk-2020-doxyfile"
	@if [ "$(DOCS_PDF_ON)" = "YES" ]; then\
		$(MAKE) -C $(GBDKDOCSDIR)/latex;\
		cp $(GBDKDOCSDIR)/latex/refman.pdf $(GBDKDOCSDIR)/gbdk_manual.pdf;\
	fi
	rm -rf $(GBDKDOCSDIR)/latex

# Turn on Latex -> PDF conversion to run run at end of regular docs build
# (which includes latex output but deletes it at the end).
#
# The conversion process requires a Latex install. 
# For Windows there are various Latex packages to choose from.
# For Linux this appears to be the minimum:
#   sudo apt install texlive-latex-base
#   sudo apt install texlive-latex-recommended
#   sudo apt install texlive-latex-extra
#
doxygen-generate-with-pdf:	DOCS_PDF_ON=YES
doxygen-generate-with-pdf:	doxygen-generate


# Generate toolchain settings markdown file (if possible)
# Then run main doxygen build
docs-toolchain-generate:	TOOLCHAIN_DOCS_FILE=$(GBDKDOCSDIR)/pages/20_toolchain_settings.md
docs-toolchain-generate:
ifneq (,$(wildcard $(BUILDDIR)/bin/))
	echo @page docs_toolchain_settings Toolchain settings > $(TOOLCHAIN_DOCS_FILE)
	echo \\n\\n@anchor lcc-settings\\n# lcc settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE); \
	  cd "$(BUILDDIR)/bin/"; \
	  ./lcc >> $(TOOLCHAIN_DOCS_FILE) 2>&1
	echo \`\`\`\\n\\n@anchor sdcc-settings\\n# sdcc settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/sdcc -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1
	echo \`\`\`\\n\\n@anchor sdasgb-settings\\n# sdasgb settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/sdasgb -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1 || true 
	echo \`\`\`\\n\@anchor bankpack-settings\\n# bankpack settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/bankpack -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1 || true 
	echo \`\`\`\\n\@anchor sdldgb-settings\\n# sdldgb settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/sdldgb -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1 || true 
	echo \`\`\`\\n\@anchor ihxcheck-settings\\n# ihxcheck settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/ihxcheck -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1 || true 
	echo \`\`\`\\n\@anchor makebin-settings\\n# makebin settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/makebin -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1
	echo \`\`\`\\n\@anchor gbcompress-settings\\n# gbcompress settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/gbcompress -h >> $(TOOLCHAIN_DOCS_FILE) 2>&1 || true 
	echo \`\`\`\\n\@anchor png2mtspr-settings\\n# png2mtspr settings\\n\`\`\` >> $(TOOLCHAIN_DOCS_FILE)
	$(BUILDDIR)/bin/png2mtspr >> $(TOOLCHAIN_DOCS_FILE) 2>&1
	echo \`\`\`\\n\\n >> $(TOOLCHAIN_DOCS_FILE)
endif

doxygen-clean:
	rm -rf $(GBDKDOCSDIR)/api

doxygen-reset:
	rm -rf $(GBDKDOCSDIR)/api
	git checkout $(GBDKDOCSDIR)/api

