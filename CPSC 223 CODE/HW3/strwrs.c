/*
Ben Foxman netid- btf28
CS223 Problem 3
DUE 2/28/2020
Strwrs Program
Implement a string rewriting system, which reads strings from the standard input 
and prints to the standard output the string after a series of substitution 
rules, specified as command-line arguments, have been applied.
*/

//EROOR WITH MULTIPLE LINES OF STDIN
#define _GNU_SOURCE

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

//applyUpperRules: decides which string substitution rule to apply
char* applyUpperRules(char* input, char upperFlag, char lowerFlag, int startIndex, \
	int argc, char **argv);

//apply current string substitution to input in manner consistent with lower flag
char* applyLowerRules(char* input, char *fi, char *ti, char lowerFlag, \
	int startIndex, bool startPlus, bool endMinus);


int main(int argc, char **argv){
	char lowerFlag = 'n'; //default lower case flag
	char upperFlag = 'N'; //default upper case flag
	char *inputStr = NULL; //will store a string from stdin

	//if argc is even, the specified flags may not be the default
	if (argc % 2 == 0){

		//first char of the argument containing the new flags must be a -
		if (argv[1][0] != '-'){
			fprintf(stderr, "Invalid input.\n");
			return 1;
		}

		//loop through the all other chars of argv[1] to see which flags to use 
		for (int i = 1; argv[1][i] != '\0'; i++){
			//new lower flag
			if (argv[1][i] == 'q' || argv[1][i] == 'r' || argv[1][i] == 'n' \
				|| argv[1][i] == 's'){
				lowerFlag = argv[1][i];
			}
			//new upper flag
			else if (argv[1][i] == 'Q' || argv[1][i] == 'R' || argv[1][i] == 'N' \
				|| argv[1][i] == 'S'){
				upperFlag = argv[1][i];
			}
			//invalid char, print error and exit
			else {
				fprintf(stderr, "Invalid input.\n");
				return 1;
			}
		}
	}

	size_t buff = 0;
	//while stdin has lines, do all string substitutions on that line
	//then print out the contents and free the line
	while (getline(&inputStr, &buff, stdin) != -1) {
		inputStr = applyUpperRules(inputStr, upperFlag, lowerFlag, 2 - (argc % 2), argc, argv);
		fputs(inputStr, stdout);
		free(inputStr);
		buff = 0;
		inputStr = NULL; 
	}
	free(inputStr);
}


