#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char * argv[]){

   char input[100];
   char *exit = "exit";
   char *blank = "\n";
   int x = 1;
   while(x){

	printf("cz> ");
	fgets(input, 100, stdin);
	
	x = strncmp(blank, input, 1);

	if(x != 0){
		fputs(input, stdout);
 	}
	x = strncmp(exit, input, 4);
   }

   return 0;
}
