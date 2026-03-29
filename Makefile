# ==============================================================================
# Makefile for LineBlitz
# Structure:
#   LineBlitz/
#   ├── Makefile
#   ├── src/
#   │   ├── main.c, discovery.c, etc.
#   │   └── lineblitz.h
# ==============================================================================

CC = gcc
# Added -Isrc to tell the compiler where to find lineblitz.h
CFLAGS = -O3 -Wall -Wextra -std=gnu11 -Isrc

# The final binary name residing in the root folder
TARGET = lineblitz

# Path to source files
SRCDIR = src
SRCS = $(wildcard $(SRCDIR)/*.c)

# Object files will be created in the root during build, then cleaned later
OBJS = $(SRCS:$(SRCDIR)/%.c=%.o)

# Default target: The beast itself
$(TARGET): $(OBJS)
	@echo "[Linking] Assembling the beast from $(SRCDIR)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	@echo "========================================================"
	@echo " Compilation successful. Target: ./$(TARGET)"
	@echo " Source code remains untouched in $(SRCDIR)/"
	@echo "========================================================"

# Compile object files from src/ to the current root
%.o: $(SRCDIR)/%.c
	@echo "[Compiling] $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup protocol: Removes objects and the binary
.PHONY: clean
clean:
	@echo "[Cleaning] Sweeping the floor..."
	rm -f $(OBJS) $(TARGET)
	@echo "Cleanup complete."
