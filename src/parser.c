#include "parser.h"

#include "file_io.h"
#include "lexer.h"
#include "logger.h"
#include "token.h"
#include "vector.h"

#include <stdlib.h>

static ASTNode parserMakeNode(Parser *parser, ASTNodeType type,
                              ASTNode *children, InterpreterValue value);

static Token *parserToken(Parser *parser);
static void parserNextToken(Parser *parser);
static void parserPrevToken(Parser *parser);
static ASTNodeType parserArithop(Parser *parser, TokenType type);
static i32 parserOperationPrecedence(Parser *parser, ASTNodeType type);
static b8 parserRightAssoc(Parser *parser, TokenType type);
static b8 parserOperationBinary(Parser *parser);

static void parserMatch(Parser *parser, TokenType type);
static b8 parserMatchb(Parser *parser, TokenType type);
static void parserMatchStr(Parser *parser);
static void parserMatchType(Parser *parser);

static ASTNode parserIdent(Parser *parser);
static ASTNode parserPrefix(Parser *parser);
static ASTNode parserPostfix(Parser *parser);
static ASTNode parserLiteral(Parser *parser);
static ASTNode parserArrayAccess(Parser *parser);
static ASTNode parserFunccall(Parser *parser);
static ASTNode parserBinexpr(Parser *parser, i32 pr);
static ASTNode *parserGlobalStatements(Parser *parser);
static ASTNode *parserStructStatements(Parser *parser);

static void parserSemi(Parser *parser);
static void parserColon(Parser *parser);
static b8 parserColonb(Parser *parser);
static void parserLbrack(Parser *parser);
static void parserRbrack(Parser *parser);
static void parserLparen(Parser *parser);
static void parserRparen(Parser *parser);
static void parserLbrace(Parser *parser);
static void parserRbrace(Parser *parser);

static ASTNode *parserImportStatement(Parser *parser);
static ASTNode parserWhileStatement(Parser *parser);
static ASTNode parserForStatement(Parser *parser);
static ASTNode parserIfStatement(Parser *parser);
static ASTNode parserReturnStatement(Parser *parser);
static ASTNode parserContinueStatement(Parser *parser);
static ASTNode parserBreakStatement(Parser *parser);
static ASTNode parserPrintStatement(Parser *parser);

static ASTNode parserFunDeclaration(Parser *parser);
static ASTNode parserVarDeclaration(Parser *parser, b8 need_type);
static ASTNode parserFunParamDeclaration(Parser *parser);
static ASTNode parserIdentDeclaration(Parser *parser, b8 need_type);

static ASTNode parserStructlit(Parser *parser);
static ASTNode parserBlock(Parser *parser);
static ASTNode parserType(Parser *parser);

void parserCreate(Token *tokens, Parser *out_parser) {
  out_parser->tokens = tokens;
  out_parser->current_token = 0;
}

void parserDestroy(Parser *parser) {
  parser->tokens = 0;
  parser->current_token = 0;
}

ASTNode parserBuildAST(Parser *parser) {
  InterpreterValue interpreter_value = {};

  return parserMakeNode(parser, AST_NODE_TYPE_PROGRAMM,
                        parserGlobalStatements(parser), interpreter_value);
}

static ASTNode parserMakeNode(Parser *parser, ASTNodeType type,
                              ASTNode *children, InterpreterValue value) {
  ASTNode node = {};
  node.type = type;
  node.children = children;
  node.value = value;

  return node;
}

static Token *parserToken(Parser *parser) {
  if (parser->current_token >= vectorLength(parser->tokens)) {
    FATAL("liv: recieved end of tokens when trying to get current token!");
  }

  return &parser->tokens[parser->current_token];
}

static void parserNextToken(Parser *parser) {
  if (parser->current_token == vectorLength(parser->tokens)) {
    FATAL("liv: recieved end of tokens when trying to get next token!");
  }

  parser->current_token++;
}

static void parserPrevToken(Parser *parser) {
  if (parser->current_token == 0) {
    FATAL("liv: received start of tokens when trying to get prev token!");
  }

  parser->current_token--;
}

