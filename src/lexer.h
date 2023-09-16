#ifndef LEXER_H
#define LEXER_H

#include "token.h"

#include <string>
#include <vector>

typedef struct Lexer {
  const char *source;
  i64 index;
  i64 line;
} Lexer;

Lexer *lexer_create(const char *source);
void lexer_destroy(Lexer *lexer);

std::vector<Token> lexer_scan(Lexer *lexer);

#endif // LEXER_H
