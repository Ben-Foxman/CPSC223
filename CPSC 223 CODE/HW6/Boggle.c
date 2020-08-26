/*
Ben Foxman - netid btf28
Problem Set 6 - Boggle
*/

#define _GNU_SOURCE
#define COMMAND_LINE_ERROR  exit(fprintf(stderr, "Invalid command line arguments.\n"));

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

//Represent a trie by a node, containing a list of other nodes as well as
//data about that node
typedef struct node {
	struct node *letters[26];
	char *word;
	int wordCount;
}Node;

//tree traversal to print words
void printWords(Node *root, bool cFlag);

//perform graph search of the boggle board
void searchFrom(Node *root, int pos, char *board, int nRows, int nCols, bool tFlag, bool visited[]);

int main(int argc, char *argv[]){
	bool cFlag = false; 
	bool tFlag = false;
	int nRows = 0;
	int nCols = 0;
	char *board = NULL;
	//////////////////////////////////////////////////////////
	//COMMAND LINE PARSING
	int offset = 0; //used to keep track of -c, -t, flags

	//min 4 cargs
	if (argc < 4){
		COMMAND_LINE_ERROR
	}
	else if (strcmp(argv[1], "-c") == 0 && strcmp(argv[2], "-t") == 0){
		cFlag = true;
		tFlag = true;
		offset += 2;
	}
	else if (strcmp(argv[1], "-c") == 0){
		cFlag = true;
		offset++;
	}
	else if (strcmp(argv[1], "-t") == 0){
		tFlag = true;
		offset++;
	}

	//4 non-optional cargs
	if (offset + 4 != argc){
		COMMAND_LINE_ERROR
	}

	//ensure nRows is a string a digits
	for (int i = 0; i < strlen(argv[offset + 1]); i++){
		if (!isdigit(argv[offset + 1][i])){
			COMMAND_LINE_ERROR
		}
	}

	//ensure nCols is a string of digits
	for (int i = 0; i < strlen(argv[offset + 2]); i++){
		if (!isdigit(argv[offset + 2][i])){
			COMMAND_LINE_ERROR
		}
	}

	nRows = atoi(argv[offset + 1]);
	nCols = atoi(argv[offset + 2]);

	//pos ints for rows/cols
	if (nRows <= 0 || nCols <= 0){
		COMMAND_LINE_ERROR
	}

	int boardChars = strlen(argv[offset + 3]);

	if(boardChars != nRows * nCols){
		COMMAND_LINE_ERROR
	}

	board = malloc(boardChars + 1); 

	//copy specified board to board variable in lower case
	for (int i = 0; i < boardChars; i++){
		if (!(isalpha(argv[offset + 3][i]) || argv[offset + 3][i] == '_')){
			COMMAND_LINE_ERROR
		}
		board[i] = tolower(argv[offset + 3][i]);
	}

	//////////////////////////////////////////////////
	//CREATE TRIE

	//set up initial root
	Node *root = malloc(sizeof(Node));
	for (int i = 0; i < 26; i++){
		root -> letters[i] = NULL;
	}
	root -> word = NULL;
	root -> wordCount = 0;

	//variables to read in stdin
	char *word = NULL;
	size_t buff = 0;

	//check if stdin line is a valid word
	bool validWord = true;

	while (getline(&word, &buff, stdin) != -1) {
		validWord = true;
		//remove trailing newlines
		word[strcspn(word, "\n")] = 0;

		//all chars of word must be a-zA-Z
		for (int i = 0; i < strlen(word); i++){
			if (!isalpha(word[i])){
				validWord = false;
				break;
			}
			//convert to lowercase
			word[i] = tolower(word[i]);
		}

		//add valid words to trie
		if (validWord){
			Node *currentNode = root;

			//for each letter of the word
			for (int i = 0; i < strlen(word); i++){
				int letter = word[i] - 'a';

				//if need to create new branch of the trie
				if (!currentNode -> letters[letter]){
					Node *newNode = malloc(sizeof(Node));

					for (int i = 0; i < 26; i++){
						newNode -> letters[i] = NULL;
					}
					newNode -> word = NULL;
					newNode -> wordCount = 0;
					currentNode -> letters[letter] = newNode;
				}

				//move currentNode one step down
				currentNode = currentNode -> letters[letter];
			}
			//store word in the last node
			currentNode -> word = word;
		}
		buff = 0;
		word = NULL;
	}
	////////////////////////////////////////////
	//GRAPH SEARCH OF BOGGLE BOARD

	//for each letter on the board
	for (int i = 0; i < nCols * nRows; i++){

		//underscore matches all letters
		if (board[i] == '_'){
			for (int j = 0; j < 26; j++){

				//letter must appear in both trie and boggle board
				if (root -> letters[j]){

					//if tFlag is specified, must keep track of which squares have been visited
					bool alreadyVisited[nCols * nRows];
					for (int j = 0; j < nCols * nRows; j++){
						alreadyVisited[j] = false;
					}
					if (tFlag) {
						alreadyVisited[i] = true;
					}

					//call recursive graph search
					searchFrom(root -> letters[j], i, board, nRows, nCols, tFlag, alreadyVisited);
				}
			}
		}
		else {
			int letter = board[i] - 'a';

			//letter must appear in both trie and boggle board
			if (root -> letters[letter]){

				//if tFlag is specified, must keep track of which squares have been visited
				bool alreadyVisited[nCols * nRows];
				for (int j = 0; j < nCols * nRows; j++){
						alreadyVisited[j] = false;
				}
				if (tFlag) {
					alreadyVisited[i] = true;
				}

				//call recursive graph search
				searchFrom(root -> letters[letter], i, board, nRows, nCols, tFlag, alreadyVisited);
			}
		}
	}
	/////////////////////////////////////////////////////////////
	//TRAVERSAL OF TREE TO PRINT ALL WORDS
	printWords(root, cFlag);
	exit(0);
}

