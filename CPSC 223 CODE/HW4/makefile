#Ben Foxman netid-btf28
#Makefile copied from Professor Eisenstat

CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -g3 

HWK4= /c/cs223/Hwk4

all:    Qsort testDeque QsortH 

Deque.o:   $(HWK4)/Deque.h

#####
# Instructions to make example, which demonstrates the use of the Deque ADT
#####

example: example.o Deque.o
	${CC} ${CFLAGS} -o $@ $^

example.o: $(HWK4)/Deque.h


#####
# Instructions to make Qsort
#####

Qsort: Qsort.o Deque.o
	${CC} ${CFLAGS} -o $@ $^

Qsort.o: $(HWK4)/Deque.h


#####
# Instructions to make testDeque
#####

testDeque: Deque.o $(HWK4)/testDeque.o
	${CC} ${CFLAGS} -o $@ $^

testDeque.o: $(HWK4)/Deque.h


#####
# Instructions to make QsortH
#####

QsortH: Qsort.o $(HWK4)/Deque.o
	${CC} ${CFLAGS} -o $@ $^


clean:
	rm -f Qsort testDeque QsortH example *.o