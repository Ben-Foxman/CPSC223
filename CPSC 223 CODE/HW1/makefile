#Ben Foxman - netid btf28
CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

all:    Cxref nLines

Cxref:  Cxref.o
	${CC} ${CFLAGS} $^ -o $@

nLines:
	/c/cs223/bin/xLines < Cxref.c

clean:
	rm -f Cxref *.o