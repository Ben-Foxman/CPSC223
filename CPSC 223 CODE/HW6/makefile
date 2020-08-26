#Ben Foxman - netid btf28
CC = gcc
CFLAGS = -g3 -std=c99 -pedantic -Wall

all:    Boggle nLines

Boggle:  Boggle.o
	${CC} ${CFLAGS} $^ -o $@

nLines:
	/c/cs223/bin/xLines < Boggle.c

clean:
	rm -f Boggle *.o