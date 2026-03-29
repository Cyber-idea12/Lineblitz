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

# Default target: The beast itself, followed by a cleanup of its mess
all: $(TARGET)
	@echo "[Housekeeping] Deleting object files... because we hate clutter."
	@rm -f $(OBJS)
	@echo "========================================================"
	@echo " Compilation successful. Target: ./$(TARGET)"
	@echo " Object files removed. No evidence left behind."
	@echo "========================================================"

$(TARGET): $(OBJS)
	@echo "[Linking] Assembling the beast from $(SRCDIR)..."
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# Compile object files from src/ to the current root
%.o: $(SRCDIR)/%.c
	@echo "[Compiling] $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Cleanup protocol: Removes the binary if you want to start fresh
.PHONY: clean
clean:
	@echo "[Cleaning] Sweeping the floor..."
	rm -f $(OBJS) $(TARGET)
	@echo "Cleanup complete."
