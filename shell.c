#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

// constants
const int SIZE = 512;
const int CHAR_SPACE = 32;

void printArray(char ** c, int size){

	int i = 0;
	for(i; i< size; i++){

		puts(c[i]);
		
	}
}

void shiftString(char * c, int offset, int sz) {
	int i = 0;
	while(i < sz) {
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

void changeDirectory(char * c) {
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
		//shiftString(c, i, SIZE);
		char * token[SIZE];
		int z = 0;
		token[z] = strtok(NULL, "/");
		while(token[z] != NULL) {
			z++;
			token[z] = strtok(NULL, "/");
		} 

		printArray(token,z);
		puts(token[0]);

		if(token[0] == "home"){

			printf("match!\n");
			char * path;
			int p = 0;
			for(p; p < z; p++){

					strcat(path, token[p]);		
			}

			puts(path);

		}else{


		}	

		//puts(tok);
		//chdir(tok);
	}
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
		changeDirectory(input);
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