static ASTNodeType parserArithop(Parser *parser, TokenType type) {
  ASTNodeType o;
  switch (type) {
  case TOKEN_TYPE_STAR:
    o = AST_NODE_TYPE_MULT;
    break;
  case TOKEN_TYPE_SLASH:
    o = AST_NODE_TYPE_DIV;
    break;
  case TOKEN_TYPE_PLUS:
    o = AST_NODE_TYPE_PLUS;
    break;
  case TOKEN_TYPE_MINUS:
    o = AST_NODE_TYPE_MINUS;
    break;
  case TOKEN_TYPE_GT:
    o = AST_NODE_TYPE_GT;
    break;
  case TOKEN_TYPE_LT:
    o = AST_NODE_TYPE_LT;
    break;
  case TOKEN_TYPE_GE:
    o = AST_NODE_TYPE_GE;
    break;
  case TOKEN_TYPE_LE:
    o = AST_NODE_TYPE_LE;
    break;
  case TOKEN_TYPE_EQ:
    o = AST_NODE_TYPE_EQ;
    break;
  case TOKEN_TYPE_NE:
    o = AST_NODE_TYPE_NE;
    break;
  case TOKEN_TYPE_ASSIGN:
    o = AST_NODE_TYPE_ASSIGN;
    break;
  case TOKEN_TYPE_AND:
    o = AST_NODE_TYPE_AND;
    break;
  case TOKEN_TYPE_OR:
    o = AST_NODE_TYPE_OR;
    break;
  default:
    FATAL("liv: unknown token type!");
    tokenPrint(parserToken(parser));
  }

  return o;
}

static i32 parserOperationPrecedence(Parser *parser, ASTNodeType type) {
  if ((i32)type > 12) {
    FATAL("liv: unknown operation precedence: %d!", (i32)type);
  }

  const i32 operation_precedence[] = {
      7, 7,       // * /
      6, 6,       // + -
      5, 5, 5, 5, // > < >= <=
      4, 4,       // == !==
      3,          // &&
      2,          // ||
      1,          // =
  };

  return operation_precedence[(i32)type];
}

static b8 parserRightAssoc(Parser *parser, TokenType type) {
  if (type == TOKEN_TYPE_ASSIGN || type == TOKEN_TYPE_STAR) {
    return true;
  }

  return false;
}

static b8 parserOperationBinary(Parser *parser) {
  return (i32)parserToken(parser)->type > 1 &&
         (i32)parserToken(parser)->type < 15;
}

static void parserMatch(Parser *parser, TokenType type) {
  if (parserToken(parser)->type != type) {
    FATAL("liv: no matches for provided token type!");
  }

  parserNextToken(parser);
}

static b8 parserMatchb(Parser *parser, TokenType type) {
  return parserToken(parser)->type == type;
}

static void parserMatchStr(Parser *parser) {
  if (parserToken(parser)->type != TOKEN_TYPE_STRLIT) {
    FATAL("liv: token type doesn't match string literal type!");
  }
}

static void parserMatchType(Parser *parser) {
  TokenType type = (TokenType)parserToken(parser)->type;
  if (type != TOKEN_TYPE_INT && type != TOKEN_TYPE_FLOAT &&
      type != TOKEN_TYPE_CHAR && type != TOKEN_TYPE_VOID &&
      type != TOKEN_TYPE_STRING && type != TOKEN_TYPE_IDENT) {
    FATAL("liv: token type doesn't match type!");
  }
}

static ASTNode parserIdent(Parser *parser) {
  if (parserToken(parser)->type != TOKEN_TYPE_IDENT) {
    FATAL("liv: expect identifier but got %s", parserToken(parser)->type);
  }

  ASTNode node = parserMakeNode(parser, AST_NODE_TYPE_IDENT, 0,
                                parserToken(parser)->value);

  parserNextToken(parser);

  return node;
}

