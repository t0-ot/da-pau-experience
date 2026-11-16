CC = gcc
TARGET_NAME = Da-Pau-Launching-Experience
RAYLIB_DIR = external/raylib/src

# Recursively find all .c files in src/
SRC := $(wildcard src/*.c src/*/*.c src/*/*/*.c)

# Common flags
BASE_CFLAGS = -Wall -Wextra -Iinclude -I$(RAYLIB_DIR)

# OS-specific configuration
ifeq ($(OS),Windows_NT)
	EXT = .exe
	BIN_BASE = bin/Windows
	OBJ_BASE = obj/Windows
	# -mwindows hides the console window in Release (matches use_console_runner="0")
	WIN_GUI_FLAG = $(if $(filter release,$(MODE)),-mwindows,)
	LDFLAGS = -L$(RAYLIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc $(WIN_GUI_FLAG)
else
	EXT =
	BIN_BASE = bin
	OBJ_BASE = obj
	LDFLAGS = -L$(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -static-libgcc
endif

MKDIR = mkdir -p $(1)
RM_DIR = rm -rf $(1)


# Build Mode Configuration (debug | release | asan)
MODE ?= debug

ifeq ($(MODE),release)
	CFLAGS = $(BASE_CFLAGS) -O2
	LDFLAGS += -s
	OUT_DIR = $(BIN_BASE)/Release
	OBJ_DIR = $(OBJ_BASE)/Release
else ifeq ($(MODE),asan)
	CFLAGS = $(BASE_CFLAGS) -g -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract
	LDFLAGS += -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract
	OUT_DIR = $(BIN_BASE)/Debug
	OBJ_DIR = $(OBJ_BASE)/Debug-Asan
else
	CFLAGS = $(BASE_CFLAGS) -g
	OUT_DIR = $(BIN_BASE)/Debug
	OBJ_DIR = $(OBJ_BASE)/Debug
endif

TARGET = $(OUT_DIR)/$(TARGET_NAME)$(EXT)
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

# Default target
.PHONY: all debug release asan clean clean-all

all: debug

debug:
	@$(MAKE) build MODE=debug

release:
	@$(MAKE) build MODE=release

asan:
	@$(MAKE) build MODE=asan

build: $(RAYLIB_LIB) $(TARGET)

# Compile Raylib submodule if libraylib.a doesn't exist yet
$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP

# Link final executable
$(TARGET): $(OBJS)
	@$(call MKDIR,$(dir $@))
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Built successfully: $@"

# Compile .c files into .o files
$(OBJ_DIR)/%.o: %.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -c $< -o $@

# Clean game build artifacts (keeps Raylib compiled for fast rebuilds)
clean:
	@$(call RM_DIR,bin)
	@$(call RM_DIR,obj)

# Deep clean (also cleans Raylib)
clean-all: clean
	$(MAKE) -C $(RAYLIB_DIR) clean

