#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// constants
const int SIZE = 512;
const int CHAR_SPACE = 32;
const int CHAR_NEWLINE = 13;
const int CHAR_NULL = 0;

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

void printArray(char ** c, int size){

	int i = 0;
	for(i; i< size; i++){

		printf("%s at %d\n", c[i], i);
		
	}
}


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
	
	int i = 2;
	while (c[i] == CHAR_SPACE) {
		i++;
	}
	//printf("i is %d\n", i);
	//printf("c[i] is %c\n", c[i]);
	//printf("c is %s\n", c);

	// changing to home
	if (c[i] == 0 || c[i] == '\n') {
		puts(getenv("HOME"));
		chdir(getenv("HOME"));
	} else {

		stripNewline(c);//remove newline
		strtok(c, " ");//skip to path characters
		char * token[SIZE];
		int z = 0;
		token[z] = strtok(NULL, "/");

		while(token[z] != NULL) {
			z++;
			token[z] = strtok(NULL, "/");
		} 

		//printArray(token,z);
		puts(token[0]);
		char* home = "home";
		//this means an absolute path
		if(strncmp(home, token[0], 4) == 0){

			printf("match!\n");
			printf("creating path\n");
			char * path;
			// path = '\0';
			printf("printing path\n");
			printf("%s\n", path);

			int p = 0;
			for(p; p < z; p++){
				printf("for loop\n");
				puts(path);
				printf("concat 1\n");
				strcat(path, token[p]);
				printf("concat 2\n");
				strcat(path, "/");
			}

			printf("shift string\n");
			shiftString(path, 2, SIZE);
			printf("printing\n");

			puts(path);

		}else{

			printf("No match!!!!!!!!!!!!! fix this\n");


		}	
		
		//puts(tok);
		//chdir(tok);
	}
}

void changeDirectory(char * c) {
	printf("It's in here\n");
	char * t;
	char * path;
	const char s[2] = " ";
	printf("setting t\n");
	t = strtok(c, s);
	while (t) {
		printf("in while\n");
		printf("t is %s\n", t);
		printf("path is %s\n", path);
		strcat(path, t);
		t = strtok(NULL, s);
	}
	printf("path is %s\n", path);
	puts(path);
}


int main(int argc, char * argv[]){

	// helper arrays
	char input[SIZE];
	char util[SIZE];

	// built in commands
	char *ciao = "exit";
	char *cd = "cd";
	char *pwd = "pwd";
	char *wait = "wait";

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
		exit(0);
	}

	}

	return 0;
}
