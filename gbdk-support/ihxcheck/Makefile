# ihxcheck makefile

ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

CC = $(TOOLSPREFIX)gcc
CFLAGS = -ggdb -O -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
OBJ = ihxcheck.o areas.o ihx_file.o
BIN = ihxcheck

all: $(BIN)

$(BIN): $(OBJ)

clean:
	rm -f *.o $(BIN) *~
	rm -f *.exe

