#pragma once

#include "ast_node.h"
#include "defines.h"
#include "token.h"

typedef struct Parser {
  Token *tokens;
  u64 current_token;
} Parser;

void parserCreate(Token *tokens, Parser *out_parser);
void parserDestroy(Parser *parser);

ASTNode parserBuildAST(Parser *parser);