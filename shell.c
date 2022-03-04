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

// declaration for shell to avoid errors during recursion
int shell(vect_t *args, vect_t *last);
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

// cd
int cd (vect_t *args) {
    if (vect_size(args) == 1) {
	return(chdir(getenv("HOME")));
    } else {
	return(chdir(vect_get(args, 1)));
    }
}
// COMMENTED OUT BECAUSE IT CAUSES TIMEOUT
int source (vect_t *args) {
// FILE* f = fopen(vect_get(args, 1), "r");
//
//    char line[max];
//    if (f)
//    {
//        while (1)
//        {
//            char* c = NULL;
//            while ((c = fgets(line, max, f)) != NULL)
//            {
//                if (line[0] == '\n')
//                {
//                    break;
//                }
//                shell(tokenize(args), NULL);
//            }
//
//            if (c == NULL)
//                break;
//        }
//
//        fclose(f);
//    }
//    else
//    {
//        printf("file not found");
//    }
    printf("filenotfound");

}

// runs shell with last args
int prev (vect_t *args, vect_t *last) {
  shell(last, last);
}

// prints help for all built ins
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
int shell(vect_t *args, vect_t *last) {
  // the if statements check for builtins
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
  // otherwise fork the child processes
  //forking logic
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

	// start handling semicolons here
	
	 vect_t *tmp = vect_new();

	 for(int i = 0; i < vect_size(args); i++) {
	   if(strcmp(vect_get(args, i), ";") == 0) { // if the thing at i is a semi colon
	     shell(tmp, last_args); // run the shell with the commands we have
	     int j = vect_size(tmp);
	     while (j != 0) {
	       vect_remove_last(tmp);
	       j--;
	     }
           } else {
	     vect_add(tmp, vect_get(args, i));
	   }
	 }

         // vect_print(last_args);
         shell(tmp, last_args); // run the shell
	 last_args = tmp;
      }
    }
    return 0;
}
/**
THIS IS WHAT WE TRIED TO DO FOR ASSIGNMENT 6, BUT WE COULDN'T FIGURE OUT WHY IT WAS EXITING AFTER EACH COMMAND WHICH
WAS CAUSING WIDESPREAD TEST FAILURES
#define max 255
int shell(vect_t *args, vect_t *colon, vect_t *last, char *infile, char *outfile);
void processargs(vect_t *args, vect_t *last_args, char *in, char *out);

void strip(char * str, char c){
    int i, j;
    int len = strlen(str);
    i = 0;
    while(i<len)
    {
        if(str[i] == c)
        {
            for(j=i; j<len; j++)
            {
                str[j] = str[j+1];
            }
            len--;
            i--;
        }
        i++;
    }

}

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
	    shell(args, NULL, tokenize(last), NULL, NULL);
    }

    fclose(f);
    exit(EXIT_SUCCESS);
}

int prev (vect_t *args, vect_t *last) {
  shell(last, NULL, last, NULL, NULL);
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
int shell(vect_t *args, vect_t *args2, vect_t *last, char *infile, char *outfile) {
  // if we want to exit the shell then do

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
        if(args2) {
            processargs(args2, args, NULL, NULL);
        }
  }
}
int piper(vect_t *args1, vect_t *args2, vect_t *last, char *in, char *out) {
    int pipe_fds[2]; // the pipe system call creates two file descriptors in the 2-element
                     // array given as argument

    assert(pipe(pipe_fds) == 0); // returns 0 on success

    int read_fd = pipe_fds[0]; // index 0 is the "read end" of the pipe
    int write_fd = pipe_fds[1]; // index 1 is the "write end" of the pipt

    int child_pid = fork();

    if (child_pid > 0) { // in parent
      close(read_fd); // close the other end of the pipe

      // replace stdout with the write end of the pipe
      if (close(1) == -1) {
        perror("Error closing stdout");
        exit(1);
      }
      assert(dup(write_fd) == 1);

      execute(args1);
    }
    else if (child_pid == 0) { // in child
      close(write_fd); // close the other end of the pipe

      // replace stdin with the read end of the pipe
      if ( close(0) == -1) {
        perror("Error closing stdin");
        exit(1);
      }
      assert(dup(read_fd) == 0);

      execute(args2);
    }
    else {
      perror("Error - fork failed");
      exit(1);
    }

}
// handles semicolons, etc
void processargs(vect_t *args, vect_t *last_args, char *in, char *out) {
	 vect_t *tmp = vect_new();
	 vect_t *tmp2 = vect_new();
	 for(int i = 0; i < vect_size(args); i++) {
        if(strcmp(vect_get(args, i), "<") == 0) { // if the thing at i is a semi colon
          shell(args, NULL, last_args, vect_get(args, i + 1), out);
          i++;
        }

        if(strcmp(vect_get(args, i), ">") == 0) { // if the thing at i is a semi colon
          shell(args, NULL, last_args, in, vect_get_copy(args, i + 1));
          i++;
        }
	   if(strcmp(vect_get(args, i), ";") == 0) { // if the thing at i is a semi colon
         vect_add(tmp, vect_get(args, i));
         shell(tmp, tmp2, last_args, in, out);
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
          shell(args, NULL, last_args, in, out);
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
          // get input from stdin
         if(fgets(commands, max, stdin) == NULL) { // if we dont have a line then we're done
           printf("\nBye bye.\n"); // print bye bye on a new line
           exit(0); // exit
         }
         args = tokenize(commands); // tokenize the commands
           if(strcmp(vect_get(args, 0), "exit") == 0) {
             printf("Bye bye.\n");
             exit(0);
           }
           else if(strcmp(vect_get(args, 0), "cd") == 0) {
              return cd(args);
           }
           else if(strcmp(vect_get(args, 0), "source") == 0) {
             return source(args);
           }
           else if(strcmp(vect_get(args, 0), "prev") == 0) {
             return prev(args, NULL);
           }
           else if(strcmp(vect_get(args, 0), "help") == 0) {
             return help(args);
           }
           else {
         int i = 0;
          while (i < vect_size(args)) {
             strip(vect_get(args, i),"\n" );
             i++;
             }

	      processargs(args, last_args, NULL, NULL);
	      }
      }
    }
    return 0;
}
**/
