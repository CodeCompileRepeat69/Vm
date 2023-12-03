#ifndef __DEFS_UTILS_H_
#define __DEFS_UTILS_H_

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

#define TO_STR(val) #val

#define EQ ==
#define OR ||
#define NOT !
#define XOR ^
#define AND && 
#define NOT_EQ !=


/*output parameter*/
#define _out_param_ 
/*dynamically allocated pointer*/
#define _dyn_alloc_ 
/*
  Secure Pointer Check.
  Terminates the program with exit(EXIT_FAILURE) if p == NULL,
  printing to stderr "NULL ptr error", otherwise returns p
*/
char *spc(char *p);
char *readfile(char *path, char *modes);
#define ARRAY_SIZE(array, type) (sizeof(array) / sizeof(type))

#endif //__DEFS_UTILS_H_
