CC = gcc
CFLAGS = -Iinclude/ -Wall -pedantic -Wswitch -Wswitch-enum -Wimplicit-fallthrough=3 -Wextra 
MAIN = main.c
PROGRAM_NAME = main
# Define the directories for the source and object files
SRC := src
RDIR := bin/release
DDIR := bin/debug

# Create a list of all C files in the source directory
SOURCES := $(wildcard $(SRC)/*.c)
# Create a corresponding list of object files for the binary directory
OBJECTS := $(patsubst $(SRC)/%.c, $(RDIR)/%.o, $(SOURCES))
DOBJECTS := $(patsubst $(SRC)/%.c, $(DDIR)/%.o, $(SOURCES))

# Default rule to build all object files
all: $(OBJECTS) 
	$(CC) $(MAIN) -o $(RDIR)/$(PROGRAM_NAME) $(CFLAGS) $(OBJECTS)

# Rule to build an object file from each source file
$(RDIR)/%.o: $(SRC)/%.c
	$(CC) -I$(SRC) -c $< -o $@ $(CFLAGS)

debug: $(DOBJECTS) clear
	$(CC) $(MAIN) -o $(DDIR)/$(PROGRAM_NAME) $(CFLAGS) $(DOBJECTS) -ggdb -DDEBUG

# Rule to build an object file from each source file
$(DDIR)/%.o: $(SRC)/%.c
	$(CC) -I$(SRC) -c $< -o $@ $(CFLAGS) -ggdb -DDEBUG
clear_all:
	rm $(wildcard $(RDIR)/*)

clear_debug:
	rm $(wildcard $(DDIR)/*)
