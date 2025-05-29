# Makefile for the os-sim project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g

# Directories
SRC_DIR = .
DS_DIR = DS
OBJ_DIR = obj
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/os-sim

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
DS_SRCS = $(wildcard $(DS_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))
DS_OBJS = $(patsubst $(DS_DIR)/%.c, $(OBJ_DIR)/DS_%.o, $(DS_SRCS))

# Default target
all: $(TARGET)

# Build the target executable
$(TARGET): $(OBJS) $(DS_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Build object files from main directory
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -lm

# Build object files from DS directory
$(OBJ_DIR)/DS_%.o: $(DS_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -lm

# Clean up build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)