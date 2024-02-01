#include "file_io.h"

#include "logger.h"

#include <stdio.h>
#include <stdlib.h>

b8 readFile(const char *path, char **buf) {
  FILE *f = fopen(path, "rb");
  if (f == 0) {
    return false;
  }

  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  fseek(f, 0, SEEK_SET);

  (*buf) = malloc(fsize + 1);
  fread(*buf, sizeof(char), fsize, f);
  fclose(f);

  (*buf)[fsize] = EOF;

  return true;
}