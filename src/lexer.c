#include "lexer.h"

#include "logger.h"
#include "vector.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define IDENTIFIER_MAX_LENGTH 255
#define MAX_TEXT_LENGTH 2048

static Token lexerReadToken(Lexer *lexer);
static f64 lexerReadNumber(Lexer *lexer, char c, b8 *has_decimal);
static char lexerReadChar(Lexer *lexer);
static char *lexerReadString(Lexer *lexer);
static char *lexerReadIdentifier(Lexer *lexer, char c);
static TokenType lexerReadKeyword(Lexer *lexer, const char *s);

static char lexerNextLetter(Lexer *lexer);
static char lexerNextLetterSkip(Lexer *lexer);
static void lexerSkipLine(Lexer *lexer);

static char lexerCharPos(Lexer *lexer, char *s, char c);

void lexerCreate(const char *source, Lexer *out_lexer) {
  out_lexer->source = source;
  out_lexer->index = 0;
  out_lexer->line = 0;
}

void lexerDestroy(Lexer *lexer) {
  lexer->source = 0;
  lexer->index = 0;
  lexer->line = 0;
}

Token *lexerScan(Lexer *lexer) {
  Token *tokens = vectorCreate(Token);
  Token token = {};

  do {
    token = lexerReadToken(lexer);
    vectorPush(tokens, token);
  } while (token.type != TOKEN_TYPE_EOF);

  return tokens;
}

static Token lexerReadToken(Lexer *lexer) {
  Token token = {};
  char c = lexerNextLetterSkip(lexer);

  switch (c) {
  case EOF: {
    token.type = TOKEN_TYPE_EOF;
  } break;
  case '+': {
    if ((c = lexerNextLetter(lexer)) == '+') {
      token.type = TOKEN_TYPE_INC;
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_PLUS;
    }
  } break;
  case '-': {
    c = lexerNextLetter(lexer);
    if (c == '>') {
      token.type = TOKEN_TYPE_ARROW;
    } else if (c == '-') {
      token.type = TOKEN_TYPE_DEC;
    } else if (isdigit(c)) {
      b8 has_decimal;
      f64 val = lexerReadNumber(lexer, c, &has_decimal);
      if (has_decimal) {
        token.type = TOKEN_TYPE_FLOATLIT;
        token.value.floating = -val;
      } else {
        token.type = TOKEN_TYPE_INTLIT;
        token.value.integer = -(i64)val;
      }
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_MINUS;
    }
  } break;
  case '*': {
    token.type = TOKEN_TYPE_STAR;
  } break;
  case '/': {
    c = lexerNextLetter(lexer);
    if (c == '/') {
      lexerSkipLine(lexer);
      return lexerReadToken(lexer);
    } else if (c == '*') {
      while (1) {
        c = lexerNextLetter(lexer);
        if (c == EOF) {
          FATAL("liv: unexpected end of file!");
        }
        if (c == '*') {
          if ((c = lexerNextLetter(lexer)) == '/') {
            return lexerReadToken(lexer);
          } else {
            lexer->index--;
          }
        }
      }
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_SLASH;
    }
  } break;
  case '=': {
    if ((c = lexerNextLetter(lexer)) == '=') {
      token.type = TOKEN_TYPE_EQ;
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_ASSIGN;
    }
  } break;
  case '!': {
    if ((c = lexerNextLetter(lexer)) == '=') {
      token.type = TOKEN_TYPE_NE;
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_EXMARK;
    }
  } break;
  case '<': {
    if ((c = lexerNextLetter(lexer)) == '=') {
      token.type = TOKEN_TYPE_LE;
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_LT;
    }
  } break;
  case '>': {
    if ((c = lexerNextLetter(lexer)) == '=') {
      token.type = TOKEN_TYPE_GE;
    } else {
      lexer->index--;
      token.type = TOKEN_TYPE_GT;
    }
  } break;
  case '&': {
    if ((c = lexerNextLetter(lexer)) == '&') {
      token.type = TOKEN_TYPE_AND;
    } else {
      FATAL("liv: invalid token at line %d!", lexer->line);
      exit(1);
    }
  } break;
  case '|': {
    if ((c = lexerNextLetter(lexer)) == '|') {
      token.type = TOKEN_TYPE_OR;
    } else {
      FATAL("liv: invalid token at line %d!", lexer->line);
      exit(1);
    }
  } break;
  case ';': {
    token.type = TOKEN_TYPE_SEMI;
  } break;
  case '.': {
    token.type = TOKEN_TYPE_DOT;
  } break;
  case ':': {
    token.type = TOKEN_TYPE_COLON;
  } break;
  case ',': {
    token.type = TOKEN_TYPE_COMMA;
  } break;
  case '{': {
    token.type = TOKEN_TYPE_LBRACE;
  } break;
  case '}': {
    token.type = TOKEN_TYPE_RBRACE;
  } break;
  case '(': {
    token.type = TOKEN_TYPE_LPAREN;
  } break;
  case ')': {
    token.type = TOKEN_TYPE_RPAREN;
  } break;
  case '[': {
    token.type = TOKEN_TYPE_LBRACK;
  } break;
  case ']': {
    token.type = TOKEN_TYPE_RBRACK;
  } break;
  case '\'': {
    token.type = TOKEN_TYPE_CHARLIT;
    token.value.character = lexerReadChar(lexer);

    if (lexerNextLetter(lexer) != '\'') {
      FATAL("liv: exprected '\\'' at end of char literal!");
    }
  } break;
  case '"': {
    token.type = TOKEN_TYPE_STRLIT;
    token.value.string = lexerReadString(lexer);
  } break;
  default: {
    if (isdigit(c)) {
      b8 has_decimal = false;
      f64 val = lexerReadNumber(lexer, c, &has_decimal);
      if (has_decimal) {
        token.type = TOKEN_TYPE_FLOATLIT;
        token.value.floating = val;
      } else {
        token.type = TOKEN_TYPE_INTLIT;
        token.value.integer = (i64)val;
      }
    } else if (isalpha(c) || c == '_') {
      TokenType type;

      char *buf = lexerReadIdentifier(lexer, c);

      if ((type = lexerReadKeyword(lexer, buf)) != TOKEN_TYPE_NONE) {
        token.type = type;
        free(buf);
        break;
      }

      token.type = TOKEN_TYPE_IDENT;
      token.value.identifier = buf;
      break;
    } else {
      FATAL("liv: unrecognised character %c on line %d!", c, lexer->line);
      exit(1);
    }
  } break;
  };

  return token;
}

