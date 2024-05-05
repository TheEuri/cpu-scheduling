CC=gcc
CFLAGS=-I.

.PHONY: all clean

all: rate edf

rate: real.c
				$(CC) real.c -o $@ $(CFLAGS)

edf: real.c
				$(CC) real.c -o $@ $(CFLAGS)

clean:
				rm -f rate edf