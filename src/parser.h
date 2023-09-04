#ifndef PARSER_H
#define PARSER_H

#include "ast_node.h"
#include "defines.h"
#include "token.h"

#include <vector>

b8 parser_system_initialize(std::vector<Token> &tokens);
void parser_system_shutdown();

ASTNode parser_build_tree();

ASTNode parser_make_node(ASTNodeType type, std::vector<ASTNode> children = {}, TokenValue value = {});

Token &parser_token();
void parser_next_token();
void parser_prev_token();
ASTNodeType parser_arithop(TokenType type);
int parser_operation_precedence(ASTNodeType type);
bool parser_right_assoc(TokenType type);
bool parser_operation_binary();

void parser_match(TokenType type);
bool parser_match_b(TokenType type);
void parser_match_str();
void parser_match_type();

ASTNode parser_ident();
ASTNode parser_prefix();
ASTNode parser_postfix();
ASTNode parser_literal();
ASTNode parser_array_access();
ASTNode parser_funccall();
ASTNode parser_binexpr(int pr);
std::vector<ASTNode> parser_global_statements();
std::vector<ASTNode> parser_struct_statements();

void parser_semi();
void parser_colon();
bool parser_colonb();
void parser_lbrack();
void parser_rbrack();
void parser_lparen();
void parser_rparen();
void parser_lbrace();
void parser_rbrace();

std::vector<ASTNode> parser_import_statement();
ASTNode parser_while_statement();
ASTNode parser_for_statement();
ASTNode parser_foreach_statement();
ASTNode parser_if_statement();
ASTNode parser_return_statement();
ASTNode parser_continue_statement();
ASTNode parser_break_statement();
ASTNode parser_print_statement();
ASTNode parser_using_statement();

ASTNode parser_fun_declaration();
ASTNode parser_var_declaration(bool need_type = true);
ASTNode parser_struct_member_declaration();
ASTNode parser_fun_param_declaration();
ASTNode parser_struct_declaration();
ASTNode parser_ident_declaration(bool need_type = true);

ASTNode parser_structlit();
ASTNode parser_block();
ASTNode parser_type();

#endif // PARSER_H
