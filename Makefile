# compiler settings
CC = gcc
CXX = g++
CFLAGS = -Wall -Wextra -O2
CXXFLAGS = -Wall -Wextra -O2
LDFLAGS =

# source and object files
SRC = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=build/%.o)

# target executable
TARGET = firesh

# build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

debug: CFLAGS += -g
debug: all

install: all
	sudo cp $(TARGET) /usr/local/bin/

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build $(TARGET)

.PHONY: all debug install clean
