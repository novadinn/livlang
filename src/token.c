#include "token.h"

#include "logger.h"

#include <stdlib.h>

void tokenDestroy(Token *token) {
  switch (token->type) {
  case TOKEN_TYPE_STRLIT: {
    free(token->value.string);
  } break;
  case TOKEN_TYPE_IDENT: {
    free(token->value.identifier);
  } break;
  default: {
  } break;
  }
}

void tokenPrint(Token *token) {
  const char *types[TOKEN_TYPE_MAX + 1] = {
      "NONE",   "EOF",      "PLUS",   "MINUS",  "STAR",     "SLASH",   "EQ",
      "NE",     "LT",       "GT",     "LE",     "GE",       "AND",     "OR",
      "ASSIGN", "INC",      "DEC",    "INTLIT", "FLOATLIT", "CHARLIT", "STRLIT",
      "IDENT",  "SEMI",     "COLON",  "COMMA",  "ARROW",    "DOT",     "EXMARK",
      "LBRACE", "RBRACE",   "LPAREN", "RPAREN", "LBRACK",   "RBRACK",  "IMPORT",
      "VAR",    "FUN",      "IF",     "ELSE",   "WHILE",    "FOR",     "RETURN",
      "BREAK",  "CONTINUE", "VOID",   "INT",    "FLOAT",    "CHAR",    "STRING",
      "PRINT",  "MAX",
  };

  switch (token->type) {
  case TOKEN_TYPE_INTLIT: {
    DEBUG("%s %ld", types[token->type], token->value.integer);
  } break;
  case TOKEN_TYPE_FLOATLIT: {
    DEBUG("%s %f", types[token->type], token->value.floating);
  } break;
  case TOKEN_TYPE_CHARLIT: {
    DEBUG("%s %c", types[token->type], token->value.character);
  } break;
  case TOKEN_TYPE_STRLIT: {
    DEBUG("%s %s", types[token->type], token->value.string);
  } break;
  case TOKEN_TYPE_IDENT: {
    DEBUG("%s %s", types[token->type], token->value.identifier);
  } break;
  default: {
    DEBUG("%s", types[token->type]);
  } break;
  };
}