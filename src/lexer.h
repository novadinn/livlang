#pragma once

#include "defines.h"
#include "token.h"

typedef struct Lexer {
  const char *source;
  i64 index;
  u64 line;
} Lexer;

void lexerCreate(const char *source, Lexer *out_lexer);
void lexerDestroy(Lexer *lexer);

Token *lexerScan(Lexer *lexer);