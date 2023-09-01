#include "lexer.h"

#include "logger.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctype.h>
#include <unistd.h>

#define IDENTIFIER_MAX_LENGTH 255
#define MAX_TEXT_LENGTH 2048

typedef struct Lexer {
  char *source;
  i64 index;
  i64 line;
} Lexer;

static Lexer *lexer;

b8 lexer_system_initialize(char *source) {
  V_ASSERT_MSG(!lexer, "vlang: lexer already initialized!");

  lexer = (Lexer *)malloc(sizeof(*lexer));
  lexer->source = source;
  lexer->index = 0;
  lexer->line = 0;

  return true;
}

void lexer_system_shutdown() { free(lexer); }

std::vector<Token> lexer_scan(char *src) {
  std::vector<Token> token_stream;
  Token token;

  while ((token = lexer_read_token()).type != TokenType::TOKEN_TYPE_EOF) {
    token_stream.push_back(token);
  }
  token_stream.push_back(token);

  return token_stream;
}

Token lexer_read_token() {
  Token token;
  char c = lexer_next_letter_skip();

  switch (c) {
  case EOF: {
    token.type = TokenType::TOKEN_TYPE_EOF;
  } break;
  case '+': {
    if ((c = lexer_next_letter()) == '+') {
      token.type = TokenType::TOKEN_TYPE_INC;
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_PLUS;
    }
  } break;
  case '-': {
    c = lexer_next_letter();
    if (c == '>') {
      token.type = TokenType::TOKEN_TYPE_ARROW;
    } else if (c == '-') {
      token.type = TokenType::TOKEN_TYPE_DEC;
    } else if (isdigit(c)) {
      b8 has_decimal;
      f64 val = lexer_read_number(c, &has_decimal);
      if (has_decimal) {
        token.type = TokenType::TOKEN_TYPE_FLOATLIT;
        token.value.floating = -val;
      } else {
        token.type = TokenType::TOKEN_TYPE_INTLIT;
        token.value.integer = -(i64)val;
      }
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_MINUS;
    }
  } break;
  case '*': {
    token.type = TokenType::TOKEN_TYPE_STAR;
  } break;
  case '/': {
    c = lexer_next_letter();
    if (c == '/') {
      lexer_skip_line();
      return lexer_read_token();
    } else if (c == '*') {
      while (1) {
        c = lexer_next_letter();
        if (c == EOF) {
          V_FATAL("vlang: unexpected end of file");
        }
        if (c == '*') {
          if ((c = lexer_next_letter()) == '/') {
            return lexer_read_token();
          } else {
            lexer->index--;
          }
        }
      }
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_SLASH;
    }
  } break;
  case '=': {
    if ((c = lexer_next_letter()) == '=') {
      token.type = TokenType::TOKEN_TYPE_EQ;
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_ASSIGN;
    }
  } break;
  case '!': {
    if ((c = lexer_next_letter()) == '=') {
      token.type = TokenType::TOKEN_TYPE_NE;
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_EXMARK;
    }
  } break;
  case '<': {
    if ((c = lexer_next_letter()) == '=') {
      token.type = TokenType::TOKEN_TYPE_LE;
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_LT;
    }
  } break;
  case '>': {
    if ((c = lexer_next_letter()) == '=') {
      token.type = TokenType::TOKEN_TYPE_GE;
    } else {
      lexer->index--;
      token.type = TokenType::TOKEN_TYPE_GT;
    }
  } break;
  case '&': {
    if ((c = lexer_next_letter()) == '&') {
      token.type = TokenType::TOKEN_TYPE_AND;
    } else {
      V_FATAL("vlang: invalid token at line %d", lexer->line);
    }
  } break;
  case '|': {
    if ((c = lexer_next_letter()) == '|') {
      token.type = TokenType::TOKEN_TYPE_OR;
    } else {
      V_FATAL("vlang: invalid token at line %d", lexer->line);
    }
  } break;
  case ';': {
    token.type = TokenType::TOKEN_TYPE_SEMI;
  } break;
  case '.': {
    token.type = TokenType::TOKEN_TYPE_DOT;
  } break;
  case ':': {
    token.type = TokenType::TOKEN_TYPE_COLON;
  } break;
  case ',': {
    token.type = TokenType::TOKEN_TYPE_COMMA;
  } break;
  case '{': {
    token.type = TokenType::TOKEN_TYPE_LBRACE;
  } break;
  case '}': {
    token.type = TokenType::TOKEN_TYPE_RBRACE;
  } break;
  case '(': {
    token.type = TokenType::TOKEN_TYPE_LPAREN;
  } break;
  case ')': {
    token.type = TokenType::TOKEN_TYPE_RPAREN;
  } break;
  case '[': {
    token.type = TokenType::TOKEN_TYPE_LBRACK;
  } break;
  case ']': {
    token.type = TokenType::TOKEN_TYPE_RBRACK;
  } break;
  case '\'': {
    token.type = TokenType::TOKEN_TYPE_CHARLIT;
    token.value.character = lexer_read_char();

    if (lexer_next_letter() != '\'') {
      V_FATAL("vlang: exprected '\\'' at end of char literal");
    }
  } break;
  case '"': {
    token.type = TOKEN_TYPE_STRLIT;
    token.value.string = lexer_read_string();
  } break;
  default: {
    if (isdigit(c)) {
      b8 has_decimal;
      f64 val = lexer_read_number(c, &has_decimal);
      if (has_decimal) {
        token.type = TokenType::TOKEN_TYPE_FLOATLIT;
        token.value.floating = val;
      } else {
        token.type = TokenType::TOKEN_TYPE_INTLIT;
        token.value.integer = (i64)val;
      }
    } else if (isalpha(c) || c == '_') {
      std::string buf;
      TokenType type;

      buf = lexer_read_identifier(c);

      if ((type = lexer_read_keyword(buf)) != TokenType::TOKEN_TYPE_NONE) {
        token.type = type;
        break;
      }

      token.type = TokenType::TOKEN_TYPE_IDENT;
      token.value.identifier = buf;
      break;
    } else {
      V_FATAL("vlang: unrecognised character %c on line %d", c, lexer->line);
    }
  } break;
  };

  return token;
}

