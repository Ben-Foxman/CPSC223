/*
Problem 5
Nine Puzzle Using Hash Tables and Breadth-First Search
Ben Foxman netid - btf28
*/


#define _GNU_SOURCE
#define COMMAND_LINE_ERROR  exit(fprintf(stderr, "Invalid command line arguments.\n"));

#include "HashTable.h"
#include "/c/cs223/Hwk4/Deque.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>


//used to sort initial and goal strings to determine if they have the same characters
int compareChar(const void *a, const void *b);

//called each time a "slide" is possible in the puzzle
void useTable(char *oldInitial, int toSwap, int nSteps, int emptyPos);

int height = 3; //default height
int width = 3;  //default width
int maxSteps = 0; //will be changed by command line args
char *initial = NULL; 
char *goal = NULL;
int tableSize = 1024; //initial table size as given by spec
int insertions = 0; //total insertions into all hash tables, used to measure load avg
HashTable currentTable; //hashtable used to store triples
Deque queue; //queue used for breath-first search


int main(int argc, char *argv[]){

	////////////////////////////////////////////////////////////////////////////
	int offset = 0; //add 2 if height and width are specified
	if (argc == 4 || argc == 6){
		//handle height/width specification - between 2 and 5 inclusive
		if (argc == 6) {
			if (!isdigit(argv[1][0]) || !isdigit(argv[2][0])){
				COMMAND_LINE_ERROR
			}
			height = atoi(argv[1]);
			width = atoi(argv[2]);
			offset += 2;
			if (height < 2 || height > 5 || width < 2 || width > 5){
				COMMAND_LINE_ERROR
			}
		}
		//get maxSteps - make sure that maxSteps is a sequence of digits
		for (int i = 0; i < strlen(argv[offset + 1]); i++){
			if (!isdigit(argv[offset + 1][i])){
				COMMAND_LINE_ERROR
			}
		}
		maxSteps = atoi(argv[offset + 1]);

		//Get INITIAL and GOAL, make sure that they are of valid length
		int entries = height * width;
		if (strlen(argv[offset + 2]) != entries || strlen(argv[offset + 3]) != entries){
			COMMAND_LINE_ERROR
		}

		//sort initial and goal by characters
		char *initialTemp = strdup(argv[offset + 2]);
		char *goalTemp = strdup(argv[offset + 3]);
		qsort(initialTemp, strlen(initialTemp), sizeof(char), compareChar);
		qsort(goalTemp, strlen(goalTemp), sizeof(char), compareChar);

		//sorted strings must match
		if (strcmp(initialTemp, goalTemp) != 0){
			COMMAND_LINE_ERROR
		}
		//string must contain '-' but not "--"
		if (!(strstr(initialTemp, "-") && !strstr(initialTemp, "--"))) {
			COMMAND_LINE_ERROR
		}
		initial = argv[offset + 2];
		goal = argv[offset + 3];
	}
	else {
		COMMAND_LINE_ERROR
	}
	////////////////////////////////////////////////////////////////////////////

	//breadth-first search algorithm

	if(strcmp(initial, goal) == 0){
		printf("%s\n", initial);
		exit(0);
	}
	//create a new HashTable of size 1024, it is the current table to use
	HashTable h;
	createH(&h, tableSize);
	createD(&queue);
	currentTable = h;

	//insert initial element to the HashTable
	insertH(&currentTable, initial, NULL, 0); 
	//add first word to queue
	addD(&queue, initial); 			
	char *oldInitial = NULL; //hold the old keys as they are used to generate new entries

	while(!isEmptyD(&queue)){ 	
		remD(&queue, &oldInitial); 
		int nSteps = searchH(&currentTable, oldInitial); 
		int emptyPos = strcspn(oldInitial, "-"); 

		if (nSteps < maxSteps){
			//left slide
			if (emptyPos % width != 0){ 
				int toSwap = emptyPos - 1;
				useTable(oldInitial, toSwap, nSteps, emptyPos);
			}
			//right slide
			if ((emptyPos + 1) % width != 0){ 
				int toSwap = emptyPos + 1;
				useTable(oldInitial, toSwap, nSteps, emptyPos);
			}
			//down slide
			if (emptyPos / width != 0){ 
				int toSwap = emptyPos - width;
				useTable(oldInitial, toSwap, nSteps, emptyPos);
			}
			//up slide
			if (emptyPos / width != (height - 1)){ 
				int toSwap = emptyPos + width;
				useTable(oldInitial, toSwap, nSteps, emptyPos);
			}
		}	
	}
	exit(0);
}

void useTable(char *oldInitial, int toSwap, int nSteps, int emptyPos){
	char *newInitial = strdup(oldInitial); 
	newInitial[emptyPos] = newInitial[toSwap];
	newInitial[toSwap] = '-';

	//check if newInitial is the goal pos
	if(strcmp(newInitial, goal) == 0){
		printFromPosH(&currentTable, oldInitial);
		printf("%s\n", newInitial);
		exit(0);
	}
	//check if newInitial not in the search table, and add it
	else if (searchH(&currentTable, newInitial) == -1){
		addD(&queue, newInitial);
		insertH(&currentTable, newInitial, oldInitial, nSteps + 1);
		insertions++;

		//if load avg exceeds 8, rehash the table, and make the new table the current table
		if (insertions / tableSize > 8.0){
			HashTable new;
			createH(&new, 8 * tableSize);
			reHashH(&currentTable, &new);
			currentTable = new;
			tableSize = 8 * tableSize;
		}
	}
	else {
		free(newInitial);
	}
	
}

int compareChar(const void *a, const void *b){
	char c = *((char*)a);
    char d = *((char*)b);
    return c - d;
}

