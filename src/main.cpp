#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "defines.h"
#include "eval.h"
#include "lexer.h"
#include "logger.h"
#include "parser.h"
#include "utils.h"

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

  Lexer *lexer = lexer_create(source);

  std::vector<Token> tokens = lexer_scan(lexer);
  for (int i = 0; i < tokens.size(); ++i) {
    // token_print(tokens[i]);
  }

  lexer_destroy(lexer);

  Parser *parser = parser_create(tokens);

  ASTNode tree = parser_build_tree(parser);
  ast_print(tree);

  parser_destroy(parser);

  free(source);

  Enviroinment *env = enviroinment_create(nullptr);

  eval(&tree, env);

  enviroinment_destroy(env);

  return 0;
}
