# gbcompress makefile

ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

CC = $(TOOLSPREFIX)gcc
CFLAGS = -ggdb -O -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
OBJ = main.o gbcompress.o files.o
BIN = gbcompress

all: $(BIN)

$(BIN): $(OBJ)

clean:
	rm -f *.o $(BIN) *~
	rm -f tmp.*
# round trip the executable through compression and de-compression as a brief test
test:
	cp $(BIN) tmp.in; ./gbcompress -v tmp.in tmp.cmp; ./gbcompress -v -d tmp.cmp tmp.dcmp; diff -s tmp.in tmp.dcmp
