/* 
Ben Foxman - netid btf28
Implementation of the HashTable ADT
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HashTable.h"
#include "/c/cs223/Hwk4/Deque.h"
#include "/c/cs223/Hwk5/hashFunction.c"

//represent a triple
typedef struct triple {
    char *pos;
    char *prevPos;
    int nSteps;
} Triple;

//represent a cell in the hashtable
typedef struct cell {
    Triple *entries;
    int numEntries;
}Cell;


struct hashtable {
    int size; 
    Cell *cells;
};

void createH (HashTable *h, int size){
    //set up hashtable framework
    *h = malloc(sizeof(**h));
    (*h) -> size = size;
    (*h) -> cells = malloc(size * sizeof(Cell));

    //each cell of the hashtable has no entries
    for (int i = 0; i < size; i++){
        Cell newCell;
        newCell.entries = NULL;
        newCell.numEntries = 0;
        (*h) -> cells[i] = newCell;
    }
}

void insertH (HashTable *h, char *pos, char *prevPos, int nSteps){
    //insert data into a new triple
    Triple newEntry;
    newEntry.pos = pos;
    newEntry.prevPos = prevPos;
    newEntry.nSteps = nSteps;

    //hash function to compute index
    int index = hash(pos, (*h) -> size); 

    //add another Triple to entries of the cell at that corresponding index
    (*h) -> cells[index].numEntries++;
    //increase size of old entries
    Triple *newEntries = realloc((*h) -> cells[index].entries, (*h) -> cells[index].numEntries * sizeof(Triple));
    (*h) -> cells[index].entries = newEntries;
    
    newEntries[(*h) -> cells[index].numEntries - 1] = newEntry;
}

int searchH (HashTable *h, char *pos){
    if (pos == NULL){
        return -1;
    }
    //hash function to compute index
    int index = hash(pos, (*h) -> size);
    for (int i = 0; i < (*h) -> cells[index].numEntries; i++){
        if (strcmp(pos,(*h) -> cells[index].entries[i].pos) == 0){
            return (*h) -> cells[index].entries[i].nSteps;
        }
    }
    //unsuccesful search
    return -1;
}

void printFromPosH(HashTable *h, char *pos){
    if (searchH(h, pos) == -1){
        fprintf(stderr, "Trying to print a non-existent solution\n");
        exit(1);
    }

    //add the words to the stack in order from goal to initial
    Deque stack;
    createD(&stack);

    char *toPrint = pos;

    //while the solution moves have not all been added to the stack
    while (toPrint){
        //hash function to compute index
        int index = hash(toPrint, (*h) -> size);

        //iterate through entries of a cell
        for (int i = 0; i < (*h) -> cells[index].numEntries; i++){

            //when the match is found
            if (strcmp(toPrint, (*h) -> cells[index].entries[i].pos) == 0){
    
                //push onto stack
                pushD(&stack, toPrint);
                toPrint = (*h) -> cells[index].entries[i].prevPos;
                break;
            }
        }
        //printf("new to print: %s\n", toPrint);
    }
    //remove and print from the stack
    while (!isEmptyD(&stack)){
        remD(&stack, &toPrint);
        fprintf(stdout, "%s\n", toPrint);
    }
    destroyD(&stack);
}



