#include "../include/defs_utils.h"
#include <stdio.h>

char *spc(char *p) {
  if (p == NULL) {
    fprintf(stderr, "NULL ptr error\n");
    exit(EXIT_FAILURE);
  }
  return p;
}

char *readfile(char *path, char *modes) {
  FILE *f = fopen(path, modes);
  if (f == NULL) {
    return NULL;
  }
  size_t read_len = 0, alloc_iter = 0;
  char *str = malloc(BUFSIZ);
  while (!feof(f)) {
    read_len +=
      fread(&str[(alloc_iter * BUFSIZ) + read_len], 1, BUFSIZ - read_len, f);
    if (read_len == BUFSIZ) {
      alloc_iter += 1;
      str = realloc(str, alloc_iter + 1 * BUFSIZ);
      read_len = 0;
    }
  }
  str[alloc_iter * BUFSIZ + read_len] = '\0';
  return str;
}
