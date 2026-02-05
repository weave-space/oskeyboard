# Variables for compiler and flags
CC = gcc
PKG_CONFIG = pkg-config
PACKAGES = gtk4 libevdev gtk4-layer-shell-0

# Fetch flags and libs using pkg-config
CFLAGS = $(shell $(PKG_CONFIG) --cflags $(PACKAGES))
LIBS = $(shell $(PKG_CONFIG) --libs $(PACKAGES))

# Target executable name
TARGET = osk

# Source files
SRC = main.c resources.c
OBJ = $(SRC:.c=.o)

# Default rule: build the target
all: $(TARGET)

# Rule to link the final executable
$(TARGET): resources.c $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LIBS)

# Rule to compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule to generate resources.c from XML
resources.c: resources.xml
	glib-compile-resources resources.xml --target=resources.c --generate-source

# Clean up generated files
clean:
	rm -f $(TARGET) $(OBJ) resources.c

.PHONY: all clean