//applyUpperRules: recursive method which determines how multiple string substitutions
//are applied
char *applyUpperRules(char* input, char upperFlag, char lowerFlag, int fiIndex, \
	int argc, char **argv){

	//if no substitution rules remain to be applied
	if (fiIndex == argc){
		return input;
	}

	//fi needs to be preprocessed, so it has a variable length
	//ti will never change length - no malloc neccesary
	char *fi = malloc((strlen(argv[fiIndex]) + 1) * sizeof(char));
	strcpy(fi, argv[fiIndex]);
	char *ti = argv[fiIndex + 1];


	/*determine if the + and/or - fences are active for the current string 
	substitution - will be passed into the function which applies the lower
	rules*/

	bool startPlus = false;
	bool endMinus = false;
	if (*(fi) == '+'){
		startPlus = true;
	}
	if (strlen(fi) == 1 && *(fi) == '-') {
		endMinus = true;
	}
	else {
		endMinus = (*(fi + strlen(fi) - 1) == '-' && *(fi + strlen(fi) - 2) != '/');
	}
	
	//preprocess the fi string: remove all / which themselves are not escaped 
	//or at the end of the string
	for (int i = 0; i < strlen(fi) - 1; i++){
		if (*(fi + i) == '/'){
			char *newFi = malloc((strlen(fi) + 1) * sizeof(char));
			strncpy(newFi, fi, i);
			*(newFi + i) = '\0';
			strncat(newFi, fi + i + 1, strlen(fi) - i - 1);
			*(newFi + strlen(fi) - 1) = '\0';
			free(fi);
			fi = malloc((strlen(newFi) + 1) * sizeof(char));
			strcpy(fi, newFi);
			free(newFi);
		}
	}
	

	//make sure that the newly processed fi is not longer than the input
	//string itself, in which case no substitution can be applied
	if ((strlen(fi) - startPlus - endMinus) > (strlen(input) + 1)){
		return input;
	}

	//save a copy of the input before modifying it - use to determine if a
	//string substitution changed the string (i.e. was successful)
	char *inputCopy = malloc((strlen(input) + 1) * sizeof(char));
	inputCopy = strcpy(inputCopy, input);
	char *newString = applyLowerRules(input, fi, ti, lowerFlag, 0, startPlus, endMinus);
	free(fi);

	//determine is the string substitution was succesful 
	bool succesfulSub = strcmp(newString, inputCopy); 
	free(inputCopy);

	//the string substitution was successful - now use flag to determine which
	//string substitution rules to apply next
	if (succesfulSub){

		//Q flag: quit when a successful rule is found
		if (upperFlag == 'Q'){
			return newString;
		}

		//N flag: no condition is rule is successful
		else if (upperFlag == 'N'){
			return applyUpperRules(newString, 'N', lowerFlag, fiIndex + 2, argc, argv);
		}

		//R flag: if a rule is successful, reapply it.
		else if (upperFlag == 'R'){
			return applyUpperRules(newString, 'R', lowerFlag, fiIndex, argc, argv);
			//i doesnt change
		}

		//S flag: if a rule is successful, start over from first rule.
		else if (upperFlag == 'S'){
			return applyUpperRules(newString, 'S', lowerFlag, 2 - (argc % 2), argc, argv);
		}
	}
	//the string substitution was unsuccesful
	else {
		return applyUpperRules(newString, upperFlag, lowerFlag, fiIndex + 2, argc, argv);
	}

	return newString;
}
char *applyLowerRules(char* input, char *fi, char *ti, char lowerFlag, \
	int startIndex, bool startPlus, bool endMinus){

	bool matchFound = false; //determine if a string substitution is ready to be made
	int missingStart = startPlus; //1 if + fence exists, 0 otherwise 
	int missingEnd = endMinus; //1 if - fence exists, 0 otherwise 

	//length of the new string after a substiution is applied
	int newLength = strlen(input) + strlen(ti) + missingStart + missingEnd;

	//length of fi disregarding any + or - fences
	int effectiveFiLength = strlen(fi) - missingStart - missingEnd;

	//tracker is a substring of the input with length equal to effectiveFiLength 
	char *tracker = malloc((effectiveFiLength + 1) * sizeof(char));

	//the string after substitutions are applied
	char *newString = malloc((newLength + 1) * sizeof(char));
	strcpy(newString, input);

	//from the start index to the end of the string, check if a substitution should be made
	for (int i = startIndex; i < strlen(input); i++){

		// - fence active, skip to the part of the string where it could be used
		if (endMinus && i != strlen(input) - effectiveFiLength - 1){
			continue;
		}

		//copy the substring of length fi into the tracker beginning at index i 
		//in the string, if fi is longer than the input no substitution can be made
		if (strlen(input) - effectiveFiLength >= i){
			strncpy(tracker, input + i, effectiveFiLength); 
			*(tracker + effectiveFiLength) = '\0';
		}
		else {
			break;
		}

		// + fence means matches can only be found at the beginning of the string
		if (startPlus && i != 0) {
			free(input);
			free(tracker);
			return newString;
		}

		//more fi preprocessing: remove any active + or - fences
		//use this final version of fi to determine if a match has been found
		char *fiNoEnds = NULL;

		if (startPlus && endMinus){
			fiNoEnds = malloc((strlen(fi) + 2) * sizeof(char));
			strncpy(fiNoEnds, fi + 1, strlen(fi) - 2);
			*(fiNoEnds + strlen(fi) - 2) = '\0';
			matchFound = !strcmp(tracker, fiNoEnds);
		}
		else if (startPlus) {
			fiNoEnds = malloc((strlen(fi) + 2) * sizeof(char));
			strncpy(fiNoEnds, fi + 1, strlen(fi) - 1);
			*(fiNoEnds + strlen(fi) - 1) = '\0';
			matchFound = !strcmp(tracker, fiNoEnds);
		}
		else if (endMinus) {
			fiNoEnds = malloc((strlen(fi) + 2) * sizeof(char));
			strncpy(fiNoEnds, fi, strlen(fi) - 1);
			*(fiNoEnds + strlen(fi) - 1) = '\0';
			matchFound = !strcmp(tracker, fiNoEnds);
		}
		else {
			matchFound = !strcmp(tracker, fi);
		}
		free(fiNoEnds);

		//if a match is found, modify the newString accordingly
		if (matchFound){

			//special case: if fi is only "-", just add ti to the end of the string
			if (strcmp(fi, "-") == 0 && endMinus) {
				free(newString);
				newString = malloc(newLength + 1);
				strncpy(newString, input, strlen(input) - 1);
				*(newString + strlen(input) - 1) = '\0';
				strcat(newString, ti);
				strcat(newString, "\n");
				free(tracker);
				free(input);
				return newString;
			}

			//newString = (input before substitution) + (ti) + (input after substitution)
			free(newString);
			newString = malloc(newLength + 1);
			strncpy(newString, input, i);
			*(newString + i)= '\0';
			strcat(newString, ti);
			strncat(newString, input + i + effectiveFiLength, strlen(input) - i - effectiveFiLength);
			*(newString + newLength) = '\0';

			//if 'r' or 's' flag specified and string substitution causes no change
			//consider unsuccessful
			if ((lowerFlag == 'r' || lowerFlag == 's') && strcmp(newString, input) == 0){
				free(tracker);
				free(input);
				return newString; 
			}

			//now, use flags to determine if any more string substiutions using
			//the current rule should be made.

			//q flag: only replace leftmost occurence.
			if (lowerFlag == 'q'){
				free(tracker);
				free(input);
				return newString;
			}
			//n flag: when occurence found, continue scan at the end of the replacement.
			else if (lowerFlag == 'n'){
				free(tracker);
				free(input);
				return applyLowerRules(newString, fi, ti, 'n', i + strlen(ti), startPlus, endMinus);
			}
			//r flag: when occurence found, rescan from beginning of the replacement.
			else if (lowerFlag == 'r'){
				free(tracker);
				free(input);
				return applyLowerRules(newString, fi, ti, 'r', i, startPlus, endMinus);
			}
			//s flag: when occurence found, rescan the entire input.
			else if (lowerFlag == 's'){
				free(tracker);
				free(input);
				return applyLowerRules(newString, fi, ti, 's', 0, startPlus, endMinus);
			}
		}
	}

	free(input);
	free(tracker);
	return newString;
}









