#include "eval.h"
#include "file_io.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"
#include "vector.h"

#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc != 2) {
    FATAL("liv: no input file given!");
    exit(1);
  }

  char *source;
  if (!readFile(argv[1], &source)) {
    FATAL("liv: failed to read a file %s!", argv[1]);
    exit(1);
  }

  Lexer lexer;
  lexerCreate(source, &lexer);

  Token *tokens = lexerScan(&lexer);

  Parser parser;
  parserCreate(tokens, &parser);

  ASTNode root = parserBuildAST(&parser);

  Environment global_env;
  environmentCreate(0, &global_env);

  eval(&root, &global_env);

  environmentDestroy(&global_env);

  ASTNodeDestroy(&root);
  parserDestroy(&parser);

  for (u32 i = 0; i < vectorLength(tokens); ++i) {
    tokenDestroy(&tokens[i]);
  }

  lexerDestroy(&lexer);
  vectorDestroy(tokens);

  free(source);

  return 0;
}