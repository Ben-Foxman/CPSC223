#Ben Foxman netid-btf28

CC=gcc
CFLAGS= -std=c99 -pedantic -Wall -g3 

HWK5 = /c/cs223/Hwk5
HWK4 = /c/cs223/Hwk4

all: Nine20 Deque

HashTable.o: HashTable.h

#(HWK5)/Deque.o:  $(HWK5)/Deque.h


#####
# Instructions to make Nine20
#####




Nine20: Nine20.o HashTable.o $(HWK5)/Deque.o
	${CC} ${CFLAGS} -o $@ $^

Nine20.o: HashTable.h $(HWK4)/Deque.h
#Nine20.o: $(HWK5)/Deque.h


clean:
	rm -f Nine20 *.o
