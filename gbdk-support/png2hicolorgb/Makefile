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
CFLAGS += -Isrc -Isrc/lodepng -Isrc/hicolor
OBJ = src/common.o \
src/tile_dedupe.o \
src/lodepng/lodepng.o \
src/hicolor/Wu.o \
src/hicolor/median.o \
src/hicolor/hicolour.o \
src/c_source.o \
src/logging.o \
src/files.o \
src/options.o \
src/main.o \
src/path_ops.o \
src/image_load.o

BIN = png2hicolorgb

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN)  $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f *.o $(BIN) *~ src/*.o src/lodepng/*.o src/hicolor/*.o
	rm -f *.exe

