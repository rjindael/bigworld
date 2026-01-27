# SPDX-License-Identifier: GPL-2.0-or-later
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDFLAGS = -lGL -lSDL3 -lm

# Extra checks for safety during debugging at the expense of performance
CFLAGS += -DSTRICT_CHECKS=1

SRC_DIR = src
DEPS_DIR = deps
BUILD_DIR = build

INCLUDES = -I$(SRC_DIR) \
           -I$(DEPS_DIR)/glad/inc \
           -I$(DEPS_DIR)/mathc/inc \
           -I$(DEPS_DIR)/microui/inc

SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/shambhala.c \
          $(SRC_DIR)/io.c \
          $(SRC_DIR)/input.c \
          $(SRC_DIR)/ui.c \
          $(SRC_DIR)/world/scene.c \
          $(SRC_DIR)/world/object.c \
          $(SRC_DIR)/render/engine.c \
          $(SRC_DIR)/render/shader.c \
          $(SRC_DIR)/render/mesh.c \
          $(DEPS_DIR)/glad/gl.c \
          $(DEPS_DIR)/mathc/mathc.c \
          $(DEPS_DIR)/microui/microui.c

OBJECTS = $(SOURCES:.c=.o)
TARGET = shambhala

.PHONY: all
all: $(TARGET)

# Link
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET)