static ASTNode parserPrefix(Parser *parser) {
  ASTNode node = {};
  switch (parserToken(parser)->type) {
  case TOKEN_TYPE_EXMARK:
    parserNextToken(parser);
    ASTNode *nodes = vectorCreate(ASTNode);
    vectorPush(nodes, parserLiteral(parser));
    InterpreterValue interpreter_value = {};
    node = parserMakeNode(parser, AST_NODE_TYPE_NOT, nodes, interpreter_value);
    break;
  default:
    FATAL("liv: token type doesn't match prefix type!");
    tokenPrint(parserToken(parser));
  }

  return node;
}

static ASTNode parserPostfix(Parser *parser) {
  InterpreterValue value = parserToken(parser)->value;
  parserNextToken(parser);

  ASTNode node = {};

  switch (parserToken(parser)->type) {
  case TOKEN_TYPE_LPAREN:
    parserPrevToken(parser);
    node = parserFunccall(parser);
    break;
  case TOKEN_TYPE_LBRACK:
    parserPrevToken(parser);
    node = parserArrayAccess(parser);
    break;
  case TOKEN_TYPE_INC:
    node = parserMakeNode(parser, AST_NODE_TYPE_POSTINC, 0, value);
    parserNextToken(parser);
    break;
  case TOKEN_TYPE_DEC:
    node = parserMakeNode(parser, AST_NODE_TYPE_POSTDEC, 0, value);
    parserNextToken(parser);
    break;
  default:
    node = parserMakeNode(parser, AST_NODE_TYPE_IDENT, 0, value);
  }

  return node;
}

static ASTNode parserLiteral(Parser *parser) {
  ASTNode node = {};

  switch (parserToken(parser)->type) {
  case TOKEN_TYPE_INTLIT:
    node = parserMakeNode(parser, AST_NODE_TYPE_INTLIT, 0,
                          parserToken(parser)->value);
    break;
  case TOKEN_TYPE_FLOATLIT:
    node = parserMakeNode(parser, AST_NODE_TYPE_FLOATLIT, 0,
                          parserToken(parser)->value);
    break;
  case TOKEN_TYPE_CHARLIT:
    node = parserMakeNode(parser, AST_NODE_TYPE_CHARLIT, 0,
                          parserToken(parser)->value);
    break;
  case TOKEN_TYPE_STRLIT:
    node = parserMakeNode(parser, AST_NODE_TYPE_STRLIT, 0,
                          parserToken(parser)->value);
    break;
  case TOKEN_TYPE_LPAREN:
    parserNextToken(parser);

    node = parserBinexpr(parser, 0);

    parserRparen(parser);
    return node;
  case TOKEN_TYPE_LBRACE:
    node = parserStructlit(parser);
    return node;
  case TOKEN_TYPE_PRINT:
    node = parserPrintStatement(parser);
    return node;
  case TOKEN_TYPE_IDENT:
    return parserPostfix(parser);
  default:
    return parserPrefix(parser);
  }

  parserNextToken(parser);

  return node;
}

static ASTNode parserArrayAccess(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  vectorPush(nodes, parserIdent(parser));

  parserLbrack(parser);

  vectorPush(nodes, parserBinexpr(parser, 0));

  parserRbrack(parser);

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_ARR_ACCESS, nodes,
                        interpreter_value);
}

static ASTNode parserFunccall(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  vectorPush(nodes, parserIdent(parser));

  parserLparen(parser);

  while (1) {
    if (parserToken(parser)->type == TOKEN_TYPE_COMMA) {
      parserNextToken(parser);
      vectorPush(nodes, parserBinexpr(parser, 0))
    } else if (parserToken(parser)->type == TOKEN_TYPE_RPAREN) {
      parserNextToken(parser);
      break;
    } else {
      vectorPush(nodes, parserBinexpr(parser, 0))
    }
  }

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_FUNC_CALL, nodes,
                        interpreter_value);
}

