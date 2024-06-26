# *****************************************************
# Variables to control Makefile operation

# Might want when doing linux -> win cross build
# LFLAGS = -s -static

CXX = $(TOOLSPREFIX)g++
CXXFLAGS = -Os -Wall -g # -Wextra -pedantic
LFLAGS = -g

ifeq ($(OS),Windows_NT)
	BUILD_OS := Windows_NT
else
	BUILD_OS := $(shell uname -s)
endif

# Target older macOS version than whatever build OS is for better compatibility
ifeq ($(BUILD_OS),Darwin)
	export MACOSX_DEPLOYMENT_TARGET=10.10
endif


# Static flag for windows cross and native builds
ifeq ($(TOOLSPREFIX),i686-w64-mingw32-)
#   prefix will automatically be .exe for cross build
	EXT = .exe
	LFLAGS += -static 
endif

ifeq ($(OS),Windows_NT)
	EXT = .exe
	LFLAGS += -static 
endif




# ****************************************************
# Targets needed to bring the executable up to date

TARGET = png2asset


SRCS := cmp_int_color.cpp  image_data.cpp lodepng.cpp  map_attributes.cpp \
        metasprites.cpp png2asset.cpp \
        rgb_to_nes_lut.cpp \
        export.cpp export_h_file.cpp export_c_file.cpp export_binary.cpp \
        image_utils.cpp main.cpp maps.cpp palettes.cpp process_arguments.cpp tiles.cpp
OBJS := $(SRCS:%.cpp=%.o)

$(TARGET): $(OBJS)
	$(CXX) $(LFLAGS) -o $(TARGET) $(OBJS)
	strip $@$(EXT)

# The main.o target can be written more simply

%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o
	rm -f *.exe
	rm -f TARGET

test:
	$(MAKE) -C testing

test-clean:
	$(MAKE) -C testing clean

test-update:
	$(MAKE) -C testing update

test-compare:
	$(MAKE) -C testing compare-output

test-reset-ref:
	$(MAKE) -C testing reset-ref
