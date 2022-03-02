#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vect.h"
#include "tokenizer.h"

#define max 255

// executes the vector of arguments
int execute(vect_t *args) {
  char* arr[vect_size(args)]; // set the size of the array
    for(int i = 0; i < vect_size(args); i++) {
      //vect to array
      arr[i] = (char *) vect_get_copy(args, i); // copy over elements to the array
    }
    arr[vect_size(args)] = NULL; // set the end to null
    execvp(arr[0], arr); // execute the arguments
    if(execvp(arr[0], arr) == -1) {
      perror("didn't run"); // if -1 then we hit an error
    }
    exit(0);
}
int cd (vect_t *args) {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  return 1;
}
#define linemax 256
int source (vect_t *args) {
 FILE* f = fopen(vect_get(args, 0), "r");

    char line[linemax];
    if (f)
    {
        while (1)
        {
            char* c = NULL;
            while ((c = fgets(line, MAXBUFF, f)) != NULL)
            {
                if (line[0] == '\n')
                {
                    break;
                }
                shell(tokenize(args));
            }

            if (c == NULL)
                break;
        }

        fclose(f);
    }
    else
    {
        printf("file not found");
    }

}
int prev (vect_t *args) {
}
int help (vect_t *args) {
printf("cd\n"
        "This command should change the current working directory of the shell.\n""
        "Tip: You can check what the current working directory is using the pwd command (not a built-in).\n"
        "source\n"
        "Execute a script.\n"
        "Takes a filename as an argument and processes each line of the file as a command, including built-ins. In other words, each line should be processed as if it was entered by the user at the prompt.\n"
        "prev\n"
        "Prints the previous command line and executes it again.\n"
        "help\n"
        "Explains all the built-in commands available in your shell)\n"
}

// run the shell's child processes
void shell(vect_t *args) {
  // if we want to exit the shell then do
  if(strcmp(vect_get(args, 0), "exit") == 0) {
    printf("Bye bye.\n");
    exit(0);
  }
  if(strcmp(vect_get(args, 0), "cd") == 0) {
    cd(args);
  }
  if(strcmp(vect_get(args, 0), "source") == 0) {
    source(args);
  }
  if(strcmp(vect_get(args, 0), "prev") == 0) {
    prev(args);
  }
  if(strcmp(vect_get(args, 0), "help") == 0) {
    help(args);
  }
  // otherwise fork the child processes
  int cpid = fork(); // fork 
  if(cpid) {
    int status;
    waitpid(cpid, &status, 0); // wait on the status to finish
  } else {
    execute(args); // execute the arguments
  }
}

int main(int argc, char **argv) {
    char commands[max]; // set up command line length
    vect_t *args; // initialize vector for tokens
    if(1 == argc) { // we just have "./shell"
      printf("Welcome to mini-shell.\n");
      while(1) { // while true
         printf("shell $ "); // print shell prompt
         char* line = fgets(commands, max, stdin); // get input from stdin
         if(!line) { // if we dont have a line then we're done
           printf("\nBye bye.\n"); // print bye bye on a new line
           exit(0); // exit
         }

         args = tokenize(commands); // tokenize the commands
         shell(args); // run the shell
         vect_delete(args); // free our vector
      }
    }
    return 0;
}
