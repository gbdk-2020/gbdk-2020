# *****************************************************
# Variables to control Makefile operation

# Might want when doing linux -> win cross build
# LFLAGS = -s -static

CXX = $(TOOLSPREFIX)g++
CXXFLAGS = -Os -Wall -g
LFLAGS = -g

ifeq ($(OS),Windows_NT)
	EXT = .exe
	LFLAGS += -static 
endif
 
# ****************************************************
# Targets needed to bring the executable up to date
 
TARGET = png2mtspr


SRCS := picopng.cpp png2mtspr.cpp
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
