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
const char* history = "history";
const char* bangbang = "!!";
const char* bang = "!";
const char error_message[30] = "An error has occurred\n";
const int SIZE = 512;
const int INT_100 = 100;
const int CHAR_SPACE = 32;
const int CHAR_NEWLINE = 13;
const int CHAR_NULL = 0;
char* pathStack[50];
const int PATHSTACK_SIZE = 50;
char* copyPathStack[50];
int copyStackPointer = 0;
int stackPointer = 0;
char* historyArr[200];
const int HISTORYARR_SIZE = 200;
int historyCount = 0;

typedef enum {true, false} bool;

void printHistory(){

	int i = 10;
	while(i > 0){
		if(historyCount >= i){
			printf("%d %s\n", historyCount-i, historyArr[historyCount-i] );
		}
		i--;
	}

}

void addToHistory(char* c){

	if(historyCount == HISTORYARR_SIZE){
		//if history array is full, remove the first 20 and remake array
		int i = 0;
		for(i;i<20;i++){
			free(historyArr[i]);
		}
		for(i;i<HISTORYARR_SIZE;i++){
			historyArr[i-20] = historyArr[i];
		}
		historyCount = 180;

	}

	if(historyCount < HISTORYARR_SIZE){
		char *util = malloc(sizeof(char)* INT_100);
		strcpy(util, c);
		historyArr[historyCount] = util;
		historyCount++;
	} 
}

void clearHistory(){

	while(historyCount != 0){
		free(historyArr[historyCount-1]);
		historyCount--;
	}
}

void push(char* c){
	if(stackPointer < PATHSTACK_SIZE){

		char *util = malloc(sizeof(char)* INT_100);
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

		char* copy = malloc(sizeof(char)* INT_100);
		strcpy(copy, pathStack[i]);
		copyPathStack[i] = copy;
	}

}

void replaceStack(){

	clearStack();

	stackPointer = copyStackPointer;
	int i = 0;
	for(i; i < stackPointer; i++){

		char* copy = malloc(sizeof(char)* INT_100);
		strcpy(copy, copyPathStack[i]);
		pathStack[i] = copy;
	}
}

void printArray(char ** c, int size){

	int i = 0;
	for(i; i< size; i++){

		printf("Parsed tokens: %s at %d\n", c[i], i);
		
	}
}

void printStack(char ** c, int size){

	int i = 0;
	for(i; i< size; i++){

		printf("Stack: %s at %d\n", c[i], i);
		
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
	while(c[i] != CHAR_NULL){
		if (c[i] == '\n') {
			c[i] = '\0';
		}
		i++;
	}
}

int countChar(char * c, char charToCount){
	int count = 0;
	int i = 0;
	while(c[i] != CHAR_NULL){
		if (c[i] == charToCount) {
			count++;
		}
		i++;
	}
	return count;
}

void shiftString(char * c, int offset, int sz) {
	int i = 0;
	while(i < sz && c[i]) {
		c[i] = c[offset];
		i++;
		offset++;
	}
}

//removes whitespace
void stripSpaces(char * c) {
	char *space = " ";
	if (strncmp(space, c, 1) == 0) {
		int i  = 0;
		while(c[i] == CHAR_SPACE) {
			i++;
		}
		shiftString(c, i, SIZE);
	}
}

void stripEndOfLine(char * c) {
	int len = strlen(c);
	printf("length is: %d\n",(int)strlen(c));
	c[len-1] = '\0';
}

void stripString(char * c) {
	stripSpaces(c);
	stripNewline(c);
}

int nextNonSpaceChar(char * c, int idx){

	idx++;
	while(idx < strlen(c) && c[idx] == CHAR_SPACE){

		idx++;
	}

	return idx;
}

void changeDirectory(char * c) {
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
		puts(homePath);
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
				printf("The specified path does not exist\n");
			}


		} else {
		//means a relative path
			copyStack();//save the stack state of path
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

			if(root == false){
			//user has not gone past root
				strcat(path, "/");
				int p = 0;
				for(p; p < stackPointer; p++){
					strcat(path, pathStack[p]);
					strcat(path, "/");
				}

				result = chdir(path);
				if(result != 0){
					printf("The path specified does not exist\n");
					replaceStack();

				}
			} else {
			//user goes past root, in command, so reload stack
				printf("Cannot move above root\n");
				replaceStack();//return stack to previous state, since user messed up
			}
		}	
	}
}


