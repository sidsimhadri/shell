#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "tokenizer.h"
#include "vect.h"

// determines if a given char is special
int isSpecial(char c) {
  return ((c == ';') || (c == '<') || (c == '>') || (c == '(') || (c == ')') || (c == '|')) ? 1 : 0;
}

void removeSubstr (char *string, char *sub) {
    char *match;
    int len = strlen(sub);
    while ((match = strstr(string, sub))) {
        *match = '\0';
        strcat(string, match+len);
    }
}

// handles interpreting a portion of the stdin line a const char 
// c and then interperets the special token at the index
char* read_special(const char* c, int idx) {
  char* token = malloc(2);
  memcpy(token, c + idx, 1);
  token[1] = 0;
  return token;
}

// handles reading / tokenizing a string and returns said string as a token
char* read_string(const char* c, int idx) {
  int counter = 1;
  while(c[idx + counter] != '\"') {
    counter++;
  }
  char* token = malloc(counter + 1);
  memcpy(token, c + idx + 1, counter);
  token[counter] = 0;
  return token;
}

// handles reading regular arguments that arent strings or special characters.
void del(char str[], char ch) {
   int i, j = 0;
   int size;
   char ch1;
   char str1[255];
 
   size = strlen(str);
 
   for (i = 0; i < size; i++) {
      if (str[i] != ch) {
         ch1 = str[i];
         str1[j] = ch1;
         j++;
      }
   }
   str1[j] = '\0';
}
// it then tokenizes what it reads from c up until the next space, double quote, or special character
char* read_else(const char* c, int idx) {
  int counter = 0;
  // && c[idx + counter] != ' '
  //&& c[idx + counter] != '\n'
  while((isSpecial(c[idx + counter]) == 0 && c[idx + counter] != '\"' && c[idx + counter] != ' ' )) {
    counter++;
  }
  char* token = malloc(counter + 1);
  memcpy(token, c + idx, counter);
  token[counter] = 0;
  return token;
}
// tokenizes a line (given presumably by stdin) and returns a vector of each of the individual tokens.
vect_t* tokenize(char* line) {
  del(line, '\n');	
  vect_t *v = vect_new(); // create a new vector

  int i = 0;
  while(i < strlen(line) - 1) { // while we're still tokenizing the stdin line
    if(' ' == line[i] || '\n' == line[i]) { // if we immediately hit a space
     i++; // jump over it
     continue;
    }
    if(isSpecial(line[i])) { // if a character is special
      char *rspec = read_special(line, i); // convert the special character to a token
      vect_add(v, rspec); // add the token to our vector to return
      i++; // increment i by one so we move past said char
      free(rspec); // free our temp variable
      continue;
    }
    // if we have a double quote we have to read everything
    // up until the next double quote as one token
    if('\"' == line[i]) {
      char *rs = read_string(line, i); // parse the line to get the string as a token
      char sub[strlen(rs)]; // handle the off by one double quote on the end of the string
      memcpy(sub, &rs[0], strlen(rs) - 1);
      sub[strlen(rs) - 1] = '\0'; // chop off the double quote at the end
      vect_add(v, sub); // add the substringed token to the vector
      i += strlen(rs) + 1; // increment i by how long the string was so we dont catch it again.
      free(rs); // free our temporary variable
      continue;
    }
      // if we dont have a string or special char, just parse up until the next one or space
      char *relse = read_else(line, i);
      vect_add(v, relse); // add the regular token
      i += strlen(relse); // increment by how long said token was
      free(relse); // free temporary token variable
  }
  const char *el = vect_get(v, vect_size(v) - 1);
  removeSubstr(el, "\n");
  vect_remove_last(v);
  vect_add(v, el); 
  return v;
}
