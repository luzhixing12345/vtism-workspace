# Compiler settings
CC = gcc
# CFLAGS = -O3 -Wall
LDFLAGS = -lnuma -lpthread

# Target settings (default if not specified)
TARGET ?= numa_promote multi_numa_promote

# Build both .o and executable by default
all: $(TARGET).o $(TARGET)

# Rule for building object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Rule for building executables
%: %.o
	$(CC) $< -o $@ $(LDFLAGS)

# Clean rule
clean:
	rm -f *.o $(TARGET) $(TARGET).o

.PHONY: all clean