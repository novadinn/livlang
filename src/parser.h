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

ASTNode parser_make_node(Parser *parser, ASTNodeType type, std::vector<ASTNode> children = {}, TokenValue value = {});

Token &parser_token(Parser *parser);
void parser_next_token(Parser *parser);
void parser_prev_token(Parser *parser);
ASTNodeType parser_arithop(Parser *parser, TokenType type);
int parser_operation_precedence(Parser *parser, ASTNodeType type);
bool parser_right_assoc(Parser *parser, TokenType type);
bool parser_operation_binary(Parser *parser);

void parser_match(Parser *parser, TokenType type);
bool parser_match_b(Parser *parser, TokenType type);
void parser_match_str(Parser *parser);
void parser_match_type(Parser *parser);

ASTNode parser_ident(Parser *parser);
ASTNode parser_prefix(Parser *parser);
ASTNode parser_postfix(Parser *parser);
ASTNode parser_literal(Parser *parser);
ASTNode parser_array_access(Parser *parser);
ASTNode parser_funccall(Parser *parser);
ASTNode parser_binexpr(Parser *parse, int pr);
std::vector<ASTNode> parser_global_statements(Parser *parser);
std::vector<ASTNode> parser_struct_statements(Parser *parser);

void parser_semi(Parser *parser);
void parser_colon(Parser *parser);
bool parser_colonb(Parser *parser);
void parser_lbrack(Parser *parser);
void parser_rbrack(Parser *parser);
void parser_lparen(Parser *parser);
void parser_rparen(Parser *parser);
void parser_lbrace(Parser *parser);
void parser_rbrace(Parser *parser);

std::vector<ASTNode> parser_import_statement(Parser *parser);
ASTNode parser_while_statement(Parser *parser);
ASTNode parser_for_statement(Parser *parser);
ASTNode parser_if_statement(Parser *parser);
ASTNode parser_return_statement(Parser *parser);
ASTNode parser_continue_statement(Parser *parser);
ASTNode parser_break_statement(Parser *parser);
ASTNode parser_print_statement(Parser *parser);

ASTNode parser_fun_declaration(Parser *parser);
ASTNode parser_var_declaration(Parser *parser, bool need_type = true);
ASTNode parser_struct_member_declaration(Parser *parser);
ASTNode parser_fun_param_declaration(Parser *parser);
ASTNode parser_struct_declaration(Parser *parser);
ASTNode parser_ident_declaration(Parser *parser, bool need_type = true);

ASTNode parser_structlit(Parser *parser);
ASTNode parser_block(Parser *parser);
ASTNode parser_type(Parser *parser);

#endif // PARSER_H
