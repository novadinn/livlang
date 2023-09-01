#ifndef LEXER_H
#define LEXER_H

#include "token.h"

#include <string>
#include <vector>

b8 lexer_system_initialize(char *source);
void lexer_system_shutdown();

std::vector<Token> lexer_scan(char *src);
Token lexer_read_token();
f64 lexer_read_number(char c, b8 *has_decimal);
char lexer_read_char();
std::string lexer_read_string();
std::string lexer_read_identifier(char c);
TokenType lexer_read_keyword(const std::string &s);

char lexer_next_letter();
char lexer_next_letter_skip();
void lexer_skip_line();

char lexer_char_pos(char *s, char c);

#endif // LEXER_H