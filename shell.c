#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char * argv[]){

   // helper arrays
   char input[512];
   char util[512];

   // built in commands
   char *ciao = "exit";
   char *cd = "cd";
   char *pwd = "pwd";
   char *wait = "wait";

   while(1){

	printf("CShell> ");
	fgets(input, 512, stdin);

	if (strncmp(cd, input, 2) == 0) {
		printf("changing directory\n");
	}

	// print working directory
	else if (strncmp(pwd, input, 3) == 0) {
		getcwd(util, 512);
		puts(util);
	}

	// exiting
	else if (strncmp(ciao, input, 4) == 0) {
		exit(0);
	}

   }

   return 0;
}
