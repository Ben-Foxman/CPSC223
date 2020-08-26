# Ben Foxman - netid btf28
# makefile identical to that of homework one, Cxref = Binpack.
CFLAGS = -g3 -std=c99 -pedantic -Wall

all:    Binpack nLines

Binpack:  Binpack.o
	${CC} ${CFLAGS} $^ -o $@

nLines:
	/c/cs223/bin/xLines < Binpack.c

clean:
	rm -f Binpack *.o