static ASTNode parserBinexpr(Parser *parser, i32 pr) {
  ASTNode left = parserLiteral(parser);

  if (!parserOperationBinary(parser)) {
    return left;
  }

  ASTNodeType op = parserArithop(parser, parserToken(parser)->type);

  while ((parserOperationPrecedence(parser, op) > pr) ||
         (parserOperationPrecedence(parser, op) == pr) &&
             parserRightAssoc(parser, parserToken(parser)->type)) {
    ASTNode *nodes = vectorCreate(ASTNode);

    i32 p = parserOperationPrecedence(parser, op);
    parserNextToken(parser);

    vectorPush(nodes, left);
    vectorPush(nodes, parserBinexpr(parser, p));

    InterpreterValue interpreter_value = {};
    left = parserMakeNode(parser, op, nodes, interpreter_value);

    if (!parserOperationBinary(parser)) {
      return left;
    }
    op = parserArithop(parser, parserToken(parser)->type);
  }

  return left;
}

static ASTNode *parserGlobalStatements(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);

  b8 run = true;
  while (run) {
    ASTNode node = {};
    switch (parserToken(parser)->type) {
    case TOKEN_TYPE_IMPORT: {
      ASTNode *imp_nodes = parserImportStatement(parser);
      for (u32 i = 0; i < vectorLength(imp_nodes); ++i) {
        vectorPush(nodes, imp_nodes[i]);
      }
      continue;
    }
    case TOKEN_TYPE_VAR:
      node = parserVarDeclaration(parser, true);
      parserSemi(parser);
      break;
    case TOKEN_TYPE_IF:
      node = parserIfStatement(parser);
      break;
    case TOKEN_TYPE_FOR:
      node = parserForStatement(parser);
      break;
    case TOKEN_TYPE_WHILE:
      node = parserWhileStatement(parser);
      break;
    case TOKEN_TYPE_FUN:
      node = parserFunDeclaration(parser);
      break;
    case TOKEN_TYPE_PRINT:
      node = parserPrintStatement(parser);
      parserSemi(parser);
      break;
      break;
    case TOKEN_TYPE_EOF:
      run = false;
      continue;
    default:
      node = parserBinexpr(parser, 0);
      parserSemi(parser);
    }

    vectorPush(nodes, node);
  }

  return nodes;
}

static ASTNode *parserStructStatements(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);

  b8 run = true;
  while (run) {
    ASTNode node = {};
    switch (parserToken(parser)->type) {
    case TOKEN_TYPE_FUN:
      node = parserFunDeclaration(parser);
      break;
    case TOKEN_TYPE_RBRACE:
      run = false;
      continue;
    default:
      FATAL("liv: token doesn't match struct statement type!");
      tokenPrint(parserToken(parser));
    }

    vectorPush(nodes, node);
  }

  return nodes;
}

static void parserSemi(Parser *parser) { parserMatch(parser, TOKEN_TYPE_SEMI); }

static void parserColon(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_COLON);
}

static b8 parserColonb(Parser *parser) {
  return parserMatchb(parser, TOKEN_TYPE_COLON);
}

static void parserLbrack(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_LBRACK);
}

static void parserRbrack(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_RBRACK);
}

static void parserLparen(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_LPAREN);
}

static void parserRparen(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_RPAREN);
}

static void parserLbrace(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_LBRACE);
}

static void parserRbrace(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_RBRACE);
}

static ASTNode *parserImportStatement(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_IMPORT);

  parserMatchStr(parser);
  InterpreterValue value = parserToken(parser)->value;
  parserNextToken(parser);

  parserSemi(parser);

  char *buf;
  if (!readFile(value.string, &buf)) {
    FATAL("liv: failed to read file %s!", value.string);
  }

  Lexer lexer = {};
  lexerCreate(buf, &lexer);

  Token *tokens = lexerScan(&lexer);

  Parser import_parser;
  parserCreate(tokens, &import_parser);

  ASTNode tree = parserBuildAST(&import_parser);

  free(buf);
  lexerDestroy(&lexer);
  vectorDestroy(tokens);
  parserDestroy(&import_parser);

  return tree.children;
}

static ASTNode parserWhileStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_WHILE);

  parserLparen(parser);
  vectorPush(nodes, parserBinexpr(parser, 0));
  parserRparen(parser);
  vectorPush(nodes, parserBlock(parser));

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_WHILE, nodes, interpreter_value);
}

