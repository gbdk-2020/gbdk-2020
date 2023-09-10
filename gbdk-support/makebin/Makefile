# makebin makefile

ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

ifeq ($(OS),Windows_NT)
	BUILD_OS := Windows_NT
else
	BUILD_OS := $(shell uname -s)
endif

# Target older macOS version than whatever build OS is for better compatibility
ifeq ($(BUILD_OS),Darwin)
	export MACOSX_DEPLOYMENT_TARGET=10.10
endif

CC = $(TOOLSPREFIX)gcc
CFLAGS = -g3 -O0 -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
LDFLAGS = -g3
OBJ = makebin.o
BIN = makebin

all: $(BIN)

$(BIN): $(OBJ)

clean:
	rm -f *.o $(BIN) *~
	rm -f *.exe

