#ifndef PARSER_H
#define PARSER_H

#include "ast_node.h"
#include "defines.h"
#include "token.h"

#include <vector>

typedef struct Parser {
  std::vector<Token> *tokens;
  u32 current_token;
} Parser;

Parser *parser_create(std::vector<Token> &tokens);
void parser_destroy(Parser *parser);

ASTNode parser_build_tree(Parser *parser);

#endif // PARSER_H
