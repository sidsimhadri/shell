#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include "vect.h"
#include "tokenizer.h"

#define max 255
int shell(vect_t *args, vect_t *last, char *infile, char *outfile);
void processargs(vect_t *args, vect_t *last_args, char *in, char *out);

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
    if (vect_size(args) == 1) {
	return(chdir(getenv("HOME")));
    } else {
	return(chdir(vect_get(args, 1)));
    }
}
#define linemax 256
int source (vect_t *args) {
    FILE *f = fopen(vect_get(args,1), "r");
    char *command;
    char *last = command;
    command = fscanf(f, "%[^\n]", f);
    int i;
    while (command != 0) {
        printf("shell $ %s\n", command);
        vect_t *read = tokenize(command);
        i = 0;
        while (i < vect_size(read)) {
           vect_set(read, i, strcspn(vect_get(read, i),"\n" ));
           i++;
           }
	    shell(args, tokenize(last), NULL, NULL);
    }

    fclose(f);
    exit(EXIT_SUCCESS);
}

int prev (vect_t *args, vect_t *last) {
  shell(last, last, NULL, NULL);
}


int help (vect_t *args) {
printf("cd\n"
        "This command should change the current working directory of the shell.\n"
        "Tip: You can check what the current working directory is using the pwd command (not a built-in).\n"
        "source\n"
        "Execute a script.\n"
        "Takes a filename as an argument and processes each line of the file as a command, including built-ins. In other words, each line should be processed as if it was entered by the user at the prompt.\n"
        "prev"
        "Prints the previous command line and executes it again."
        "help\n"
        "Explains all the built-in commands available in your shell\n");
}

// run the shell's child processes
int shell(vect_t *args, vect_t *last, char *infile, char *outfile) {
  // if we want to exit the shell then do
  if(strcmp(vect_get(args, 0), "exit") == 0) {
    printf("Bye bye.\n");
    exit(0);
  }
  if(strcmp(vect_get(args, 0), "cd") == 0) {
     return cd(args);
  }
  if(strcmp(vect_get(args, 0), "source") == 0) {
    source(args);
  }
  if(strcmp(vect_get(args, 0), "prev") == 0) {
    return prev(args, last);
  }
  if(strcmp(vect_get(args, 0), "help") == 0) {
    help(args);
  }
  int cpid = fork(); // fork
  int in;
  int out;
  if(cpid) {
    if(infile) {
        // Close standard in
        if (close(0) == -1) {
          perror("Error closing stdin");
          exit(1);
        }


        // Open the file for reading
        in = open(infile, O_RDONLY);

        // The open file should replace standard in
        assert(in == 0);
    }
    if(outfile) {
        // Close standard out
        if (close(1) == -1) {
          perror("Error closing stdout");
          exit(1);
        }

        // Create the file, truncate it if it exists
        out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        // The open file should replace standard out
        assert(out == 1);
    }
    //i switched execute and waitpid's location
    execute(args); // execute the arguments
                         // wait on the status to finish
  } else {
    int status;
    waitpid(cpid, &status, 0);
  }
}


int colon(vect_t *args1, vect_t *args2, vect_t *last, char *infile, char *outfile) {
       pid_t a, b;
       a = fork();
      int cpid = fork(); // fork
      int in;
      int out;
      if(a == 0) {
        if(infile) {
            // Close standard in
            if (close(0) == -1) {
              perror("Error closing stdin");
              exit(1);
            }


            // Open the file for reading
            in = open(infile, O_RDONLY);

            // The open file should replace standard in
            assert(in == 0);
        }
        if(outfile) {
            // Close standard out
            if (close(1) == -1) {
              perror("Error closing stdout");
              exit(1);
            }

            // Create the file, truncate it if it exists
            out = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

            // The open file should replace standard out
            assert(out == 1);
        }
        //i switched execute and waitpid's location
        execute(args1); // execute the arguments
                             // wait on the status to finish
      } else {
        int statusa, statusb;
        waitpid(a, &statusa, 0);
        b = fork();
        if(b == 0) {
            processargs(args2, args1, NULL, NULL);
        }
        waitpid(b, &statusb, 0);
      }
}

int piper(vect_t *args1, vect_t *args2, vect_t *last, char *in, char *out) {

    pid_t a, b;

    a = fork();

    if (a == 0) {
        int pipe_fds[2]; // the pipe system call creates two file descriptors in the 2-element
                         // array given as argument

        assert(pipe(pipe_fds) == 0); // returns 0 on success

        int read_fd = pipe_fds[0]; // index 0 is the "read end" of the pipe
        int write_fd = pipe_fds[1]; // index 1 is the "write end" of the pipe
        b = fork();
        if (b == 0) {

              close(read_fd); // close the other end of the pipe

              // replace stdout with the write end of the pipe
              if (close(1) == -1) {
                perror("Error closing stdout");
                exit(1);
              }
            execute(args1);
        }
              close(write_fd); // close the other end of the pipe

              // replace stdin with the read end of the pipe
              if ( close(0) == -1) {
                perror("Error closing stdin");
                exit(1);
              }
              execute(args2);
              int status1;
              waitpid(b, &status1, 0);

        }

        else {
            int status2;
            waitpid(a, &status2, 0);
        }

}
// handles semicolons, etc
void processargs(vect_t *args, vect_t *last_args, char *in, char *out) {
	 vect_t *tmp = vect_new();
	 vect_t *tmp2 = vect_new();
	 for(int i = 0; i < vect_size(args); i++) {
        if(strcmp(vect_get(args, i), "<") == 0) { // if the thing at i is a semi colon
          processargs(args, last_args, vect_get(args, i + 1), out);
          i++;
        }

        if(strcmp(vect_get(args, i), ">") == 0) { // if the thing at i is a semi colon
          processargs(args, last_args, in, vect_get_copy(args, i + 1));
          i++;
        }
	   if(strcmp(vect_get(args, i), ";") == 0) { // if the thing at i is a semi colon
         vect_add(tmp, vect_get(args, i));
         colon(tmp, tmp2, last_args, in, out);
         tmp = NULL;
       }
        else {
	     vect_add(tmp2, vect_get(args, i));
	   }
       if(strcmp(vect_get(args, i), "|") == 0) { // if the thing at i is a semi colon
         vect_add(tmp, vect_get(args, i));
         piper(tmp, tmp2, last_args, in, out);
         tmp = NULL;
       }
       else {
          vect_add(tmp2, vect_get(args, i));
       }


	 }
     if(tmp != NULL) {
          shell(args, last_args, in, out);
     }

}

int main(int argc, char **argv) {
    char commands[max]; // set up command line length
    vect_t *args; // initialize vector for tokens
    vect_t *last_args = vect_new();
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
//         int i = 0;
//          while (i < vect_size(args)) {
//             vect_set(args, i, strcspn(vect_get(args, i),"\n" ));
//             i++;
//             }

	      processargs(args, last_args, NULL, NULL);
      }
    }
    return 0;
}
