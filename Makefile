
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
	BIN_BASE = bin
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

OUT_DIR = $(BIN_BASE)/$(MODE_DIR)
OBJ_DIR = $(OBJ_BASE)/$(MODE_DIR)
SHARED_DIR = bin/$(MODE_DIR)

TARGET = $(OUT_DIR)/$(TARGET_NAME)$(EXT)
SHARED_TARGET = $(SHARED_DIR)/$(TARGET_NAME)$(EXT)
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRC))
DEPS = $(OBJS:.o=.d)

RAYLIB_LIB = $(RAYLIB_DIR)/libraylib.a

.PHONY: all debug release asan clean clean-all
all: debug

debug:
	@$(MAKE) build MODE=debug

release:
	@$(MAKE) build MODE=release

asan:
	@$(MAKE) build MODE=asan

build: $(RAYLIB_LIB) $(TARGET)

# Do not pass CFLAGS here or Raylib's platform defines get overwritten
$(RAYLIB_LIB):
	$(MAKE) -C $(RAYLIB_DIR) PLATFORM=PLATFORM_DESKTOP

$(TARGET): $(OBJS)
	@$(call MKDIR,$(dir $@))
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
ifeq ($(OS),Windows_NT)
	@$(call MKDIR,$(SHARED_DIR))
	@$(call CP_FILE,$@,$(SHARED_TARGET))
endif

# Compile source files and generate header dependencies
$(OBJ_DIR)/%.o: %.c
	@$(call MKDIR,$(dir $@))
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include automatically generated header dependencies
-include $(DEPS)

clean:
	@$(call RM_DIR,bin)
	@$(call RM_DIR,obj)

clean-all: clean
	$(MAKE) -C $(RAYLIB_DIR) clean

