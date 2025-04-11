#Compiler Flags
CC = g++
CFLAGS = -fdiagnostics-color=always -g -Wall -Wextra -std=c++11 -O2
CLIBS = -lm -lSDL3 -lSDL3_ttf -lSDL3_image

#Folders
SRC_DIR = Source
OBJ_DIR = Object
DEP_DIR = dependencies
INSTALL_DIR = /usr/local

#Source Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OUTPUT = GkotzamBoy

#Bulder Scripts
help:
	@echo "Using make [target]"
	@echo "target:"
	@echo "clean - Clean up the build"
	@echo "folders - Create folders for the build"
	@echo "build - Build the project"
	@echo "run - Run the project"
	@echo "download - Download SDL3, SDL_Image, SDL_TTF from github"
	@echo "install - install SDL3, SDL_Image, SDL_TTF"
	@echo "unistall - unistall SDL3, SDL_Image, SDL_TTF"
clean:
	@echo "Cleaning up..."
	@rm -rfv $(OBJ_DIR)
folders:
	@echo "Creating folders..."
	@mkdir -p $(OBJ_DIR)
build: folders
	@echo "Building Compiling and Linking..."
	@$(CC) $(CFLAGS) $(SRC_FILES) $(CLIBS) -o $(OBJ_DIR)/$(OUTPUT).o
run:
	@echo "Running..."
	@$(OBJ_DIR)/$(OUTPUT).o
	@echo "Execution complete."

#SDL3 Installer
download:
	@echo "Checking for SDL3 dependencies inside the project folder"
	@if [ -d $(DEP_DIR) ]; then \
		echo "SDL3 dependencies folder already exists"; \
	else \
		echo "Creating SDL3 dependencies folder"; \
		mkdir -p $(DEP_DIR); \
		cd $(DEP_DIR) && git clone https://github.com/libsdl-org/SDL.git && git clone https://github.com/libsdl-org/SDL_image.git && git clone https://github.com/libsdl-org/SDL_ttf.git; \
	fi 
install: download
	@echo "Testing Building Installing SDL3"
	@cd $(DEP_DIR)/SDL && cmake -S . -B build
	@cd $(DEP_DIR)/SDL && cmake --build build
	@cd $(DEP_DIR)/SDL && sudo cmake --install build --prefix $(INSTALL_DIR)

	@echo "Testing Building Installing SDL3_Image"
	@cd $(DEP_DIR)/SDL_image && cmake -S . -B build
	@cd $(DEP_DIR)/SDL_image && cmake --build build
	@cd $(DEP_DIR)/SDL_image && sudo cmake --install build --prefix $(INSTALL_DIR)

	@echo "Testing Building Installing SDL3_TTF"
	@cd $(DEP_DIR)/SDL_ttf && cmake -S . -B build
	@cd $(DEP_DIR)/SDL_ttf && cmake --build build
	@cd $(DEP_DIR)/SDL_ttf && sudo cmake --install build --prefix $(INSTALL_DIR)

	@echo "Reload libraries"
	@sudo ldconfig
	@ldconfig -p | grep SDL3
unistall:
	@echo "Removing SDL3, SDL_Image, SDL_TTF from System"
	@echo "Remove Include:"
	@sudo rm -rfv $(INSTALL_DIR)/include/SDL3*
	@echo "Remove libraries"
	@sudo rm $(INSTALL_DIR)/lib/libSDL3*
	@echo "Remove CMAKE folders"
	@sudo rm -rfv $(INSTALL_DIR)/lib/cmake/SDL3*
	@rm -rfv $(DEP_DIR)
	@sudo ldconfig
	@echo "Removing complete"