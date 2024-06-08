ifndef TARGETDIR
TARGETDIR = /opt/gbdk
endif

ifeq ($(OS),Windows_NT)
	BUILD_OS := Windows_NT
  LDFLAGS = -s -static
else
	BUILD_OS := $(shell uname -s)
  LDFLAGS = -s
endif

# Target older macOS version than whatever build OS is for better compatibility
ifeq ($(BUILD_OS),Darwin)
	export MACOSX_DEPLOYMENT_TARGET=10.10
endif

CC = $(TOOLSPREFIX)gcc
CFLAGS = -O -Wno-incompatible-pointer-types -DGBDKLIBDIR=\"$(TARGETDIR)\"
CFLAGS += -Isrc
OBJ = src/banks.c \
src/banks_print.c \
src/bank_templates.c \
src/common.c \
src/list.c \
src/map_file.c \
src/rom_file.c \
src/banks_color.c \
src/banks_summarized.c \
src/cdb_file.c \
src/ihx_file.c \
src/logging.c \
src/noi_file.c \
src/romusage.c


BIN = romusage

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN)  $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o $(BIN) *~ src/*.o
	rm -f *.exe

