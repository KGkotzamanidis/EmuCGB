# ==============================================================================
# EmuCGB - Makefile
# ==============================================================================

# --- Compiler ------------------------------------------------------------------
CC     = g++
CFLAGS = -fdiagnostics-color=always -g -Wall -Wextra -std=c++17 -O2
# -I  : tells the compiler WHERE to find headers  (e.g. <SDL3/SDL.h>)
# -L  : tells the linker WHERE to find .so files
# -l  : tells the linker WHICH library to link
# -Wl,-rpath : bakes the runtime library path into the binary so the .so
#              files are found at launch without setting LD_LIBRARY_PATH.
INCLUDES = -I$(INSTALL_DIR)/include
LDFLAGS  = -L$(INSTALL_DIR)/lib -Wl,-rpath,$(INSTALL_DIR)/lib
CLIBS    = -lSDL3 -lSDL3_ttf -lSDL3_image -lm

# --- Directories ---------------------------------------------------------------
SRC_DIR     = Source
OBJ_DIR     = Object
DEP_DIR     = dependencies
INSTALL_DIR = /usr/local

# --- Files ---------------------------------------------------------------------
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
# FIX 1: Derive object files from source files so each .cpp compiles
#        separately. This enables incremental builds — only changed files
#        are recompiled, not the whole project every time.
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))

OUTPUT = GkotzamBoy
# FIX 2: The final binary is an executable, not an object file.
#        Use no extension (Linux convention) instead of .o.
TARGET = $(OBJ_DIR)/$(OUTPUT)

# ==============================================================================
# Phony targets — tell Make these are not files on disk
# ==============================================================================
# FIX 3: Declare all non-file targets as .PHONY so Make does not confuse
#        them with actual files and always runs them when requested.
.PHONY: help clean folders build run download install uninstall

# --- Default target ------------------------------------------------------------
all: build

# ==============================================================================
# Help
# ==============================================================================
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build the project (default)"
	@echo "  build      - Compile and link the project"
	@echo "  run        - Run the compiled binary"
	@echo "  clean      - Remove all build artifacts"
	@echo "  folders    - Create the Object directory"
	@echo "  download   - Clone SDL3, SDL_image, SDL_ttf from GitHub"
	@echo "  install    - Build and install the SDL3 libraries system-wide"
	@echo "  uninstall  - Remove the SDL3 libraries from the system"

# ==============================================================================
# Build
# ==============================================================================
folders:
	@echo "Creating build directory..."
	@mkdir -p $(OBJ_DIR)

# FIX 4: Compile each .cpp to its own .o file.
#        The automatic variables $< (source) and $@ (target) are used so
#        the rule works for every source file without repetition.
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | folders
	@echo "  CC  $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# FIX 5: Link step is now separate from compilation.
#        LDFLAGS must come before CLIBS — the linker resolves left-to-right.
$(TARGET): $(OBJ_FILES)
	@echo "  LD  $(TARGET)"
	@$(CC) $(CFLAGS) $(OBJ_FILES) $(LDFLAGS) $(CLIBS) -o $(TARGET)

build: $(TARGET)
	@echo "Build complete -> $(TARGET)"

# ==============================================================================
# Run
# ==============================================================================
# FIX 6: run depends on build so the binary is always up-to-date before
#        execution. Previously you could run a stale binary silently.
run: build
	@echo "Running $(TARGET)..."
	@$(TARGET)
	@echo "Execution complete."

# ==============================================================================
# Clean
# ==============================================================================
clean:
	@echo "Cleaning build artifacts..."
	@rm -rfv $(OBJ_DIR)

# ==============================================================================
# SDL3 Dependency Management
# ==============================================================================
download:
	@echo "Checking for SDL3 dependencies..."
	@if [ -d "$(DEP_DIR)" ]; then \
		echo "  Dependencies folder already exists — skipping clone."; \
	else \
		echo "  Cloning SDL3 libraries into $(DEP_DIR)/"; \
		mkdir -p $(DEP_DIR); \
		git clone https://github.com/libsdl-org/SDL.git       $(DEP_DIR)/SDL; \
		git clone https://github.com/libsdl-org/SDL_image.git $(DEP_DIR)/SDL_image; \
		git clone https://github.com/libsdl-org/SDL_ttf.git   $(DEP_DIR)/SDL_ttf; \
	fi

# FIX 7: Each cmake configure/build/install step is written as a single
#        shell command per library using &&, which means:
#        - All steps run in one shell (no need for cd tricks)
#        - If any step fails the recipe aborts immediately
#        Previously each @cd ... line ran in its own subshell so the cd
#        had no effect on the next line — the build commands were running
#        from the wrong directory.
install: download
	@echo "--- Building and installing SDL3 ---"
	cmake -S $(DEP_DIR)/SDL       -B $(DEP_DIR)/SDL/build       && \
	cmake --build $(DEP_DIR)/SDL/build       --parallel          && \
	sudo cmake --install $(DEP_DIR)/SDL/build --prefix $(INSTALL_DIR)

	@echo "--- Building and installing SDL3_image ---"
	cmake -S $(DEP_DIR)/SDL_image -B $(DEP_DIR)/SDL_image/build  && \
	cmake --build $(DEP_DIR)/SDL_image/build --parallel           && \
	sudo cmake --install $(DEP_DIR)/SDL_image/build --prefix $(INSTALL_DIR)

	@echo "--- Building and installing SDL3_ttf ---"
	cmake -S $(DEP_DIR)/SDL_ttf   -B $(DEP_DIR)/SDL_ttf/build    && \
	cmake --build $(DEP_DIR)/SDL_ttf/build   --parallel           && \
	sudo cmake --install $(DEP_DIR)/SDL_ttf/build --prefix $(INSTALL_DIR)

	@echo "--- Reloading linker cache ---"
	@sudo ldconfig
	@ldconfig -p | grep SDL3

# FIX 8: Renamed 'unistall' -> 'uninstall' (was a typo).
uninstall:
	@echo "Removing SDL3, SDL3_image, SDL3_ttf from system..."
	@sudo rm -rfv $(INSTALL_DIR)/include/SDL3
	@sudo rm -rfv $(INSTALL_DIR)/include/SDL3_image
	@sudo rm -rfv $(INSTALL_DIR)/include/SDL3_ttf
	@sudo rm -f   $(INSTALL_DIR)/lib/libSDL3*
	@sudo rm -rfv $(INSTALL_DIR)/lib/cmake/SDL3*
	@sudo rm -rfv $(INSTALL_DIR)/lib/pkgconfig/sdl3*
	@rm   -rfv    $(DEP_DIR)
	@sudo ldconfig
	@echo "Uninstall complete."