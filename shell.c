// shell.c
// Project 1
// Chris Zawora + Taylor Wan
// Operating Systems, Spring 2016

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>



/************************
 *	Constants + Globals
 ************************/

// builtins
const char* home = "home";
const char* dotdot = "..";
const char* ciao = "exit";
const char* cd = "cd";
const char* pwd = "pwd";
const char* _wait = "wait";
const char* history = "history";
const char* bangbang = "!!";
const char* bang = "!";

// error
const int ERROR_CODE = -1;

// sizing
const int SIZE = 512;
const int INT_100 = 100;

// characters
const int CHAR_SPACE = 32;
const int CHAR_NEWLINE = 13;
const int CHAR_NULL = 0;

// stack
const int PATHSTACK_SIZE = 50;
char* pathStack[50];
char* copyPathStack[50];
int copyStackPointer = 0;
int stackPointer = 0;

// history
char* historyArr[200];
const int HISTORYARR_SIZE = 200;
int historyCount = 0;

// boolean
typedef enum {true, false} bool;



/************************
 *	History Helpers
 *
 *	- printHistory
 *	- _addToHistory
 *	- _clearHistory
 ************************/

/** printHistory: prints the last 10 items in history
 */
void printHistory() {
	int i = 10;
	while (i > 0) {
		if (historyCount >= i) {
			printf("%d %s\n", historyCount-i, historyArr[historyCount-i]);
		}
		i--;
	}
}

/** _addToHistory: add an item to history
 * @param c - command to add
 */
void _addToHistory(char* c) {

	if (historyCount == HISTORYARR_SIZE) {

		//if history array is full, remove the first 20 and remake array
		int i = 0;
		for (i; i < 20; i++) {
			free(historyArr[i]);
		}
		for (i; i<HISTORYARR_SIZE; i++) {
			historyArr[i-20] = historyArr[i];
		}
		historyCount = 180;

	}

	if (strncmp(c, "\n", 1) != 0) {
		if (historyCount < HISTORYARR_SIZE) {
			char *util = malloc(sizeof(char)* INT_100);
			strcpy(util, c);
			historyArr[historyCount] = util;
			historyCount++;
		}
	}
}

/** _clearHistory: clear the history
 */
void _clearHistory() {

	while (historyCount != 0) {
		free(historyArr[historyCount-1]);
		historyCount--;
	}
}



/************************
 *	Stack Helpers
 *
 *	- _push
 *	- _pop
 *	- _clearStack
 *	- _clearCopyStack
 *	- _copyStack
 *	- _replaceStack
 *	- _printStack
 *	- _fillPathStack
 ************************/

/** _push: push to stack
 * @param c - item to push
 */
void _push(char* c) {
	if (stackPointer < PATHSTACK_SIZE) {

		char *util = malloc(sizeof(char)* INT_100);
		strcpy(util, c);
		pathStack[stackPointer] = util;
		stackPointer++;
	}
}

/** _pop: clear the history
 * @return - the item removed
 */
char * _pop() {
	if (stackPointer-1 >= 0) {
		char * val = pathStack[stackPointer-1];
		free(pathStack[stackPointer-1]);
		stackPointer--;
		return val;
	}
	// exit on negative index
	exit(10);
}

/** _clearStack: clear the stack
 */
void _clearStack() {
	while (stackPointer != 0) {
		_pop();
	}
}

/** _clearCopyStack: clear the copy stack
 */
void _clearCopyStack() {
	while (copyStackPointer != 0) {
		free(copyPathStack[copyStackPointer-1]);
		copyStackPointer--;

	}
}

/** _copyStack: copy the stack to our copy stack
 */
void _copyStack() {
	copyStackPointer = stackPointer;

	int i = 0;
	for (i; i < stackPointer; i++) {

		char* copy = malloc(sizeof(char)* INT_100);
		strcpy(copy, pathStack[i]);
		copyPathStack[i] = copy;
	}
}

/** _replaceStack: replace the stack with our copy stack
 */
