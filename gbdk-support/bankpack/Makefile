# bankcheck (auto bank tool) makefile

ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

# Target older macOS version than whatever build OS is for better compatibility
ifeq ($(UNAME_S),Darwin)
	export MACOSX_DEPLOYMENT_TARGET=10.9 # 2013 vintage is plenty old
endif

CC = $(TOOLSPREFIX)gcc
CFLAGS = -ggdb -O -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
OBJ = bankpack.o files.o obj_data.o list.o path_ops.o
BIN = bankpack

all: $(BIN)

$(BIN): $(OBJ)

clean:
	rm -f *.o $(BIN) *~
	rm -f *.exe

