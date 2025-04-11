#Compiler Flags
CC = g++
CFLAGS = -fdiagnostics-color=always -g -Wall -Wextra -std=c++11 -O2
CLIBS = -lm

#Folders
SRC_DIR = Source
OBJ_DIR = Object
DEP_DIR = dependencies

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
	@echo "SDL3 - Build SDL3"
	@echo "SDL3_Image - Build SDL3_Image"
	@echo "SDL3_TTF - Build SDL3_TTF"
	@echo "SDL3_print - Print SDL3 installation path"
	@echo "SDL3_update - Update SDL3 libraries"
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
SDL3:
	@echo "Building the SDL3 Basic Library"
	make -C dependencies/SDL3 build
SDL3_Image:
	@echo "Building the SDL3 Image Library"
	make -C dependencies/SDL3_Image build
SDL3_TTF:
	@echo "Building the SDL3 TTF Library"
	make -C dependencies/SDL3_TTF build
SDL3_update:
	@echo "UPDATING SDL3 LIBS"
	@sudo ldconfig
	@ldconfig -p | grep SDL3