

CC = gcc

TARGET = mem_stress mem_access_1s

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

mem_stress: mem_stress.c
	$(CC) $^ $(LDFLAGS) -lnuma -o $@

mem_access_1s: mem_access_1s.c
	$(CC) $^ $(LDFLAGS) -o $@

clean:
	rm -f *.o $(TARGET)