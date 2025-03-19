CC = gcc
CFLAGS = -O2
OBJ = main.o btree1.o
DEPS = config.h

all: btree

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

btree: $(OBJ)
	$(CC) $(CFLAGS) $^ -lnuma -o $@

clean:
	rm -f *.o btree
