# compiler to use
CC=gcc

# warnings
CFLAGS=-c -Wall

# executable name
TARGET = main

# source files
SRC = priority_queue.c main.c 

# .c files to .o files conversion
OBJ = $(SRC:.c=.o)

# target
all: $(TARGET)

# object and executables linking
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# cleanups
clean:
	rm -f $(OBJ) $(TARGET)

# running after compiling
run: $(TARGET)
	./$(TARGET)
