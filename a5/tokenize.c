#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vect.h"
#include "tokenizer.h"

int main(int argc, char **argv) {
  
  char line[256];
  char* str = fgets(line, 256, stdin);
  vect_t *v = tokenize(str);

  for(int i = 0; i < vect_size(v); i++) {
    printf("%s\n", vect_get(v, i));
  }

  vect_delete(v);
}
