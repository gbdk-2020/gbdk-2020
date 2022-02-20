# bankcheck (auto bank tool) makefile

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

CC =
CFLAGS = 
OBJ = 
BIN = makecom

all: $(BIN)

$(BIN): makecom.dpr
	fpc -B $< 

clean:
	rm -f *.o $(BIN) *~ *.dof *.cfg
	rm -f *.exe

