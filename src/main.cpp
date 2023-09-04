#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "defines.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"

b8 read_file(const char *path, char **buf);

int main(int argc, char **argv) {
  if (argc != 2) {
    V_FATAL("vlad: no input files");
    exit(1);
  }

  const char *file_path = argv[1];
  char *source;
  if (!read_file(argv[1], &source)) {
    V_FATAL("vlad: failed to read file");
    exit(1);
  }

  lexer_system_initialize(source);

  std::vector<Token> tokens = lexer_scan(source);
  for (int i = 0; i < tokens.size(); ++i) {
    token_print(tokens[i]);
  }

  lexer_system_shutdown();

  parser_system_initialize(tokens);

  ASTNode tree = parser_build_tree();

  parser_system_shutdown();

  free(source);

  return 0;
}

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
