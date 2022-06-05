# Makefile for libc

.EXPORT_ALL_VARIABLES:

ifeq ($(PORTS),)
	PORTS = sm83 z80 mos6502
endif

ifeq ($(PLATFORMS),)
	PLATFORMS = gb ap duck gg sms msxdos
endif

TOPDIR = ..

CSRC =	atoi.c atol.c isalpha.c isdigit.c \
	islower.c isspace.c isupper.c \
	sprintf.c printf.c puts.c scanf.c strcat.c string.c \
	strncat.c strncmp.c strncpy.c time.c \
	tolower.c toupper.c \
	__assert.c \
	_modulong.c _modslong.c _divulong.c _divslong.c _mullong.c \
	bsearch.c qsort.c atomic_flag_clear.c \
	free.c malloc.c realloc.c calloc.c

include $(TOPDIR)/Makefile.common

all: ports platforms

clean: port-clean ports-clean platform-clean

ports:
	for i in $(PORTS); do make port THIS=$$i PORT=$$i; done

platforms:
	for j in $(PORTS); do for i in $(PLATFORMS); do if [ -d "targets/$$j/$$i" ]; then make -C targets/$$j/$$i platform THIS=$$i; fi done done

# Make all the std libs
# Make all the port specific libs

# Uses the LIB <- OBJ rule from Makefile.rules
port: port-clean $(LIB)
	make -C asm/$(PORT) port

port-clean:
	rm -f $(LIBC_OBJ) $(CLEANSPEC)

ports-clean:
	for i in $(PORTS); do make -C asm/$$i clean THIS=$$i; done

platform-clean:
	for j in $(PORTS); do for i in $(PLATFORMS); do if [ -d "targets/$$j/$$i" ]; then make -C targets/$$j/$$i clean THIS=$$i; fi done done

include Makefile.rules
