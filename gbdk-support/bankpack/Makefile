# bankcheck (auto bank tool) makefile

ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

CC = $(TOOLSPREFIX)gcc
CFLAGS = -ggdb -O -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
OBJ = bankpack.o files.o obj_data.o list.o path_ops.o
BIN = bankpack

all: $(BIN)

$(BIN): $(OBJ)

clean:
	rm -f *.o $(BIN) *~
