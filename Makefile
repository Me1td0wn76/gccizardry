# Makefile for DRPG

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
LDFLAGS = -lgdi32 -luser32
TARGET = gccizardry.exe
SRC = src/main.cpp src/battle.cpp src/enemy.cpp src/character.cpp src/ground.cpp src/map.cpp src/map.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
