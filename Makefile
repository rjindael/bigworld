# SPDX-License-Identifier: GPL-2.0-or-later
CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
LDFLAGS = -lSDL3 -lm

# Extra checks for safety during debugging at the expense of performance
CFLAGS += -DSTRICT_CHECKS=1

# Auto-generate per-.c header dependencies (as .d files, included below) so
# that e.g. editing a struct in engine.h correctly triggers a rebuild of every
# .c that includes it, not just engine.c itself -- plain header-less rules
# silently leave stale .o files linked against an old struct layout, which is
# a real (and only sometimes obvious) heap-corrupting ODR mismatch, not just
# a slow/unnecessary-rebuild inconvenience.
CFLAGS += -MMD -MP

SRC_DIR = src
DEPS_DIR = deps
BUILD_DIR = build

WGPU_DIR = $(DEPS_DIR)/wgpu-native
WGPU_LIB = $(WGPU_DIR)/lib/libwgpu_native.so

INCLUDES = -I$(SRC_DIR) \
           -I$(WGPU_DIR)/include \
           -I$(DEPS_DIR)/mathc/inc \
           -I$(DEPS_DIR)/microui/inc

LDFLAGS += -L$(WGPU_DIR)/lib -lwgpu_native -Wl,-rpath,$(abspath $(WGPU_DIR)/lib) -lpthread -ldl

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
          $(DEPS_DIR)/mathc/mathc.c \
          $(DEPS_DIR)/microui/microui.c

OBJECTS = $(SOURCES:.c=.o)
DEPFILES = $(SOURCES:.c=.d)
TARGET = shambhala

.PHONY: all
all: $(TARGET)

# Fetch the prebuilt wgpu-native library + headers for the host platform (see deps/wgpu-native/fetch.sh)
$(WGPU_LIB):
	$(DEPS_DIR)/wgpu-native/fetch.sh

# Link
$(TARGET): $(WGPU_LIB) $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compile
%.o: %.c $(WGPU_LIB)
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

-include $(DEPFILES)

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(DEPFILES) $(TARGET)

.PHONY: distclean
distclean: clean
	rm -rf $(WGPU_DIR)/include $(WGPU_DIR)/lib

.PHONY: run
run: $(TARGET)
	./$(TARGET)
