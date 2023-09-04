#ifndef LEXER_H
#define LEXER_H

#include "token.h"

#include <string>
#include <vector>

typedef struct Lexer {
  char *source;
  i64 index;
  i64 line;
} Lexer;

Lexer *lexer_create(char *source);
void lexer_destroy(Lexer *lexer);

std::vector<Token> lexer_scan(Lexer *lexer, char *src);
Token lexer_read_token(Lexer *lexer);
f64 lexer_read_number(Lexer *lexer, char c, b8 *has_decimal);
char lexer_read_char(Lexer *lexer);
std::string lexer_read_string(Lexer *lexer);
std::string lexer_read_identifier(Lexer *lexer, char c);
TokenType lexer_read_keyword(Lexer *lexer, const std::string &s);

char lexer_next_letter(Lexer *lexer);
char lexer_next_letter_skip(Lexer *lexer);
void lexer_skip_line(Lexer *lexer);

char lexer_char_pos(Lexer *lexer, char *s, char c);

#endif // LEXER_H