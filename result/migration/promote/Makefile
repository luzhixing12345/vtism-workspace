
LDFLAG = -lnuma

all: numa_promote

numa_promote: numa_promote.o
	$(CC) -o $@ $< $(LDFLAG)

numa_promote.o: numa_promote.c
	$(CC) -c $<


clean:
	rm -f *.o numa_promote