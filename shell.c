#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// constants
const char* home = "home";
const char* dotdot = "..";
const char *ciao = "exit";
const char *cd = "cd";
const char *pwd = "pwd";
const char *_wait = "wait";
const int SIZE = 512;
const int CHAR_SPACE = 32;
const int CHAR_NEWLINE = 13;
const int CHAR_NULL = 0;
char* pathStack[50];
char* copyPathStack[50];
int copyStackPointer = 0;
int stackPointer = 0;

typedef enum {true, false} bool;

void push(char* c){
	if(stackPointer < SIZE){

		char *util = malloc(sizeof(char)* 100);
		strcpy(util, c);
		pathStack[stackPointer] = util;
		stackPointer++;
	}
}

void pop(){
	if(stackPointer-1 >= 0){
		free(pathStack[stackPointer-1]);
		stackPointer--;
	} else {
		exit(10);
	}
}

void clearStack(){

	while(stackPointer != 0){
		pop();
	}
}

void clearCopyStack(){

	while(copyStackPointer != 0){
		free(copyPathStack[copyStackPointer-1]);
		copyStackPointer--;

	}
}

void copyStack(){

	copyStackPointer = stackPointer;

	int i = 0;
	for(i; i < stackPointer; i++){

		char* copy = malloc(sizeof(char)* 100);
		strcpy(copy, pathStack[i]);
		copyPathStack[i] = copy;
	}

}

void replaceStack(){

	clearStack();

	stackPointer = copyStackPointer;
	int i = 0;
	for(i; i < stackPointer; i++){

		char* copy = malloc(sizeof(char)* 100);
		strcpy(copy, copyPathStack[i]);
		pathStack[i] = copy;
	}
}

void printArray(char ** c, int size){

	int i = 0;
	for(i; i< size; i++){

		printf("parsed tokens: %s at %d\n", c[i], i);
		
	}
}

void printStack(char ** c, int size){

	int i = 0;
	for(i; i< size; i++){

		printf("stack: %s at %d\n", c[i], i);
		
	}
}

void fillPathStack(){

	char util[SIZE];
	getcwd(util, SIZE);

	char* token;
	token = strtok(util, "/");
	while(token != NULL) {
		push(token);
		token = strtok(NULL, "/");
	} 

}

void stripNewline(char * c){

	int i = 0;
	char util[SIZE];
	while(c[i] != CHAR_NULL){
		if (c[i] == '\n') {
			c[i] = '\0';
		}
		i++;
	}
}

/*
void condensePath(char * c){

	int i = 0;
	char util[SIZE];
	while(c[i] != CHAR_NULL){
		if (c[i] == '\n') {
			c[i] = '\0';
		}
		i++;
	}
}
*/


void shiftString(char * c, int offset, int sz) {
	int i = 0;
	while(i < sz && c[i]) {
		c[i] = c[offset];
		i++;
		offset++;
	}
}

void stripString(char * c) {
	char *space = " ";
	if (strncmp(space, c, 1) == 0) {
		int i  = 0;
		while(c[i] == CHAR_SPACE) {
			i++;
		}
		shiftString(c, i, SIZE);
	}
}

void _changeDirectory(char * c) {
	puts(c);
	int result;
	char path[SIZE];
	memset(&path[0], 0, sizeof(path));


	int i = 2;//jump to beginning of input path
	while (c[i] == CHAR_SPACE) {
		i++;
	}

	// changing to home, means user only wrote "cd"
	if (c[i] == 0 || c[i] == '\n') {

		char* homePath = getenv("HOME");
		printf("homePath is %s\n", homePath );
		result = chdir(homePath);

		if (result == 0){//chdir worked

			clearStack();
			char* homeTokens;
			homeTokens = strtok(homePath, "/");

			//get tokens based on /
			while(homeTokens != NULL) {
				push(homeTokens);
				homeTokens = strtok(NULL, "/");
			} 
		}	

	} else {//otherwise user gave more info about directory traversal

		stripNewline(c);//remove newline
		strtok(c, " ");//skip to path characters
		char * token[SIZE];
		int z = 0;
		token[z] = strtok(NULL, "/");

		//get tokens based on /
		while(token[z] != NULL) {
			z++;
			token[z] = strtok(NULL, "/");
		} 

		printArray(token, z);

		//this means an absolute path
		if(strncmp(home, token[0], 4) == 0){

			strcat(path, "/");
			int p = 0;
			for(p; p < z; p++){
				strcat(path, token[p]);
				strcat(path, "/");
			}

			result = chdir(path);
			if(result == 0){

				clearStack();
				int v = 0;

				for(v; v < z; v++){
					push(token[v]);
				}
			} else {
				printf("the specified path does not exist\n");
				printStack(pathStack, stackPointer);
			}


		}else{//means a relative path
			printStack(pathStack, stackPointer);
			copyStack();
			bool root = false;

			int f = 0;
			for(f; f <z; f++){
				if(strncmp(dotdot, token[f], 2) == 0){
					if(stackPointer == 0){
						root = true; //user tries to traverse past root
						break;
					} else {
						pop();
					}
				} else {
					push(token[f]);
				}
			}

			if(root == false){//user has not gone past root
				strcat(path, "/");
				int p = 0;
				for(p; p < stackPointer; p++){
					strcat(path, pathStack[p]);
					strcat(path, "/");
				}

				puts(path);

				printStack(pathStack, stackPointer);

				result = chdir(path);
				if(result != 0){
					printf("The path specified does not exist\n");
					replaceStack();
					printStack(pathStack, stackPointer);

				}
			} else {//user goes past root, in command, so reload stack
				printf("cannot move above root\n");
				replaceStack();
				printStack(pathStack, stackPointer);

			}




		}	
		
		//puts(tok);
		//chdir(tok);
	}

}

// void changeDirectory(char * c) {
// 	printf("It's in here\n");
// 	char * t;
// 	char * path;
// 	const char s[2] = " ";
// 	printf("setting t\n");
// 	t = strtok(c, s);
// 	while (t) {
// 		printf("in while\n");
// 		printf("t is %s\n", t);
// 		printf("path is %s\n", path);
// 		strcat(path, t);
// 		t = strtok(NULL, s);
// 	}
// 	printf("path is %s\n", path);
// 	puts(path);
// }


int main(int argc, char * argv[]){

	// helper arrays
	char input[SIZE];
	char util[SIZE];

	fillPathStack();

	// built in commands

	while(1){


	printf("CShell> ");
	fgets(input, SIZE, stdin);

	stripString(input);

	if (strncmp(cd, input, 2) == 0) {
		_changeDirectory(input);
	}

	// print working directory
	else if (strncmp(pwd, input, 3) == 0) {
		getcwd(util, SIZE);
		puts(util);
	}

	// exiting
	else if (strncmp(ciao, input, 4) == 0) {
		clearStack();
		clearCopyStack();
		exit(0);
	}

	}

	return 0;
}
