# Makefile for the olang project

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -g

# Source files
SRCS = lexer.c parser.c olang.c interpreter.c value.c environment.c utils.c slog.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = olang

# Default target
all: $(TARGET)

# Rule to link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Rule to compile source files into object files
%.o: %.c lexer.h
	$(CC) $(CFLAGS) -c $<

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