static ASTNode parserForStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_FOR);

  parserLparen(parser);
  if (parserToken(parser)->type == TOKEN_TYPE_VAR) {
    vectorPush(nodes, parserVarDeclaration(parser, true));
  } else {
    vectorPush(nodes, parserBinexpr(parser, 0));
  }
  parserSemi(parser);
  vectorPush(nodes, parserBinexpr(parser, 0));
  parserSemi(parser);
  vectorPush(nodes, parserBinexpr(parser, 0));
  parserRparen(parser);

  vectorPush(nodes, parserBlock(parser));

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_FOR, nodes, interpreter_value);
}

static ASTNode parserIfStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_IF);

  parserLparen(parser);
  vectorPush(nodes, parserBinexpr(parser, 0));
  parserRparen(parser);
  vectorPush(nodes, parserBlock(parser));

  if (parserToken(parser)->type == TOKEN_TYPE_ELSE) {
    parserNextToken(parser);

    ASTNode value = {};
    if (parserToken(parser)->type == TOKEN_TYPE_IF) {
      value = parserIfStatement(parser);
    } else {
      value = parserBlock(parser);
    }

    ASTNode *chilren = vectorCreate(ASTNode);
    vectorPush(chilren, value);

    InterpreterValue interpreter_value = {};
    vectorPush(nodes, parserMakeNode(parser, AST_NODE_TYPE_ELSE, chilren,
                                     interpreter_value));
  }

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_IF, nodes, interpreter_value);
}

static ASTNode parserReturnStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_RETURN);
  if (parserToken(parser)->type != TOKEN_TYPE_SEMI) {
    vectorPush(nodes, parserBinexpr(parser, 0));
  }

  parserSemi(parser);

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_RETURN, nodes, interpreter_value);
}

static ASTNode parserContinueStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_CONTINUE);

  if (parserToken(parser)->type == TOKEN_TYPE_IDENT) {
    vectorPush(nodes, parserIdent(parser));
  }

  parserSemi(parser);

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_CONTINUE, nodes,
                        interpreter_value);
}

static ASTNode parserBreakStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_BREAK);

  if (parserToken(parser)->type == TOKEN_TYPE_IDENT) {
    vectorPush(nodes, parserIdent(parser));
  }

  parserSemi(parser);

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_BREAK, nodes, interpreter_value);
}

static ASTNode parserPrintStatement(Parser *parser) {
  ASTNode *nodes = vectorCreate(ASTNode);
  parserMatch(parser, TOKEN_TYPE_PRINT);

  parserLparen(parser);

  vectorPush(nodes, parserBinexpr(parser, 0));

  while (1) {
    if (parserToken(parser)->type == TOKEN_TYPE_COMMA) {
      parserNextToken(parser);
      vectorPush(nodes, parserBinexpr(parser, 0));
    } else {
      break;
    }
  }

  parserRparen(parser);

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_PRINT, nodes, interpreter_value);
}

static ASTNode parserFunDeclaration(Parser *parser) {
  parserMatch(parser, TOKEN_TYPE_FUN);

  ASTNode *nodes = vectorCreate(ASTNode);
  vectorPush(nodes, parserIdent(parser));

  parserLparen(parser);

  while (1) {
    if (parserToken(parser)->type == TOKEN_TYPE_IDENT) {
      vectorPush(nodes, parserFunParamDeclaration(parser));
    } else if (parserToken(parser)->type == TOKEN_TYPE_COMMA) {
      parserNextToken(parser);
      vectorPush(nodes, parserFunParamDeclaration(parser));
    } else {
      break;
    }
  }

  parserRparen(parser);

  if (parserToken(parser)->type == TOKEN_TYPE_ARROW) {
    parserNextToken(parser);
    vectorPush(nodes, parserType(parser));
  } else {
    InterpreterValue interpreter_value = {};
    vectorPush(nodes, parserMakeNode(parser, AST_NODE_TYPE_VOID, 0,
                                     interpreter_value));
  }

  vectorPush(nodes, parserBlock(parser));

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_FUN, nodes, interpreter_value);
}

