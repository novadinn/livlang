#ifndef PARSER_H
#define PARSER_H

#include "ast_node.h"
#include "defines.h"
#include "token.h"

#include <vector>

b8 parser_system_initialize(std::vector<Token> &tokens);
void parser_system_shutdown();

std::vector<ASTNode> parser_build_tree();

#endif // PARSER_H