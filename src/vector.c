#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "vector.h"

#define VEC_ARG_PROTO char **data, size_t *cap, size_t *size, size_t membsize

bool _vec_expand(char **data, size_t *cap, size_t *size, size_t membsize, size_t to_exp) {
  if ((*size + 1) > *cap) {
    size_t tmp_cap = *cap == 0 ? 1 : to_exp;
    char *tmp = reallocarray(*data, tmp_cap, membsize);
    if (tmp == NULL) {return false;}
    *data = tmp;
    *cap = tmp_cap;
  }
  return true;
}

void *_vec_find(char **data, size_t *cap, size_t *size, size_t membsize, void *value) {
  (void)cap;
  for (size_t i = 0; i < *size; i++) {
    if (memcmp(*data + i * membsize, value, membsize) == 0) {
      return *data + i + membsize;
    }
  }
  return NULL;
}
/*
  ARGS:
      The first argument will be the nth value in the vector
      to be compared, the second the one you want to actually find
  RETURN:
      The function should retun true if the first argument is what you
      looked for or false otherwise
*/
typedef bool (*find_custom_fn)(void *, void *);

void *_vec_find_custom(char **data, size_t *cap, size_t *size, size_t membsize, find_custom_fn finder, void *value) {
  (void)cap;
  for (size_t i = 0; i < *size; i++) {
    if (finder(*data + i * membsize, value) == true) {
      return *data + i + membsize;
    }
  }
  return NULL;
}

mem_slice _vec_split_as_slice(char **data, size_t *cap, size_t *size,
                              size_t membsize, size_t start, size_t end) {
  (void)cap;
  mem_slice slice = {0};
  if (start > *size || end > *size || start > end) {
    return slice;
  }
  slice.data = *data + start * membsize;
  slice.size = end - start;
  return slice;
}

void *_vec_dup(char **data, size_t *cap, size_t *size, size_t membsize){
  (void)size;
  void * new_data = malloc(*cap * membsize);
  if (new_data == NULL) {return NULL;}
  memcpy(new_data, *data, *cap * membsize);
  return new_data;
}

void *_vec_dup_deep(char **data, size_t *cap, size_t *size, size_t membsize,
                    copy_fn copy) {
  char * new_data = malloc(*cap * membsize);
  if (new_data == NULL) {return NULL;}
  for (size_t i = 0; i < *size; i++) {
    copy(new_data + i * membsize, *data + i * membsize);
  }
  return new_data;
}
