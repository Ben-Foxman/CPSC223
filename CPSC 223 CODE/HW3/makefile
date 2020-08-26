#Ben Foxman - netid btf28

CFLAGS = -g3 -std=c99 -pedantic -Wall

all:    strwrs nLines

strwrs:  strwrs.o
	${CC} ${CFLAGS} $^ -o $@

nLines:
	/c/cs223/bin/xLines < strwrs.c

clean:
	rm -f strwrs *.o