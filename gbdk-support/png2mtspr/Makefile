# *****************************************************
# Variables to control Makefile operation
 
CXX = g++
CXXFLAGS = -Wall -g
LFLAGS = -g -static -std=c++11
 
# ****************************************************
# Targets needed to bring the executable up to date
 
TARGET = png2mtspr

SRCS := picopng.cpp png2mtspr.cpp
OBJS := $(SRCS:%.cpp=%.o)
 
$(TARGET): $(OBJS)
	$(CXX) $(LFLAGS) -o $(TARGET) $(OBJS)
 
# The main.o target can be written more simply
 
%.cpp.o: %.cpp
	@echo compiling $<
	$(CXX) $(CXXFLAGS) -c $< -o $@
 
 clean:
	@echo cleaning $(TARGET)
	rm -f *.o
	rm -f *.exe
	rm -f TARGET