static ASTNode parserVarDeclaration(Parser *parser, b8 need_type) {
  parserMatch(parser, TOKEN_TYPE_VAR);

  ASTNode *nodes = vectorCreate(ASTNode);
  vectorPush(nodes, parserIdentDeclaration(parser, need_type));

  while (1) {
    if (parserToken(parser)->type == TOKEN_TYPE_COMMA) {
      parserNextToken(parser);
      vectorPush(nodes, parserIdentDeclaration(parser, need_type));
    } else {
      break;
    }
  }

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_VAR, nodes, interpreter_value);
}

static ASTNode parserFunParamDeclaration(Parser *parser) {
  InterpreterValue value = parserToken(parser)->value;
  parserMatch(parser, TOKEN_TYPE_IDENT);
  b8 arr = false;

  ASTNode *nodes = vectorCreate(ASTNode);
  ASTNode *arr_nodes = vectorCreate(ASTNode);

  if (parserToken(parser)->type == TOKEN_TYPE_LBRACK) {
    parserNextToken(parser);
    if (parserToken(parser)->type != TOKEN_TYPE_RBRACK) {
      vectorPush(arr_nodes, parserBinexpr(parser, 0));
    }

    parserRbrack(parser);
    arr = true;
  }

  if (parserColonb(parser)) {
    parserNextToken(parser);
    parserMatchType(parser);

    vectorPush(nodes, parserType(parser));
  }

  ASTNode ident = parserMakeNode(parser, AST_NODE_TYPE_IDENT, nodes, value);

  if (arr) {
    vectorPush(arr_nodes, ident);

    InterpreterValue interpreter_value = {};
    ident = parserMakeNode(parser, AST_NODE_TYPE_ARRAY, arr_nodes,
                           interpreter_value);
  } else {
    vectorDestroy(arr_nodes);
  }

  return ident;
}

static ASTNode parserIdentDeclaration(Parser *parser, b8 need_type) {
  ASTNode *ident_nodes = vectorCreate(ASTNode);
  ASTNode *arr_nodes = vectorCreate(ASTNode);

  InterpreterValue ident_value = parserToken(parser)->value;
  parserMatch(parser, TOKEN_TYPE_IDENT);
  b8 arr = false;
  b8 have_type = !need_type;

  if (parserToken(parser)->type == TOKEN_TYPE_LBRACK) {
    parserNextToken(parser);
    vectorPush(arr_nodes, parserBinexpr(parser, 0));
    parserRbrack(parser);
    arr = true;
  }

  if (parserColonb(parser)) {
    parserNextToken(parser);
    parserMatchType(parser);

    vectorPush(ident_nodes, parserType(parser));
    have_type = true;
  }

  ASTNode ident =
      parserMakeNode(parser, AST_NODE_TYPE_IDENT, ident_nodes, ident_value);

  if (arr) {
    vectorPush(arr_nodes, ident);

    b8 init = false;
    if (parserToken(parser)->type == TOKEN_TYPE_LBRACE) {
      vectorPush(arr_nodes, parserStructlit(parser));
      init = true;
    }

    InterpreterValue interpreter_value = {};
    ident = parserMakeNode(parser, AST_NODE_TYPE_ARRAY, arr_nodes,
                           interpreter_value);
    if (init) {
      return ident;
    }
  } else {
    vectorDestroy(arr_nodes);
  }

  if (parserToken(parser)->type == TOKEN_TYPE_ASSIGN) {
    ASTNode *nodes = vectorCreate(ASTNode);
    vectorPush(nodes, ident);

    parserNextToken(parser);
    vectorPush(nodes, parserBinexpr(parser, 0));

    InterpreterValue interpreter_value = {};
    ident =
        parserMakeNode(parser, AST_NODE_TYPE_ASSIGN, nodes, interpreter_value);
    have_type = have_type || true;
  }

  if (!have_type) {
    FATAL("liv: no type is provided for ident declaration!");
  }

  return ident;
}

