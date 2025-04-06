# Compiler settings
CC = gcc

# Target settings (default if not specified)
TARGET ?= scan_time

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