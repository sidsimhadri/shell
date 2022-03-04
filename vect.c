/**
 * Vector implementation.
 *
 * - Implement each of the functions to create a working growable array (vector).
 * - Do not change any of the structs
 * - When submitting, You should not have any 'printf' statements in your vector 
 *   functions.
 *
 * IMPORTANT: The initial capacity and the vector's growth factor should be 
 * expressed in terms of the configuration constants in vect.h
 */
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vect.h"

/** Main data structure for the vector. */
struct vect {
  char **data;             /* Array containing the actual data. */
  unsigned int size;       /* Number of items currently in the vector. */
  unsigned int capacity;   /* Maximum number of items the vector can hold before growing. */
};

/** Construct a new empty vector. */
vect_t *vect_new() {

  vect_t *v = malloc(sizeof(vect_t));
  v->size = 0;
  v->capacity = 2; // set the initial cap of the data
  v->data = malloc(sizeof(char *) * 2); // malloc appropriately 

  return v;
}

/** Delete the vector, freeing all memory it occupies. */
void vect_delete(vect_t *v) {
  assert(v != NULL);
  for (int i = 0; i < v->size; i++) {
    free(v->data[i]); // free all the data iteratively
  }
  free(v->data); // free the data
  free(v); // free the vector itself
}

/** Get the element at the given index. */
const char *vect_get(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);
  const char *tr = v->data[idx]; // set the to-return char to be the value at the index
  return tr; // return it
}

/** Get a copy of the element at the given index. The caller is responsible
 *  for freeing the memory occupied by the copy. */
char *vect_get_copy(vect_t *v, unsigned int idx) {
  assert(v != NULL);
  assert(idx < v->size);
  char *tc = (char *) malloc(sizeof(char *) * strlen(v->data[idx])); // malloc for the size of the char to copy
  strcpy(tc, v->data[idx]); // string copy the value at the index to the tmp variable
  return tc; // return it
}

/** Set the element at the given index. */
void vect_set(vect_t *v, unsigned int idx, const char *elt) {
  assert(v != NULL);
  free(v->data[idx]); // free the value at the current index to set so we don't need to worry about overriding things
  char *tc = (char *) malloc(sizeof(char *) * strlen(elt)); // malloc for the character to copy
  strcpy(tc, elt); // copy over the item to set
  v->data[idx] = tc; // set the item at the index to the local alias of elt
}

/** Add an element to the back of the vector. */
void vect_add(vect_t *v, const char *elt) {
  assert(v != NULL);
  if(v->size >= v->capacity) { // if the size is greater than the capacity we need to allocate more memory
    // realloc
    // new capacity and new items required to be increased in size
    v->capacity *=  2; // double the capacity
    v->data = realloc(v->data, v->capacity * sizeof(char *)); // realloc more data
  }
  char *tc = malloc(1 + strlen(elt)); // set up local variable etc
  strcpy(tc, elt);
  v->data[v->size++] = tc;  // increment size and set the new element to add to be at that index
}

/** Remove the last element from the vector. */
void vect_remove_last(vect_t *v) {
  assert(v != NULL);
  if(v->size > 0) {
    v->data[v->size - 1] = NULL; // set the last value of data to null 
    v->size--; // decrement the size
  }
}

/** The number of items currently in the vector. */
unsigned int vect_size(vect_t *v) {
  assert(v != NULL);
  return v->size;
}

/** The maximum number of items the vector can hold before it has to grow. */
unsigned int vect_current_capacity(vect_t *v) {
  assert(v != NULL);
  return v->capacity;
}

void vect_print(vect_t *v) {
  for(int i = 0; i < v->size; i++) {
    printf("%s\n", v->data[i]);
  }
}