void _replaceStack() {
	_clearStack();

	stackPointer = copyStackPointer;
	int i = 0;
	for (i; i < stackPointer; i++) {
		char* copy = malloc(sizeof(char)* INT_100);
		strcpy(copy, copyPathStack[i]);
		pathStack[i] = copy;
	}
}

/** _printStack: print a stack
 * @param c - the stack (an array of strings) to print
 * @param sz - the size of the stack (array) to print
 */
void _printStack(char ** c, int sz) {
	int i = 0;
	for (i; i< sz; i++) {
		printf("Stack: %s at %d\n", c[i], i);
	}
}

/** _fillPathStack: populate our stack with the current path
 * @param c - the array to print
 * @param sz - the size of the array to print
 */
void _fillPathStack() {
	char util[SIZE];
	getcwd(util, SIZE);

	char* token;
	token = strtok(util, "/");
	while (token != NULL) {
		_push(token);
		token = strtok(NULL, "/");
	}
}



/************************
 *	String Helpers
 *
 *	- findFirst
 *	- find
 *	- countChar
 *	- shiftString
 *	- stripSpaces
 *	- stripEndOfLine
 *	- stripEndSpace
 *	- stripNewline
 *	- stripString
 *	- nextNonSpaceChar
 *	- lastChar
 ************************/

/** find: find the first character in a string
 * @param c - String to search
 * @param x - specified character
 * @return the first index that contains the specified
 *		   character or -1 if the character is not found
 */
int findFirst(char * c, char x) {
	int i = 0;
	while (c[i] != CHAR_NULL) {
		if (c[i] == x) {
			return i;
		}
		i++;
	}
	return -1;
}

/** find: find the first character in a string, given a
 * 		  start index
 * @param c - String to search
 * @param x - specified character
 * @param i - starting index
 * @return the first index after the start index that
 *		   contains the specified character or -1 if the
 *		   character is not found
 */
int find(char * c, char x, int i) {
	while (c[i] != CHAR_NULL) {
		if (c[i] == x) {
			return i;
		}
		i++;
	}
	return -1;
}

/** countChar: count the number of times a particular character
 * 			   shows up in a string
 * @param c - String to search
 * @param x - specified character
 * @return number of times the specified character appears
 * 		   in the string
 */
int countChar(char * c, char x) {
	int count = 0;
	int i = 0;
	while (c[i] != CHAR_NULL) {
		if (c[i] == x) {
			count++;
		}
		i++;
	}
	return count;
}

/** shiftString: shifts a string left by striping characters
 *				 from the front of the array
 * @param c - String to shift
 * @param offset - the number of spaces to shift the string
 * @param sz - the size of the string
 */
void shiftString(char * c, int offset, int sz) {
	int i = 0;
	while (i < sz && c[i]) {
		c[i] = c[offset];
		i++;
		offset++;
	}
}

/** stripSpaces: removes whitespace from a string
 * @param c - String to strip
 */
void stripSpaces(char * c) {
	char *space = " ";
	if (strncmp(space, c, 1) == 0) {
		int i  = 0;
		while (c[i] == CHAR_SPACE) {
			i++;
		}
		shiftString(c, i, SIZE);
	}
}

/** stripEndOfLine: removes the last character from a string
 * @param c - String to strip
 */
void stripEndOfLine(char * c) {
	int len = strlen(c);
	printf("length is: %d\n",(int)strlen(c));
	c[len-1] = '\0';
}

/** stripEndSpace: removes whitespace from the end of a string
 * @param c - String to strip
 */
void stripEndSpace(char * c) {
	int len = strlen(c);
	while(len > 1) {
		char cur = c[len-1];
		if (cur == ' ' || cur == '\n')
			c[len-1] = '\0';
		else
			break;
		len--;
	}
}

/** stripNewline: removes all new line characters from a string
 * @param c - String to strip
 */
void stripNewline(char * c) {
	int i = 0;
	while (c[i] != CHAR_NULL) {
		if (c[i] == '\n') {
			c[i] = '\0';
		}
		i++;
	}
}

/** stripString: strips spaces and the new line characters
 * 				 from a string
 * @param c - String to strip
 */
