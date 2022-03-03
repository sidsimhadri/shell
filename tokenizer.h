#ifndef _TOKENIZER_H
#define _TOKENIZER_H

#include <limits.h>
#include "vect.h"

/** tokenizes a char input (typically from stdin) */
vect_t *tokenize(char* line); 

#endif /* ifndef _TOKENIZER_H */

