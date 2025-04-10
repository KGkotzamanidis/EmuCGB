#Compiler Flags
CC = g++
CFLAGS = -fdiagnostics-color=always -g -Wall -Wextra -std=c++11 -O2
CLIBS = -lm

#Folders
SRC_DIR = Source
OBJ_DIR = Object

#Source Files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OUTPUT = GkotzamBoy

#Bulder Scripts
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