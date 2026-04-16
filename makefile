# An Gameboy and GameboyColor emulation with project name EmuCGB
# Copyright (C) <Sun Apr 06 2025>  <KGkotzamanidis>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
# ======================================================================
# 				EmuCGB - Makefile by KGkotzamanidis
# ======================================================================
# 						Compiler & Linker
CXX			= g++
BASEFLAGS	= -fdiagnostics-color=always -Wall -Wextra -std=c++17
LDFLAGS		= -L$(INSTALL_DIR)/lib -Wl,-rpath,$(INSTALL_DIR)/lib
LDLIBS		= -lSDL3 -lSDL3_ttf -lSDL3_image -lm
INCLUDES	= -I$(INSTALL_DIR)/include
# ======================================================================
# 							Directories
SRC_DIR		= Source
DEP_DIR		= Dependencies
INSTALL_DIR	= /usr/local
# ======================================================================
# 					Build Mode Login (Debug/Release)
ifeq ($(DEBUG), 1)
	OBJ_DIR	= Object/Debug
	CXXFLAGS= $(BASEFLAGS) -O0 -g3 -DDEBUG
	MODE	= "Debug Mode"
else
	OBJ_DIR	= Object/Release
	CXXFLAGS= $(BASEFLAGS) -O2 -g
	MODE	= "Release Mode"
endif
# ======================================================================
# 						Source and Object Files
SRC_FILES	= $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES	= $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
DEP_FILES	= $(OBJ_FILES:.o=.d)
OUTPUT		= GkotzamBoy
TARGET		= $(OBJ_DIR)/$(OUTPUT)
# ======================================================================
# 							Phony Targets
.PHONY: help all clean install uninstall build debug gdb run
# ======================================================================
#								All Target
all: build
# ======================================================================
# 								Help Target
help:
	@echo "Usage: make [target] [DEBUG=1]"
	@echo "Targets:"
	@echo "  all (default) - Build the project"
	@echo "  build         - Build the project"
	@echo "  debug         - Build the project in debug mode"
	@echo "  gdb           - Build and run with gdb"
	@echo "  run           - Build and run the emulator"
	@echo "  clean         - Remove build artifacts"
	@echo "  install       - Install the emulator to $(INSTALL_DIR)"
	@echo "  uninstall     - Uninstall the emulator from $(INSTALL_DIR)"
# ======================================================================
# 								Debug Target
debug:
	@$(MAKE) DEBUG=1 build
	@echo "Built in Debug Mode"
gbd:
	@$(MAKE) DEBUG=1 build
	@echo "Running with gdb..."
	@gdb ./$(TARGET)
# ======================================================================
# 								Build Target
folders:
	@mkdir -p $(OBJ_DIR)

-include $(DEP_FILES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | folders
	@echo " CC [$(MODE)] $<"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

$(TARGET): $(OBJ_FILES)
	@echo " LD [$(MODE)] $(TARGET)"
	@$(CXX) $(CXXFLAGS) $(OBJ_FILES) $(LDFLAGS) $(LDLIBS) -o $(TARGET)

build: $(TARGET)
	@echo "Build completed -> $(TARGET) ($(MODE))"
# ======================================================================
# 								Run Target
run: build
	@echo "Running $(TARGET)..."
	@./$(TARGET)
# ======================================================================
# 								Clean Target
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR)
# ======================================================================
# 								Download Target
# This target is a placeholder for downloading dependencies like SDL3.
# You can implement it using git clone or wget as needed.
# Official SDL Repositories: https://github.com/libsdl-org
download:
	@echo "Downloading dependencies..."
	@if [ -d "$(DEP_DIR)" ]; then \
		echo "Dependencies already downloaded."; \
	else \
		mkdir -p $(DEP_DIR); \
		git clone https://github.com/libsdl-org/SDL.git       $(DEP_DIR)/SDL; \
		git clone https://github.com/libsdl-org/SDL_image.git $(DEP_DIR)/SDL_image; \
		git clone https://github.com/libsdl-org/SDL_ttf.git   $(DEP_DIR)/SDL_ttf; \
	fi
# ======================================================================
# 								Install Target
# This target is a placeholder for installing the emulator. You can implement 
# it using cp or install commands as needed.
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
# ======================================================================
# 								Uninstall Target
# This target is a placeholder for uninstalling the emulator. You can implement
uninstall:
	@echo "Removing SDL3 components..."
	@sudo rm -rfv $(INSTALL_DIR)/include/SDL3*
	@sudo rm -f   $(INSTALL_DIR)/lib/libSDL3*
	@sudo ldconfig
	@echo "Uninstall complete."
# ======================================================================