static f64 lexerReadNumber(Lexer *lexer, char c, b8 *has_decimal) {
  i8 k = 0;
  i64 val = 0;
  f64 fval = 0;
  i32 num_digits = 0;
  i32 decimal_pos = -1;

  *has_decimal = false;

  while ((k = lexerCharPos(lexer, "0123456789.", c)) >= 0) {
    if (c == '.') {
      *has_decimal = true;
      if (decimal_pos >= 0) {
        FATAL("liv: invalid float value");
        exit(1);
      }

      decimal_pos = num_digits;
    } else {
      val = val * 10 + k;
      num_digits++;
    }

    c = lexerNextLetter(lexer);
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

static char lexerReadChar(Lexer *lexer) {
  char c = 0;

  c = lexerNextLetter(lexer);
  if (c == '\\') {
    switch (c = lexerNextLetter(lexer)) {
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
      FATAL("liv: unknown escape sequence %c", c);
      exit(1);
    }
  }

  return c;
}

static char *lexerReadString(Lexer *lexer) {
  u32 i = 0;
  char c = 0;
  char *buf = malloc(sizeof(char) * MAX_TEXT_LENGTH);

  for (i = 0; i < MAX_TEXT_LENGTH - 1; ++i) {
    if ((c = lexerReadChar(lexer)) == '"') {
      buf[i] = '\0';

      return buf;
    }
    buf[i] = c;
  }

  FATAL("liv: string literal too long on line %d", lexer->line);
  exit(1);

  return buf;
}

static char *lexerReadIdentifier(Lexer *lexer, char c) {
  char *buf = malloc(sizeof(char) * MAX_TEXT_LENGTH);
  u32 i = 0;

  while (isalpha(c) || isdigit(c) || c == '_') {
    if (i == (IDENTIFIER_MAX_LENGTH - 1)) {
      FATAL("liv: identifier too long on line %d", lexer->line);
      exit(1);
    }

    buf[i] = c;
    i++;
    c = lexerNextLetter(lexer);
  }

  lexer->index--;
  buf[i] = '\0';

  return buf;
}

static TokenType lexerReadKeyword(Lexer *lexer, const char *s) {
  switch (s[0]) {
  case 'i': {
    if (!strcmp(s, "int"))
      return TOKEN_TYPE_INT;
    if (!strcmp(s, "if"))
      return TOKEN_TYPE_IF;
    if (!strcmp(s, "import"))
      return TOKEN_TYPE_IMPORT;
  } break;
  case 'p': {
    if (!strcmp(s, "print"))
      return TOKEN_TYPE_PRINT;
  } break;
  case 'w': {
    if (!strcmp(s, "while"))
      return TOKEN_TYPE_WHILE;
  } break;
  case 'f': {
    if (!strcmp(s, "float"))
      return TOKEN_TYPE_FLOAT;
    if (!strcmp(s, "for"))
      return TOKEN_TYPE_FOR;
    if (!strcmp(s, "fun")) {
      return TOKEN_TYPE_FUN;
    }
  } break;
  case 's': {
    if (!strcmp(s, "string"))
      return TOKEN_TYPE_STRING;
  } break;
  case 'v': {
    if (!strcmp(s, "var"))
      return TOKEN_TYPE_VAR;
    if (!strcmp(s, "void"))
      return TOKEN_TYPE_VOID;
  } break;
  case 'r': {
    if (!strcmp(s, "return"))
      return TOKEN_TYPE_RETURN;
  } break;
  case 'b': {
    if (!strcmp(s, "break"))
      return TOKEN_TYPE_BREAK;
  } break;
  case 'c': {
    if (!strcmp(s, "char"))
      return TOKEN_TYPE_CHAR;
    if (!strcmp(s, "continue"))
      return TOKEN_TYPE_CONTINUE;
  } break;
  case 'e': {
    if (!strcmp(s, "else"))
      return TOKEN_TYPE_ELSE;
  } break;
  };

  return TOKEN_TYPE_NONE;
}

static char lexerNextLetter(Lexer *lexer) {
  char c = 0;

  if (lexer->index >= strlen(lexer->source)) {
    FATAL("liv: lexer source index out of range");
    exit(1);
  }

  c = lexer->source[lexer->index++];
  if (c == '\n')
    ++lexer->line;

  return c;
}

static char lexerNextLetterSkip(Lexer *lexer) {
  char c = lexerNextLetter(lexer);

  while (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f')
    c = lexerNextLetter(lexer);

  return c;
}

static void lexerSkipLine(Lexer *lexer) {
  i32 line = lexer->line;

  while (line == lexer->line)
    lexerNextLetter(lexer);
}

static char lexerCharPos(Lexer *lexer, char *s, char c) {
  char *p;

  p = strchr(s, c);
  return (p ? p - s : -1);
}