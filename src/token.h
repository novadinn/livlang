#ifndef TOKEN_H
#define TOKEN_H

#include "defines.h"

#include <string>

typedef enum TokenType {
  TOKEN_TYPE_NONE = 0,
  /* end of file */
  TOKEN_TYPE_EOF,
  /* + */
  TOKEN_TYPE_PLUS,
  /* - */
  TOKEN_TYPE_MINUS,
  /* * */
  TOKEN_TYPE_STAR,
  /* / */
  TOKEN_TYPE_SLASH,
  /* == */
  TOKEN_TYPE_EQ,
  /* != */
  TOKEN_TYPE_NE,
  /* < */
  TOKEN_TYPE_LT,
  /* > */
  TOKEN_TYPE_GT,
  /* <= */
  TOKEN_TYPE_LE,
  /* >= */
  TOKEN_TYPE_GE,
  /* && */
  TOKEN_TYPE_AND,
  /* || */
  TOKEN_TYPE_OR,
  /* = */
  TOKEN_TYPE_ASSIGN,
  /* ++ */
  TOKEN_TYPE_INC,
  /* -- */
  TOKEN_TYPE_DEC,
  /* integer literal */
  TOKEN_TYPE_INTLIT,
  /* float literal */
  TOKEN_TYPE_FLOATLIT,
  /* char literal */
  TOKEN_TYPE_CHARLIT,
  /* string literal */
  TOKEN_TYPE_STRLIT,
  /* identifier */
  TOKEN_TYPE_IDENT,
  /* ; */
  TOKEN_TYPE_SEMI,
  /* : */
  TOKEN_TYPE_COLON,
  /* , */
  TOKEN_TYPE_COMMA,
  /* -> */
  TOKEN_TYPE_ARROW,
  /* . */
  TOKEN_TYPE_DOT,
  /* ! */
  TOKEN_TYPE_EXMARK,
  /* { */
  TOKEN_TYPE_LBRACE,
  /* } */
  TOKEN_TYPE_RBRACE,
  /* ( */
  TOKEN_TYPE_LPAREN,
  /* ) */
  TOKEN_TYPE_RPAREN,
  /* [ */
  TOKEN_TYPE_LBRACK,
  /* ] */
  TOKEN_TYPE_RBRACK,
  /* import keyword */
  TOKEN_TYPE_IMPORT,
  /* var keyword */
  TOKEN_TYPE_VAR,
  /* fun keyword */
  TOKEN_TYPE_FUN,
  /* if keyword */
  TOKEN_TYPE_IF,
  /* else keyword */
  TOKEN_TYPE_ELSE,
  /* while keyword */
  TOKEN_TYPE_WHILE,
  /* for keyword */
  TOKEN_TYPE_FOR,
  /* return keyword */
  TOKEN_TYPE_RETURN,
  /* break keyword */
  TOKEN_TYPE_BREAK,
  /* continue keyword */
  TOKEN_TYPE_CONTINUE,
  /* void keyword */
  TOKEN_TYPE_VOID,
  /* int keyword */
  TOKEN_TYPE_INT,
  /* float keyword */
  TOKEN_TYPE_FLOAT,
  /* char keyword */
  TOKEN_TYPE_CHAR,
  /* string keyword */
  TOKEN_TYPE_STRING,
  /* builtin print function */
  TOKEN_TYPE_PRINT,
  TOKEN_TYPE_MAX,
} TokenType;

typedef struct TokenValue {
  i64 integer;
  f64 floating;
  u8 character;
  std::string string;
  std::string identifier;
} TokenValue;

typedef struct Token {
  u8 type;
  TokenValue value;
} Token;

void token_print(Token token);

#endif // TOKEN_H
