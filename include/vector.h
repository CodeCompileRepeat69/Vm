#ifndef __VECTOR_H
#define __VECTOR_H

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

#define Slice_t(Type) struct {size_t size; Type *data;}

#define Vec_t(Type)				\
  struct {size_t cap; size_t size; Type *data;}

typedef Slice_t(char) mem_slice;
typedef Vec_t(char) mem_vec;
/*
  ARGS:
      The first argument will be the nth value in the vector
      to be compared, the second the one you want to actually find
  RETURN:
      The function should retun true if the first argument is what you
      looked for or false otherwise
*/
typedef bool (*find_custom_fn)(void *, void *);
typedef char *(copy_fn)(char*,char*);


bool _vec_expand(char **data, size_t *cap, size_t *size, size_t membsize,
                 size_t to_exp);

mem_slice _vec_split_as_slice(char **data, size_t *cap, size_t *size,
                              size_t membsize, size_t start, size_t end);

void *_vec_find(char **data, size_t *cap, size_t *size, size_t membsize,
                void *value);

void *_vec_find_custom(char **data, size_t *cap, size_t *size, size_t membsize,
                       find_custom_fn finder, void *value);

void *_vec_dup(char **data, size_t *cap, size_t *size, size_t membsize);

void *_vec_dup_deep(char **data, size_t *cap, size_t *size, size_t membsize, copy_fn copy);

#define vec_arg(vec)							\
  (char **)&(vec)->data, &(vec)->cap, &(vec)->size, sizeof(*(vec)->data)

#define vec_push(vec, val)				\
  _vec_expand(vec_arg(vec), (vec)->cap * 2) == true ?	\
    ((vec)->data[(vec)->size++] = val, true) : false

#define vec_foreach(vec_type, vec, iterator_name)			\
  for(vec_type *iterator_name = (vec)->data;				\
      iterator_name< (vec)->data + (vec)->size; iterator_name++)

#define vec_clean(vec)				\
  do {						\
    memset((vec)->data, 0, (vec)->cap);		\
    (vec)->size = 0;				\
  } while (0);

#define vec_destroy(vec)			\
  do {						\
    free((vec)->data);				\
    (vec)->cap = 0;				\
    (vec)->size = 0;				\
  } while (0);


//   _vec_dup(sizeof((vec_ptr)->data[0]), (mem_vec *)vec_ptr)
#define vec_dup_shallow(VectorType, vec_ptr)		\
  (VectorType){						\
  .data = _vec_dup(vec_arg(vec_ptr)),			\
    .cap = (vec_ptr)->cap,				\
    .size = (vec_ptr)->size}

#define vec_dup_deep(VectorType, vec_ptr, copy_function)	\
  (VectorType){							\
    .data = _vec_dup_deep(vec_arg(vec_ptr), copy_function),	\
      .cap = (vec_ptr)->cap,					\
      .size = (vec_ptr)->size}


#define vec_split_as_slice(slice_t, vec, start, end)	\
  (slice_t) _vec_split_as_slice(vec_arg, start, end)    


#endif /* __VECTOR_H */
