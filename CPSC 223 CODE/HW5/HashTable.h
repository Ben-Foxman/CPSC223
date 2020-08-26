/*
Ben Foxman - netid (02/26/20)
Interface for a HashTable
*/
#include <stdbool.h>

typedef struct hashtable *HashTable;

//create a hashtable with size "size" at *h
void createH (HashTable *h, int size);

//insert a triple into the hashtable *h
void insertH (HashTable *h, char *pos, char *prevPos, int max);

//return the nSteps of the triple with key pos, -1 if the key isn't in the table
int searchH (HashTable *h, char *pos);

//print the sequence of moves from the initial key to the triple with key pos
void printFromPosH(HashTable *h, char *pos);

//create a hashtable 8 times larger than *old at *new
void reHashH(HashTable *old, HashTable *new);