int main(int argc, char * argv[]){

	// helper arrays
	char input[SIZE];
	char util[SIZE];

	fillPathStack();

	char* hello = "ab    cd ef gh";
	int idx = 0;
	idx = nextNonSpaceChar(hello, idx);
	printf("idx %d\n", idx);

	// built in commands

	while(1){


		printf("CShell> ");
		fgets(input, SIZE, stdin);

		stripString(input);

		//handle history commands
		//want to rerun last command
		if(strncmp(bangbang, input, 2) == 0){

			strcpy(input, historyArr[historyCount-1]);
		}//want to rerun n command
		else if(strncmp(bang, input, 1) == 0){

			int len = strlen(input);
			if(len != 1){
				int ind = 1;
				long total = atoi(&input[ind]);
				if(total < historyCount && total > historyCount -10){
					strcpy(input, historyArr[total]);
					puts(input);
				} else {
					write(STDERR_FILENO, error_message, strlen(error_message));
					continue;
				}
			} else {
				write(STDERR_FILENO, error_message, strlen(error_message));
				continue;
			}
		}

		addToHistory(input);

		// char* ret = strtok(input, ">");
		// // puts(ret);
		// char* redir = strtok(NULL, ">");
		// if (redir != NULL) {
		// 	stripSpaces(redir);
		// 	puts(redir);
		// 	redir = strtok(redir, " ");
		// 	puts(redir);
		// }

		//should only be one redirection >
		strcpy(util, input);


		// char* ret = strtok(util, ">");
		// printf("command it %s\n", ret);	
		// printf("original line is %s\n", input);
		// if( strncmp(ret, input, strlen(input)) != 0 ){//means there is a redirection
		// 	//if second redirect present, then error
		// 	char* redirDest = strtok(NULL, ">");
		// 	char* check2rets = strtok(NULL, ">");
		// 	if(strlen(ret)+strlen(redirDest) + 1 < strlen(input)){
		// 		write(STDERR_FILENO, error_message, strlen(error_message));
		// 		continue;
		// 	}
		// 	stripSpaces(redirDest);
		// 	printf("redirection destination is %s\n", redirDest);

		// 	puts(redirDest);
			
		// 	// memset(&util[0], 0, sizeof(util));
		// 	strcpy(util,redirDest);
		// 	char* splitDest = strtok(util, " ");
		// 	splitDest = strtok(NULL, " ");
		// 	printf("splitDest %s\n", splitDest);
		// 	printf("whole redirection: %s\n", redirDest);
		// 	printf("length of whole redirection %d\n", (int)strlen(redirDest));
		// 	printf("split redirection: %s\n", splitDest);
		// 	printf("length of split redirection %d\n", (int)strlen(splitDest));

		// 	// strlen(splitDest) != strlen(redirDest)
		// 	// if(strlen(splitDest) != strlen(redirDest)) {
		// 	// 	write(STDERR_FILENO, error_message, strlen(error_message));
		// 	// 	continue;
		// 	// }
		
		// }

		if (strncmp(cd, input, 2) == 0) {
			changeDirectory(input);
		}

		// print working directory
		else if (strncmp(pwd, input, 3) == 0) {
			memset(&util[0], 0, sizeof(util));
			getcwd(util, SIZE);
			puts(util);
		}

		else if(strncmp(history, input, 7) == 0){
			printHistory();

		}

		else if(strncmp(_wait, input, 4) == 0){
			wait(0);
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
