#include "utils.h"

#include <cstdio>

b8 read_file(const char *path, char **buf) {
  FILE *f = fopen(path, "rb");
  if (f == NULL) {
    V_FATAL("vlad: file %s can't be opened", path);
    return false;
  }

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  (*buf) = (char *)malloc(fsize + 1);
  fread(*buf, fsize, 1, f);
  fclose(f);

  (*buf)[fsize] = EOF;

  return true;
}


