CC ?= gcc
TARGET_NAME = Da-Pau-Launching-Experience
RAYLIB_DIR = external/raylib/src

# Recursively find all .c files in src/
SRC := $(wildcard src/*.c src/*/*.c src/*/*/*.c)

# Common flags
BASE_CFLAGS = -Wall -Wextra -Iinclude -I$(RAYLIB_DIR)

# Detect MSYS2 / POSIX-like shell on Windows vs native cmd.exe
ifeq ($(OS),Windows_NT)
	EXT = .exe
	OS_DIR = Windows
	OBJ_BASE = obj/Windows
	WIN_GUI_FLAG = $(if $(filter release,$(MODE)),-mwindows,)
	LDFLAGS = -L$(RAYLIB_DIR) -lraylib -lopengl32 -lgdi32 -lwinmm -static-libgcc $(WIN_GUI_FLAG)
	ifdef MSYSTEM
		MKDIR = mkdir -p "$(1)"
		RM_DIR = rm -rf "$(1)"
		CP_FILE = cp -f "$(1)" "$(2)"
	else
		MKDIR = if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))"
		RM_DIR = if exist "$(subst /,\,$(1))" rmdir /s /q "$(subst /,\,$(1))"
		CP_FILE = copy /Y "$(subst /,\,$(1))" "$(subst /,\,$(2))" >nul
	endif
else
	EXT =
	OS_DIR = Unix
	OBJ_BASE = obj
	LDFLAGS = -L$(RAYLIB_DIR) -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -static-libgcc
	MKDIR = mkdir -p "$(1)"
	RM_DIR = rm -rf "$(1)"
	CP_FILE = cp -f "$(1)" "$(2)"
endif

# Build Mode Configuration (debug | release | asan)
MODE ?= debug
ifeq ($(MODE),release)
	CFLAGS = $(BASE_CFLAGS) -O2
	LDFLAGS += -s
	MODE_DIR = Release
else ifeq ($(MODE),asan)
	CFLAGS = $(BASE_CFLAGS) -g -O1 -fno-omit-frame-pointer -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract
	LDFLAGS += -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract
	MODE_DIR = Debug-Asan
else
	CFLAGS = $(BASE_CFLAGS) -g
	MODE_DIR = Debug
endif

# Output directories: builds in bin/<Mode>/<OS>/ and copies one level up to bin/<Mode>/
MIRROR_DIR = bin/$(MODE_DIR)
OUT_DIR = $(MIRROR_DIR)/$(OS_DIR)
OBJ_DIR = $(OBJ_BASE)/$(MODE_DIR)

TARGET = $(OUT_DIR)/$(TARGET_NAME)$(EXT)
MIRROR_TARGET = $(MIRROR_DIR)/$(TARGET_NAME)$(EXT)
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

# Default target
.PHONY: all debug release asan build raylib-build clean clean-all
all: debug

debug:
	@$(MAKE) build MODE=debug

release:
	@$(MAKE) build MODE=release

asan:
	@$(MAKE) build MODE=asan

build: raylib-build $(TARGET)

# Compile Raylib with matching sanitizer/optimization flags when needed
raylib-build:
ifeq ($(MODE),asan)
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP CFLAGS="-O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined,leak,pointer-compare,pointer-subtract" LDFLAGS="-fsanitize=address,undefined,leak,pointer-compare,pointer-subtract"
else ifeq ($(MODE),release)
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP CFLAGS="-O2"
else
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP CFLAGS="-g"
endif

# Link final executable and copy to bin/<Mode>/$(TARGET_NAME)$(EXT)
$(TARGET): $(OBJS)
	@$(call MKDIR,$(dir $@))
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@$(call MKDIR,$(MIRROR_DIR))
	@$(call CP_FILE,$@,$(MIRROR_TARGET))
	@echo "Built successfully: $@ (copied to $(MIRROR_TARGET))"

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