void printWords(Node *root, bool cFlag){
	//cFlag = print only words with count 0
	if (cFlag){
		if (root -> word && root -> wordCount == 0){
			printf("%s\n", root -> word);
		}
	}
	else {
		if (root -> word && root -> wordCount > 0){
			printf("%s: %d\n", root -> word, root -> wordCount);
		}
	}

	//call recursively from left-to-right, since alphabetical order is
	//top-down, and left-right
	for (int i = 0; i < 26; i++){
		if (root -> letters[i] != NULL){
			printWords(root -> letters[i], cFlag);
		}
	}
}

void searchFrom(Node *root, int pos, char *board, int nRows, int nCols, bool tFlag, bool visited[]){
	if (root -> word){
		root -> wordCount++;
	}

	//loop through the 3x3 grid with center at the current pos
	for (int hOffset = -1; hOffset <= 1; hOffset++){

		//bottom row 
		if (hOffset == -1 && pos / nCols == 0){
			continue;
		}

		//top row
		if (hOffset == 1 && pos / nCols == nRows - 1){
			continue;
		}

		for (int wOffset = -1; wOffset <= 1; wOffset++){

			//left col
			if (wOffset == -1 && pos % nCols == 0){
				continue;
			}

			//right col
			if (wOffset == 1 && pos % nCols == nCols - 1){
				continue;
			}

			//can't repeat pos
			if (wOffset == 0 && hOffset == 0){
				continue;
			}

			int newPos = pos + (hOffset * nCols) + wOffset;

			//no repeats if tFlag specified
			if (tFlag && visited[newPos]){
				continue;
			}

			if (board[newPos] == '_'){
				for (int j = 0; j < 26; j++){
					//call recursive search if any letter is represented in trie
					if (root -> letters[j]){
						visited[newPos] = true;
						searchFrom(root -> letters[j], newPos, board, nRows, nCols, tFlag, visited);
						visited[newPos] = false;
					}
				}
			}
			else {
				int newLetter = board[newPos] - 'a';
				if (root -> letters[newLetter]){
					//call recursvie search if this letter is represented in trie
					visited[newPos] = true;
					searchFrom(root -> letters[newLetter], newPos, board, nRows, nCols, tFlag, visited);
					visited[newPos] = false;
				}
			}
		}
	}
}