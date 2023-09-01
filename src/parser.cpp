#include "parser.h"

#include "logger.h"

typedef struct Parser {
  std::vector<Token> *tokens;
} Parser;

static Parser *parser;

b8 parser_system_initialize(std::vector<Token> &tokens) {
  V_ASSERT_MSG(!parser, "vlang: parser already initialized!");

  parser = (Parser *)malloc(sizeof(*parser));
  parser->tokens = &tokens;

  return true;
}

void parser_system_shutdown() { free(parser); }

std::vector<ASTNode> parser_build_tree() { return std::vector<ASTNode>(); }