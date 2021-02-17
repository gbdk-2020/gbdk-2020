# Simple Makefile for the lcc frontend.

ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

BUILDDATE=$(shell date --utc +%Y/%m/%d)
BUILDTIME=$(shell date --utc +%H:%M:%S)

CC = $(TOOLSPREFIX)gcc
CFLAGS = -ggdb -O -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
CFLAGS += -DBUILDDATE=\"$(BUILDDATE)\" -DBUILDTIME=\"$(BUILDTIME)\"
ifdef BINDIR
CFLAGS += -DGBDKBINDIR=\"$(BINDIR)\"
endif
OBJ = lcc.o gb.o
BIN = lcc

all: $(BIN)

$(BIN): $(OBJ)

clean:
	rm -f *.o $(BIN) *~