f64 lexer_read_number(char c, b8 *has_decimal) {
  i32 k, val = 0;
  f64 fval = 0;
  i32 num_digits = 0;
  i32 decimal_pos = -1;

  *has_decimal = false;

  while ((k = lexer_char_pos("0123456789.", c)) >= 0) {
    if (c == '.') {
      *has_decimal = true;
      if (decimal_pos >= 0) {
        V_FATAL("vlang: invalid float value");
      }

      decimal_pos = num_digits;
    } else {
      val = val * 10 + k;
      num_digits++;
    }

    c = lexer_next_letter();
  }

  if (decimal_pos >= 0) {
    i32 divisor = 1;
    for (i32 i = 0; i < (num_digits - decimal_pos); i++) {
      divisor *= 10;
    }

    fval = (f64)val / (f64)divisor;
  } else {
    fval = (f64)val;
  }

  lexer->index--;
  return fval;
}

char lexer_read_char() {
  char c;

  c = lexer_next_letter();
  if (c == '\\') {
    switch (c = lexer_next_letter()) {
    case 'a':
      return '\a';
    case 'b':
      return '\b';
    case 'f':
      return '\f';
    case 'n':
      return '\n';
    case 'r':
      return '\r';
    case 't':
      return '\t';
    case 'v':
      return '\v';
    case '\\':
      return '\\';
    case '"':
      return '"';
    case '\'':
      return '\'';
    default:
      V_FATAL("vlang: unknown escape sequence %c", c);
    }
  }

  return c;
}

