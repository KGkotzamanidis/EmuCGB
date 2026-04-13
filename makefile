# ==============================================================================
# EmuCGB - Makefile (With Debugging Support)
# ==============================================================================

# --- Compiler & Base Flags ----------------------------------------------------
CC          = g++
BASE_CFLAGS = -fdiagnostics-color=always -Wall -Wextra -std=c++17
INCLUDES    = -I$(INSTALL_DIR)/include
LDFLAGS     = -L$(INSTALL_DIR)/lib -Wl,-rpath,$(INSTALL_DIR)/lib
CLIBS       = -lSDL3 -lSDL3_ttf -lSDL3_image -lm

# --- Directories ---------------------------------------------------------------
SRC_DIR     = Source
OBJ_DIR     = Object
DEP_DIR     = dependencies
INSTALL_DIR = /usr/local

# --- Files ---------------------------------------------------------------------
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
OUTPUT    = GkotzamBoy
TARGET    = $(OBJ_DIR)/$(OUTPUT)

# ==============================================================================
# Build Modes (Debug vs Release)
# ==============================================================================
# Default mode is Release
ifeq ($(DEBUG), 1)
    # -O0: No optimization (easiest to debug)
    # -g3: Maximum debug information (includes macros)
    # -DDEBUG: Define a DEBUG macro for use in code
    CFLAGS = $(BASE_CFLAGS) -O0 -g3 -DDEBUG
    MODE_MSG = "DEBUG MODE"
else
    # -O2: High optimization
    # -g:  Basic debug symbols (still useful for stack traces)
    CFLAGS = $(BASE_CFLAGS) -O2 -g
    MODE_MSG = "RELEASE MODE"
endif

# ==============================================================================
# Phony targets
# ==============================================================================
.PHONY: all help clean folders build run debug gdb download install uninstall

all: build

# ==============================================================================
# Help
# ==============================================================================
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build the project in Release mode"
	@echo "  debug      - Build the project in Debug mode (No optimizations)"
	@echo "  gdb        - Build in debug and launch GDB debugger"
	@echo "  run        - Build and run the project"
	@echo "  clean      - Remove all build artifacts"
	@echo "  install    - Build and install SDL3 dependencies"

# ==============================================================================
# Debugging Features
# ==============================================================================

# To build in debug mode, we recursively call make with the DEBUG variable set to 1
debug:
	@$(MAKE) DEBUG=1 build

# Build in debug mode and launch GDB immediately
gdb:
	@$(MAKE) DEBUG=1 build
	@echo "Starting GDB..."
	gdb ./$(TARGET)

# ==============================================================================
# Build Rules
# ==============================================================================
folders:
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | folders
	@echo "  CC [$(MODE_MSG)] $<"
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	@echo "  LD [$(MODE_MSG)] $(TARGET)"
	@$(CC) $(CFLAGS) $(OBJ_FILES) $(LDFLAGS) $(CLIBS) -o $(TARGET)

build: $(TARGET)
	@echo "Build complete -> $(TARGET) ($(MODE_MSG))"

run: build
	@echo "Running $(TARGET)..."
	@$(TARGET)

# ==============================================================================
# Cleanup
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
		echo "  Dependencies folder already exists."; \
	else \
		mkdir -p $(DEP_DIR); \
		git clone https://github.com/libsdl-org/SDL.git       $(DEP_DIR)/SDL; \
		git clone https://github.com/libsdl-org/SDL_image.git $(DEP_DIR)/SDL_image; \
		git clone https://github.com/libsdl-org/SDL_ttf.git   $(DEP_DIR)/SDL_ttf; \
	fi

install: download
	@echo "--- Building and installing SDL3 ---"
	cmake -S $(DEP_DIR)/SDL -B $(DEP_DIR)/SDL/build && \
	cmake --build $(DEP_DIR)/SDL/build --parallel && \
	sudo cmake --install $(DEP_DIR)/SDL/build --prefix $(INSTALL_DIR)

	@echo "--- Building and installing SDL3_image ---"
	cmake -S $(DEP_DIR)/SDL_image -B $(DEP_DIR)/SDL_image/build && \
	cmake --build $(DEP_DIR)/SDL_image/build --parallel && \
	sudo cmake --install $(DEP_DIR)/SDL_image/build --prefix $(INSTALL_DIR)

	@echo "--- Building and installing SDL3_ttf ---"
	cmake -S $(DEP_DIR)/SDL_ttf -B $(DEP_DIR)/SDL_ttf/build && \
	cmake --build $(DEP_DIR)/SDL_ttf/build --parallel && \
	sudo cmake --install $(DEP_DIR)/SDL_ttf/build --prefix $(INSTALL_DIR)

	@sudo ldconfig
	@echo "Installation complete."

uninstall:
	@echo "Removing SDL3 components..."
	@sudo rm -rfv $(INSTALL_DIR)/include/SDL3*
	@sudo rm -f   $(INSTALL_DIR)/lib/libSDL3*
	@sudo ldconfig
	@echo "Uninstall complete."