CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Iinclude
LDFLAGS  = -lncurses

SRCS = main.cpp \
       src/chicken.cpp \
       src/vehicle.cpp \
       src/lane.cpp    \
       src/game.cpp

TARGET = ga_qua_duong

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
