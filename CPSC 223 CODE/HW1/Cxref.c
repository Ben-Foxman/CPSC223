/*
Problem 1
CPSC 223
Ben Foxman - netid btf28
Due 02/01/2020 at 2:00 am

This program acts a filter, reading a C Program from stdin and writing to
stdout every C identifier in the program, along with its line number. 
                                                                               
*/
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

void next_char();
void check_splices();
void get_identifier();
void skip_literal();
void check_forward_slash();
void skip_number();

static int lineCount = 1; //the current line count, starts at 1
static char currentChar; //current char being read
static char prevChar = ' '; //char most recently read before the current one
static bool noRead = false; //prevent/allow reading of char at top of while
//loop
static bool inIdentifier = false; //to print line numbers of identifiers which 
//end at EOF

int main() {

	while (true){

		//update current and previous chars
		if (!noRead){
			next_char();
		}

		noRead = false;
		inIdentifier = false;

		/*
		NOTE: the order in which we check special cases is important:
		a) Identifiers can only exist outside of literals and comments, and not 
		directly after numbers.
		b) Do not bother to check for anything in a comment, so skip over all 
		comments first.
		c) Since we can assume that there are no multicharacter char constants, 
		and there are multicharacter string constants, we should check strings
		before characters.
		*/

		//forward slash: can be the start of a comment
		if (currentChar == '/') {
			check_forward_slash();
		}

		//quote: start of literal - skip until close quote or EOF
		if (currentChar == '"' || currentChar == '\'') {
			skip_literal();
		}

		//number: skip all immediately following alphanumeric chars
		if (isdigit(currentChar)) {
			skip_number();
		}

		//chars to start an identifier must be alphabetical or an underscore 
		if (isalpha(currentChar) || currentChar == '_'){
			get_identifier();
		}
	}
}

//Description: when a forward slash is found, this method checks to see if a 
//comment has been initiated, in which case it skips the entire comment.
void check_forward_slash(){
	next_char();
	check_splices();

	//one-line comment: skip rest of line
	if (currentChar == '/') {

		while (!(currentChar == '\n')) {
			next_char();
			check_splices();
		}
	}

	//block comment: skip until */ or EOF
	else if (currentChar == '*') {
		next_char();
		next_char(); // to avoid the /*/ from ending the comment

		while (!(currentChar == '/' && prevChar == '*')) {
			next_char();
			check_splices();
		}
	}
}

//Description: when a quote is found, this method skips the accompanying 
//literal.
void skip_literal(){
	char type = currentChar; //determine if single or double quote 
	//make sure the close quote isn't escaped
	next_char();

	while (!(currentChar == type && prevChar != '\\')) {

		//make sure that the backslash that would otherwise escape the literal 
		//isn't escaped itself, so change an escaped backlash to some other 
		//arbitrary character.
		if (currentChar == '\\' && prevChar == '\\') {
			currentChar = 'a';
		}
		next_char();
	}

}

//Description: When a number is found, skip all alphanumeric characters which 
//follow it.
void skip_number(){
	while (isalnum(currentChar)) {
		next_char();
		check_splices();
	}
	/*
	In the above while loop, we read one too many chars(the char after the 
	last alphanumeric char), so use noRead to prevent another char from being
	read at the top of the loop.
	*/ 
	noRead = true;
}

//Description: when an identifier is found, this method prints all the 
//chars of that identifier.
void get_identifier(){
	inIdentifier = true;
	int currentLineCount = lineCount; //save the current line count in 
	//case it changes due to a line splice

	while (isalnum(currentChar) || currentChar == '_') {

		if (inIdentifier){
			printf("%c", currentChar);
			next_char();
			check_splices();
		}
		else {
			break;
		}
	}

	/*
	In the above while loop, we read one too many chars(the char after the 
	identifier), so use noRead to prevent another char from being read at the
	top of the loop.
	*/ 
	noRead = true;
	printf(":%d\n", currentLineCount);
}

//Description: next_char updates the state of currentChar and
//prevChar after reading a character from a file.
void next_char(){
	int temp = getchar();

	if (temp == EOF){

		if (inIdentifier) {
			printf(":%d\n", lineCount);
		}
		exit(0);
	}
	prevChar = currentChar;
	currentChar = temp;

	if (currentChar == '\n') {
		lineCount++;
	}
}

// Description: If a line splice is found, "skip over" both the backlash and
// the newline character.
void check_splices(){

	while (currentChar == '\\') {
		next_char();
		if (currentChar == '\n') {
			next_char();
			continue;
		}
		else {
			//no line splice, so an identifier is terminated
			inIdentifier = false;
		}
	}
}