std::string lexer_read_string() {
  int i;
  char c;
  std::string buf;
  buf.reserve(MAX_TEXT_LENGTH);

  for (i = 0; i < MAX_TEXT_LENGTH - 1; ++i) {
    if ((c = lexer_read_char()) == '"') {
      buf[i] = 0;
      return buf;
    }
    buf[i] = c;
  }

  V_FATAL("vlang: string literal too long on line %d", lexer->line);

  return "";
}

std::string lexer_read_identifier(char c) {
  std::string buf;
  buf.reserve(IDENTIFIER_MAX_LENGTH);
  i32 i = 0;

  while (isalpha(c) || isdigit(c) || c == '_') {
    if (i == (IDENTIFIER_MAX_LENGTH - 1)) {
      V_FATAL("vlang: identifier too long on line %d", lexer->line);
    }

    buf[i++] = c;
    c = lexer_next_letter();
  }

  lexer->index--;
  buf[i] = '\0';

  return buf;
}

TokenType lexer_read_keyword(const std::string &s) {
  V_ASSERT_MSG(s.empty(), "vlang: empty string error in lexer_read_keyword");

  switch (s[0]) {
  case 'i': {
    if (!strcmp(s.c_str(), "int"))
      return TokenType::TOKEN_TYPE_INT;
    if (!strcmp(s.c_str(), "if"))
      return TokenType::TOKEN_TYPE_IF;
    if (!strcmp(s.c_str(), "import"))
      return TokenType::TOKEN_TYPE_IMPORT;
  } break;
  case 'p': {
    if (!strcmp(s.c_str(), "print"))
      return TokenType::TOKEN_TYPE_PRINT;
  } break;
  case 'w': {
    if (!strcmp(s.c_str(), "while"))
      return TokenType::TOKEN_TYPE_WHILE;
  } break;
  case 'f': {
    if (!strcmp(s.c_str(), "float"))
      return TokenType::TOKEN_TYPE_FLOAT;
    if (!strcmp(s.c_str(), "for"))
      return TokenType::TOKEN_TYPE_FOR;
    if (!strcmp(s.c_str(), "fun")) {
      return TokenType::TOKEN_TYPE_FUN;
    }
  } break;
  case 's': {
    if (!strcmp(s.c_str(), "struct"))
      return TokenType::TOKEN_TYPE_STRUCT;
    if (!strcmp(s.c_str(), "string"))
      return TokenType::TOKEN_TYPE_STRING;
  } break;
  case 'v': {
    if (!strcmp(s.c_str(), "var"))
      return TokenType::TOKEN_TYPE_VAR;
    if (!strcmp(s.c_str(), "void"))
      return TokenType::TOKEN_TYPE_VOID;
  } break;
  case 'r': {
    if (!strcmp(s.c_str(), "return"))
      return TokenType::TOKEN_TYPE_RETURN;
  } break;
  case 'b': {
    if (!strcmp(s.c_str(), "break"))
      return TokenType::TOKEN_TYPE_BREAK;
  } break;
  case 'c': {
    if (!strcmp(s.c_str(), "char"))
      return TokenType::TOKEN_TYPE_CHAR;
    if (!strcmp(s.c_str(), "continue"))
      return TokenType::TOKEN_TYPE_CONTINUE;
  } break;
  case 'e': {
    if (!strcmp(s.c_str(), "else"))
      return TokenType::TOKEN_TYPE_ELSE;
  } break;
  };

  return TokenType::TOKEN_TYPE_NONE;
}

char lexer_next_letter() {
  char c;

  V_ASSERT_MSG(lexer->index < strlen(lexer->source),
               "vlang: lexer source index out of range");
  c = lexer->source[lexer->index++];
  if (c == '\n')
    ++lexer->line;

  return c;
}

char lexer_next_letter_skip() {
  char c = lexer_next_letter();

  while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
    c = lexer_next_letter();

  return c;
}

void lexer_skip_line() {
  int line = lexer->line;

  while (line == lexer->line)
    lexer_next_letter();
}

char lexer_char_pos(char *s, char c) {
  char *p;

  p = strchr(s, c);
  return (p ? p - s : -1);
}