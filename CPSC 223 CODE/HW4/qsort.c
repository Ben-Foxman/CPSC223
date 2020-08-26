/* Ben Foxman netid-btf28
CS223 - Problem 4 
QuickSort Using Deques
This program read lines from specified files, and sorts
them in ascending order based on the "key" (a substring
of each line). The program then prints each sorted line to 
stdout. 
*/


#define _GNU_SOURCE

//the following two macros are copied from example.c (Stan Eisenstat)

// Print message to stderr and exit.
#define DIE(msg)    exit (fprintf (stderr, "%s\n", msg))

// Print error message unless COND is true
#define ORDIE(cond,msg)     if (cond) ; else DIE(msg)

#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "/c/cs223/Hwk4/Deque.h"

//given inputs in Q, quicksort and place outputs in S1
bool quicksort (Deque Q, Deque S1, Deque S2, int pos, int len, bool stability);

int main(int argc, char *argv[]) {
	Deque Q;
	Deque S1;
	Deque S2;
	ORDIE(createD(&Q), "createQ() failed");
	ORDIE(createD(&S1), "createS1() failed");
	ORDIE(createD(&S2), "createS2() failed");

	//default value for pos,len
	int pos = 0; 
	int len = INT_MAX;
	int firstFile = 1; //index of first file to be read

	////////////////////////////////////////////////////////////////////////////

	//parse pos,len if specified
	if (argc >= 2 && argv[1][0] == '-'){
		firstFile++;
		char *temp = NULL;
		pos = -strtol(argv[1], &temp, 10);
		if (strcmp(temp, "") != 0){
			if ((temp[0] != ',' || strlen(temp) < 2) || temp[1] == '+'){
			DIE("Invalid pos,len flag.");
			}
			else {
				len = strtol(temp + 1, &temp, 10);
				if (len < 0){
					DIE("Invalid pos,len flag.");
				}
			}
		}
		if (strcmp(temp, "") != 0){
			DIE("Invalid pos,len flag.");
		}
	}

    ////////////////////////////////////////////////////////////////////////////

	//push all strings into Q
	char *buffer = NULL;
	size_t bufferSize = 0;
	FILE *file; 
	//for every file
	for (int i = firstFile; i < argc; i++){ 
		if (!(file = fopen(argv[i], "r"))){
			fputs("Could not open file.\n", stderr);
			exit(1);
		}
		//add all lines of files to Q
		while(getline(&buffer, &bufferSize, file) >= 0){
			buffer[strcspn(buffer, "\n")] = 0; //remove trailing newline
			addD(&Q, buffer); 
			bufferSize = 0;
			buffer = NULL;
		}
		fclose(file);
	}
	free(buffer);

	// >0 lines, quicksort them (result is stored in S1)
	if (!isEmptyD(&Q)){
		quicksort(Q, S1, S2, pos, len, false); //perform quicksort - sorted order stored in S1
	}

	//remove and print each item of S1
	while (!isEmptyD(&S1)){
		ORDIE(remD(&S1, &buffer), "remS1() failed");
		printf("%s\n", buffer);
		free(buffer);
	}
	ORDIE(destroyD(&Q), "destroyQ() failed");
	ORDIE(destroyD(&S1), "destroyS1() failed");
	ORDIE(destroyD(&S2), "destroyS2() failed");
}

////////////////////////////////////////////////////////////////////////////////

bool quicksort (Deque Q, Deque S1, Deque S2, int pos, int len, bool stability){
	char *splitter = NULL; //buffer to store splitter
	char *toCompare = NULL; //buffer to stor non-splitter elements to compare
	int lowerCount = 0; //count of elements less than splitter
	int higherCount = 0; //count of elements greater than splitter

	//remove splitter from Q
	ORDIE(remD(&Q, &splitter), "remQ() failed");

	//if pos > strlen, we need to set set pos back to strlen to avoid error
	int cmpPos = pos; 
	int splitPos = pos;
	bool splitterEmpty = false;
	bool cmpEmpty = false;

	//if splitter is too short, key is empty string
	if (pos > strlen(splitter)){
		splitterEmpty = true;
	}

	while(!isEmptyD(&Q)){
		ORDIE(remD(&Q, &toCompare), "remQ() failed");
		//if toCompare is too short, key is empty string
		if (pos > strlen(toCompare)){
			cmpEmpty = true;
		}

		int compareVal = 0;
		
		if (splitterEmpty && cmpEmpty){
			compareVal = 0;
		}
		else if (splitterEmpty) {
			compareVal = strncmp(toCompare + cmpPos, "", len);
		}
		else if (cmpEmpty) {
			compareVal = strncmp("", splitter + splitPos, len);
		}
		else {
			compareVal = strncmp(toCompare + cmpPos, splitter + splitPos, len);
		}
		

		//if spliiter == toCompare, push to S1 or S2 determined by stability
		//else, move to S1 or S2 according to relation with splitter
		if (compareVal == 0){
			if (stability){
				ORDIE(pushD(&S2, toCompare), "pushS2() failed");
				higherCount++;
			}
			else {
				ORDIE(pushD(&S1, toCompare), "pushS1() failed");
				lowerCount++;
			}
		}
		else if (compareVal > 0){
			ORDIE(pushD(&S1, toCompare), "pushS1() failed");
			lowerCount++;
		}
		else {
			ORDIE(pushD(&S2, toCompare), "pushS2() failed");
			higherCount++;
		}
	}

	//no lower elements - move splitter to S1
	if (lowerCount == 0){
		ORDIE(pushD(&S1, splitter), "pushS1() failed");
	}
	//recursively quicksort part below splitter
	if (lowerCount > 0){ 
		
		for (int i = 0; i < lowerCount; i++){
			ORDIE(remD(&S1, &toCompare), "remS1() failed");
			ORDIE(addD(&Q, toCompare), "addQ() failed");
		}
		quicksort(Q, S1, S2, pos, len, !stability);

		ORDIE(pushD(&S1, splitter), "pushS1() failed");
	}

	//recursively quicksort part above splitter
	if (higherCount > 0){ 
		for (int i = 0; i < higherCount; i++){
			ORDIE(remD(&S2, &toCompare), "remS2() failed");
			ORDIE(addD(&Q, toCompare), "addQ() failed");
		}
		quicksort(Q, S1, S2, pos, len, !stability);
	}
	return true;
}