void stripString(char * c) {
	stripSpaces(c);
	stripEndSpace(c);
}

/** nextNonSpaceChar: return the next non-space character
 * 					  from a string
 * @param c - String to strip
 * @param idx - start index
 * @return the index of next non-space character
 */
int nextNonSpaceChar(char * c, int idx) {
	idx++;
	while (idx < strlen(c) && c[idx] == CHAR_SPACE) {
		idx++;
	}
	return idx-1;
}

/** lastChar: find the last non-null and non-endline
 * 			  character from a string
 * @param c - String to search
 * @return the index of the last non-null, non-endline
 * 		   char in the string
 */
int lastChar(char * c) {

	int idx = 0;
	while (c[idx] != '\0' && c[idx] != '\n') {
		idx++;
	}
	return idx-1;
}



/************************
 *	Other Helpers
 *
 *	- _printArray
 *	- throwError
 ************************/

/** _printArray: print an array of strings
 * @param c - the array to print
 * @param sz - the size of the array to print
 */
void _printArray(char ** c, int sz) {
	int i = 0;
	for (i; i< sz; i++) {
		printf("Parsed tokens: %s at %d\n", c[i], i);
	}
}

/** throwError: throw an error
 * @return ERROR_CODE (should be -1)
 */
int throwError() {

	char * error_message = "An error has occurred\n";
	write(STDERR_FILENO, error_message, strlen(error_message));
	return ERROR_CODE;
}


/************************
 *	Command Helpers
 *	- changeDirectory
 *	- parse
 ************************/


/** changeDirectory: handle the change directory command
 * @param c - user input (beginning with "cd ")
 */
void changeDirectory(char * c) {
	int result;
	char path[SIZE];
	memset(&path[0], 0, sizeof(path));

	int i = 2; //jump to beginning of input path
	while (c[i] == CHAR_SPACE) {
		i++;
	}

	// changing to home, means user only wrote "cd"
	if (c[i] == 0 || c[i] == '\n') {

		char* homePath = getenv("HOME");
		result = chdir(homePath);

		if (result == 0) {//chdir worked

			_clearStack();
			char* homeTokens;
			homeTokens = strtok(homePath, "/");

			//get tokens based on /
			while (homeTokens != NULL) {
				_push(homeTokens);
				homeTokens = strtok(NULL, "/");
			}
		}
	}

	//otherwise user gave more info about directory traversal
	else {

		stripNewline(c); //remove newline
		strtok(c, " "); //skip to path characters
		char * token[SIZE];
		int z = 0;
		token[z] = strtok(NULL, "/");

		//get tokens based on /
		while (token[z] != NULL) {
			z++;
			token[z] = strtok(NULL, "/");
		}

		//this means an absolute path
		if (strncmp(home, token[0], 4) == 0) {

			strcat(path, "/");
			int p = 0;
			for (p; p < z; p++) {
				strcat(path, token[p]);
				strcat(path, "/");
			}

			result = chdir(path);
			if (result == 0) {

				_clearStack();
				int v = 0;

				for (v; v < z; v++) {
					_push(token[v]);
				}
			} else {
				printf("The specified path does not exist\n");
			}


		} else {
		//means a relative path
			_copyStack(); //save the stack state of path
			bool root = false;

			int f = 0;
			for (f; f <z; f++) {
				if (strncmp(dotdot, token[f], 2) == 0) {
					if (stackPointer == 0) {
						root = true; //user tries to traverse past root
						break;
					} else {
						_pop();
					}
				} else {
					_push(token[f]);
				}
			}

			if (root == false) {
			//user has not gone past root
				strcat(path, "/");
				int p = 0;
				for (p; p < stackPointer; p++) {
					strcat(path, pathStack[p]);
					strcat(path, "/");
				}

				result = chdir(path);
				if (result != 0) {
					printf("The path specified does not exist\n");
					_replaceStack();

				}
			} else {
			//user goes past root, in command, so reload stack
				printf("Cannot move above root\n");
				_replaceStack(); //return stack to previous state, since user messed up
			}
		}
	}
}

