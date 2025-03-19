# Makefile for Linux

SHELL = /bin/sh

# System-specific settings

CC =		gcc
CCFLAGS =	-O -g -DCHECK -I /usr/include/x86_64-linux-gnu/mpich
LINK =		gcc
LINKFLAGS =	-O -g 
LIB =		-lmpich

# Link target

gups:	gups.o
	$(LINK) $(LINKFLAGS) gups.o $(LIB) -o gups

gups_nonpow2:	gups_nonpow2.o
	$(LINK) $(LINKFLAGS) gups_nonpow2.o $(LIB) -o gups_nonpow2

gups_opt:	gups_opt.o
	$(LINK) $(LINKFLAGS) gups_opt.o $(LIB) -o gups_opt

# Compilation rules

%.o:%.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f *.o gups gups_nonpow2 gups_opt