static ASTNode parserStructlit(Parser *parser) {
  parserLbrace(parser);

  ASTNode *nodes = vectorCreate(ASTNode);

  while (1) {
    if (parserToken(parser)->type == TOKEN_TYPE_COMMA) {
      parserNextToken(parser);
    } else if (parserToken(parser)->type == TOKEN_TYPE_RBRACE) {
      break;
    } else {
      vectorPush(nodes, parserBinexpr(parser, 0));
    }
  }

  parserRbrace(parser);

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_STRUCTLIT, nodes,
                        interpreter_value);
}

static ASTNode parserBlock(Parser *parser) {
  parserLbrace(parser);

  ASTNode *nodes = vectorCreate(ASTNode);

  b8 run = true;
  while (run) {
    ASTNode node;
    switch (parserToken(parser)->type) {
    case TOKEN_TYPE_VAR:
      node = parserVarDeclaration(parser, true);
      parserSemi(parser);
      break;
    case TOKEN_TYPE_IF:
      node = parserIfStatement(parser);
      break;
    case TOKEN_TYPE_FOR:
      node = parserForStatement(parser);
      break;
    case TOKEN_TYPE_WHILE:
      node = parserWhileStatement(parser);
      break;
    case TOKEN_TYPE_FUN:
      node = parserFunDeclaration(parser);
      break;
    case TOKEN_TYPE_RETURN:
      node = parserReturnStatement(parser);
      break;
    case TOKEN_TYPE_CONTINUE:
      node = parserContinueStatement(parser);
      break;
    case TOKEN_TYPE_BREAK:
      node = parserBreakStatement(parser);
      break;
    case TOKEN_TYPE_PRINT:
      node = parserPrintStatement(parser);
      parserSemi(parser);
      break;
    case TOKEN_TYPE_RBRACE:
      parserNextToken(parser);
      run = false;
      continue;
    default:
      node = parserBinexpr(parser, 0);
      parserSemi(parser);
    }

    vectorPush(nodes, node);
  }

  InterpreterValue interpreter_value = {};
  return parserMakeNode(parser, AST_NODE_TYPE_BLOCK, nodes, interpreter_value);
}

static ASTNode parserType(Parser *parser) {
  ASTNode node = {};
  InterpreterValue interpreter_value = {};
  switch (parserToken(parser)->type) {
  case TOKEN_TYPE_INT:
    node = parserMakeNode(parser, AST_NODE_TYPE_INT, 0, interpreter_value);
    break;
  case TOKEN_TYPE_CHAR:
    node = parserMakeNode(parser, AST_NODE_TYPE_CHAR, 0, interpreter_value);
    break;
  case TOKEN_TYPE_FLOAT:
    node = parserMakeNode(parser, AST_NODE_TYPE_FLOAT, 0, interpreter_value);
    break;
  case TOKEN_TYPE_VOID:
    node = parserMakeNode(parser, AST_NODE_TYPE_VOID, 0, interpreter_value);
    break;
  case TOKEN_TYPE_STRING:
    node = parserMakeNode(parser, AST_NODE_TYPE_STRING, 0, interpreter_value);
    break;
  case TOKEN_TYPE_IDENT:
    node = parserMakeNode(parser, AST_NODE_TYPE_IDENT, 0,
                          parserToken(parser)->value);
    break;
  default:
    FATAL("liv: token type doesn't match the type!");
  }
  parserNextToken(parser);
  if (parserToken(parser)->type == TOKEN_TYPE_LBRACK) {
    ASTNode *nodes = vectorCreate(ASTNode);
    vectorPush(nodes, node);

    parserNextToken(parser);
    vectorPush(nodes, parserBinexpr(parser, 0));
    parserRbrack(parser);

    parserNextToken(parser);

    InterpreterValue interpreter_value = {};
    return parserMakeNode(parser, AST_NODE_TYPE_ARRAY, nodes,
                          interpreter_value);
  }

  return node;
}
