#include "token.h"

#include "logger.h"

void token_print(Token token) {
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

  switch (token.type) {
  case TOKEN_TYPE_INTLIT: {
    V_TRACE("%s %ld", types[token.type], token.value.integer);
  } break;
  case TOKEN_TYPE_FLOATLIT: {
    V_TRACE("%s %f", types[token.type], token.value.floating);
  } break;
  case TOKEN_TYPE_CHARLIT: {
    V_TRACE("%s %c", types[token.type], token.value.character);
  } break;
  case TOKEN_TYPE_STRLIT: {
    V_TRACE("%s %s", types[token.type], token.value.string.c_str());
  } break;
  case TOKEN_TYPE_IDENT: {
    V_TRACE("%s %s", types[token.type], token.value.identifier.c_str());
  } break;
  default: {
    V_TRACE("%s", types[token.type]);
  } break;
  };
}