/** parse: parse user input and call the appropriate commands
 * @param input - user input (beginning with "cd ")
 * @return ERROR_CODE if anything throws an error
 */
int parse(char * input) {

	// helpers
	char util[SIZE];
	int savedSTDOUT = -1;

	// grab command
	strcpy(util, input);
	char * ptr = strtok(util, "> ");
	char * cmd = ptr;

	/* builtin: '!!' and '!n' */

	// '!!' run last command
	if (strncmp(bangbang, cmd, 2) == 0) {
		strcpy(input, historyArr[historyCount-1]);
	}

	// '!n' run nth command in history
	else if (strncmp(bang, cmd, 1) == 0) {

		int len = strlen(cmd);

		// if we get more than just '!'
		if (len != 1) {
			int ind = 1;
			long n = atoi(&cmd[ind]); // # of the entry requested

			// if the entry # is valid and within the last 10
			if (n < historyCount && n > historyCount-10) {
				strcpy(input, historyArr[n]);
				puts(input);
			}

			// the entry # is not valid
			else { return throwError(); }
		}
		// we only got a '!'
		else { return throwError(); }
	}

	// add last command to history
	_addToHistory(input);


	/* checking for redirection */

	int arrCount = countChar(input, '>');

	// if there's a redirection
	if (arrCount > 0) {
		char * out = NULL;
		int args = 0;

		// while there are args
		while (ptr != NULL) {
			// more than one '>'
			if (arrCount > 1) { return throwError(); }
			// first argument
			else if (args == 0) { cmd = ptr; }
			// second argument
			else if (args == 1) { out = ptr; }
			// more than two args
			else { return throwError(); }

			// increment
			ptr = strtok(NULL, "> ");
			args++;
		}

		// check for the correct # of matched arguments
		// or the '>' comes after the two args
		if ((arrCount+1 != args) ||
			(strlen(out) + strlen(cmd) < findFirst(input, '>'))) {
			return throwError();
		}

		// set redirect for outfile
		int outfile = open(out, O_RDWR|O_CREAT|O_TRUNC, S_IXUSR|S_IXUSR|S_IRUSR);
		savedSTDOUT = dup(STDOUT_FILENO);
		dup2(outfile, STDOUT_FILENO);
		close(outfile);
	}

	// recopy util
	strcpy(util, input);
	cmd = strtok(util, " ");

	/* remaining builtins */

	// cd
	if (strcmp(cd, cmd) == 0) {
		changeDirectory(input);
	}

	// print working directory
	else if (strcmp(pwd, cmd) == 0) {
		memset(&util[0], 0, sizeof(util));
		getcwd(util, SIZE);
		strcat(util, "\n");
		write(STDOUT_FILENO, (void*) util, sizeof(util));
	}

	// history
	else if (strcmp(history, cmd) == 0) {
		printHistory();
	}

	// wait
	else if (strcmp(_wait, cmd) == 0) {
		wait(0);
	}

	// exit
	else if (strcmp(ciao, cmd) == 0) {
		_clearStack();
		_clearCopyStack();
		exit(0);
	}

	// if STDOUT was redirected, reset it back to screen
	if (savedSTDOUT != -1) {
		dup2(savedSTDOUT, STDOUT_FILENO);
	}

	return 0;
}


/** main: initialize our shell, and loop until user specifies
 *		  exit with the appropriate commands.
 * @param c - user input (beginning with "cd ")
 */
int main(int argc, char * argv[]) {

	char input[SIZE];
	int fk = 0;
	_fillPathStack();

	// loop until user exits
	while (1) {

		printf("CShell> ");
		fgets(input, SIZE, stdin);
		stripString(input);

		int lastcharidx = lastChar(input);
		if (lastcharidx >= 0) {
			if (input[lastcharidx] == '&') {
				fk = fork();
			}
		}

		//fork failed; exit
		if (fk < 0) {
			fprintf(stderr, "fork failed\n");
			exit(1);
		}

		// child process: runs bash
		else if (fk == 0) {
			int result = parse(input);
			if (result == ERROR_CODE) {
				continue;
			}
		}

		//parent does nothing
		else {}
	}

	return 0;
}
