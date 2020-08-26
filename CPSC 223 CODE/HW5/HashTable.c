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


//repreent a hashtable
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

    //increase size of old entries
    (*h) -> cells[index].numEntries++;

    //add another Triple to entries of the cell at that corresponding index
    Triple *newEntries = realloc((*h) -> cells[index].entries, (*h) -> cells[index].numEntries * sizeof(Triple));
    (*h) -> cells[index].entries = newEntries;

    //no collisions
    if ((*h) -> cells[index].numEntries == 1){
        newEntries[0] = newEntry;
    }
    //collision handling - insert those keys before the new key, then the key, then those after
    else {
        int entriesBefore = 0;
        while (entriesBefore < (*h) -> cells[index].numEntries - 1 && strcmp(newEntries[entriesBefore].pos, pos) > 0){
            entriesBefore++;
        }
        for (int i = (*h) -> cells[index].numEntries - 1; i > entriesBefore; i--){
            newEntries[i] = newEntries[i - 1];
        }
        newEntries[entriesBefore] = newEntry;
    }
    
}

void reHashH(HashTable *old, HashTable *new){
    //go through old, copy all elements into a new hashtable 
    for (int i = 0; i < (*old) -> size; i++){
        for (int j = 0; j < (*old) -> cells[i].numEntries; j++){
            Triple t = (*old) -> cells[i].entries[j];
            insertH(new, t.pos, t.prevPos, t.nSteps);
        }
    }
}


int searchH (HashTable *h, char *pos){
    //hash function to compute index
    int index = hash(pos, (*h) -> size);

    if (pos == NULL || (*h) -> cells[index].numEntries == 0){
        return -1;
    }

    //while loop to increment to where there could be a match
    int i = 0;
    while (i < (*h) -> cells[index].numEntries - 1 && strcmp(pos,(*h) -> cells[index].entries[i].pos) < 0){
        i++;
    }
    //check if match exists
    if (strcmp(pos,(*h) -> cells[index].entries[i].pos) == 0){
        return (*h) -> cells[index].entries[i].nSteps;
    }
    //unsuccessful search
    return -1;
}

void printFromPosH(HashTable *h, char *pos){
    if (searchH(h, pos) == -1){
        fprintf(stderr, "Trying to print a non-existent solution\n");
        exit(1);
    }

    //words will be added to the stack in order from goal to initial
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
    }
    //remove and print from the stack
    while (!isEmptyD(&stack)){
        remD(&stack, &toPrint);
        fprintf(stdout, "%s\n", toPrint);
    }
    destroyD(&stack);
}





