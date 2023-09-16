#include "parser.h"
#include "lexer.h"
#include "logger.h"
#include "utils.h"

static ASTNode parser_make_node(Parser *parser, ASTNodeType type,
                                std::vector<ASTNode> children = {},
                                TokenValue value = {});

static Token &parser_token(Parser *parser);
static void parser_next_token(Parser *parser);
static void parser_prev_token(Parser *parser);
static ASTNodeType parser_arithop(Parser *parser, TokenType type);
static int parser_operation_precedence(Parser *parser, ASTNodeType type);
static bool parser_right_assoc(Parser *parser, TokenType type);
static bool parser_operation_binary(Parser *parser);

static void parser_match(Parser *parser, TokenType type);
static void parser_match_str(Parser *parser);
static void parser_match_type(Parser *parser);

static ASTNode parser_ident(Parser *parser);
static ASTNode parser_prefix(Parser *parser);
static ASTNode parser_postfix(Parser *parser);
static ASTNode parser_literal(Parser *parser);
static ASTNode parser_array_access(Parser *parser);
static ASTNode parser_funccall(Parser *parser);
static ASTNode parser_binexpr(Parser *parse, int pr);
static std::vector<ASTNode> parser_global_statements(Parser *parser);
static std::vector<ASTNode> parser_struct_statements(Parser *parser);

static void parser_semi(Parser *parser);
static void parser_colon(Parser *parser);
static bool parser_colonb(Parser *parser);
static void parser_lbrack(Parser *parser);
static void parser_rbrack(Parser *parser);
static void parser_lparen(Parser *parser);
static void parser_rparen(Parser *parser);
static void parser_lbrace(Parser *parser);
static void parser_rbrace(Parser *parser);

static std::vector<ASTNode> parser_import_statement(Parser *parser);
static ASTNode parser_while_statement(Parser *parser);
static ASTNode parser_for_statement(Parser *parser);
static ASTNode parser_if_statement(Parser *parser);
static ASTNode parser_return_statement(Parser *parser);
static ASTNode parser_continue_statement(Parser *parser);
static ASTNode parser_break_statement(Parser *parser);
static ASTNode parser_print_statement(Parser *parser);

static ASTNode parser_fun_declaration(Parser *parser);
static ASTNode parser_var_declaration(Parser *parser, bool need_type = true);
static ASTNode parser_fun_param_declaration(Parser *parser);
static ASTNode parser_ident_declaration(Parser *parser, bool need_type = true);

static ASTNode parser_structlit(Parser *parser);
static ASTNode parser_block(Parser *parser);
static ASTNode parser_type(Parser *parser);

Parser *parser_create(std::vector<Token> &tokens) {
  Parser *parser = new Parser();
  parser->tokens = &tokens;
  parser->current_token = 0;

  return parser;
}

void parser_destroy(Parser *parser) { delete parser; }

ASTNode parser_build_tree(Parser *parser) {
  return parser_make_node(parser, AST_NODE_TYPE_PROGRAMM,
                          parser_global_statements(parser));
}

static ASTNode parser_make_node(Parser *parser, ASTNodeType type,
                                std::vector<ASTNode> children,
                                TokenValue value) {
  ASTNode node;
  node.type = type;
  node.children = children;
  node.value = value;
  return node;
}

static Token &parser_token(Parser *parser) {
  if (parser->current_token >= parser->tokens->size()) {
    V_FATAL("vlad: recieved end of tokens when trying to get current token");
  }

  return (*parser->tokens)[parser->current_token];
}

static void parser_next_token(Parser *parser) {
  if (parser->current_token == parser->tokens->size()) {
    V_FATAL("recieved end of tokens when trying to get next token");
  }
  parser->current_token++;
}

static void parser_prev_token(Parser *parser) {
  if (parser->current_token == 0) {
    V_FATAL("received start of tokens when trying to get prev token");
  }
  parser->current_token--;
}

static ASTNodeType parser_arithop(Parser *parser, TokenType type) {
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
    V_FATAL("unknown token type");
    token_print(parser_token(parser));
  }
  return o;
}

static int parser_operation_precedence(Parser *parser, ASTNodeType type) {
  if ((int)type > 12) {
    V_FATAL("unknown operation precedence: ", (int)type);
  }

  const int operation_precedence[] = {
      7, 7,       // * /
      6, 6,       // + -
      5, 5, 5, 5, // > < >= <=
      4, 4,       // == !==
      3,          // &&
      2,          // ||
      1,          // =
  };

  return operation_precedence[(int)type];
}

static bool parser_right_assoc(Parser *parser, TokenType type) {
  if (type == TOKEN_TYPE_ASSIGN) {
    return true;
  } else if (type == TOKEN_TYPE_STAR) {
    return true;
  }
  return false;
}

static bool parser_operation_binary(Parser *parser) {
  return (int)parser_token(parser).type > 1 &&
         (int)parser_token(parser).type < 15;
}

static void parser_match(Parser *parser, TokenType type) {
  if (parser_token(parser).type != type) {
    V_FATAL("no matches for provided token type");
  }
  parser_next_token(parser);
}

static bool parser_matchb(Parser *parser, TokenType type) {
  return parser_token(parser).type == type;
}

static void parser_match_str(Parser *parser) {
  if (parser_token(parser).type != TOKEN_TYPE_STRLIT) {
    V_FATAL("token type doesn't match string literal type");
  }
}

static void parser_match_type(Parser *parser) {
  TokenType type = (TokenType)parser_token(parser).type;
  if (type != TOKEN_TYPE_INT && type != TOKEN_TYPE_FLOAT &&
      type != TOKEN_TYPE_CHAR && type != TOKEN_TYPE_VOID &&
      type != TOKEN_TYPE_STRING && type != TOKEN_TYPE_IDENT) {
    V_FATAL("token type doesn't match \"type\" type");
  }
}

static ASTNode parser_prefix(Parser *parser) {
  ASTNode node;
  switch ((TokenType)parser_token(parser).type) {
  case TOKEN_TYPE_EXMARK:
    parser_next_token(parser);
    node =
        parser_make_node(parser, AST_NODE_TYPE_NOT, {parser_literal(parser)});
    break;
  default:
    V_FATAL("token type doesn't match prefix type");
    token_print(parser_token(parser));
  }
  return node;
}

static ASTNode parser_postfix(Parser *parser) {
  TokenValue value = parser_token(parser).value;
  parser_next_token(parser);

  ASTNode node;

  switch (parser_token(parser).type) {
  case TOKEN_TYPE_LPAREN:
    parser_prev_token(parser);
    node = parser_funccall(parser);
    break;
  case TOKEN_TYPE_LBRACK:
    parser_prev_token(parser);
    node = parser_array_access(parser);
    break;
  case TOKEN_TYPE_INC:
    node = parser_make_node(parser, AST_NODE_TYPE_POSTINC, {}, value);
    parser_next_token(parser);
    break;
  case TOKEN_TYPE_DEC:
    node = parser_make_node(parser, AST_NODE_TYPE_POSTDEC, {}, value);
    parser_next_token(parser);
    break;
  default:
    node = parser_make_node(parser, AST_NODE_TYPE_IDENT, {}, value);
  }

  return node;
}

static ASTNode parser_literal(Parser *parser) {
  ASTNode node;

  switch (parser_token(parser).type) {
  case TOKEN_TYPE_INTLIT:
    node = parser_make_node(parser, AST_NODE_TYPE_INTLIT, {},
                            parser_token(parser).value);
    break;
  case TOKEN_TYPE_FLOATLIT:
    node = parser_make_node(parser, AST_NODE_TYPE_FLOATLIT, {},
                            parser_token(parser).value);
    break;
  case TOKEN_TYPE_CHARLIT:
    node = parser_make_node(parser, AST_NODE_TYPE_CHARLIT, {},
                            parser_token(parser).value);
    break;
  case TOKEN_TYPE_STRLIT:
    node = parser_make_node(parser, AST_NODE_TYPE_STRLIT, {},
                            parser_token(parser).value);
    break;
  case TOKEN_TYPE_LPAREN:
    parser_next_token(parser);

    node = parser_binexpr(parser, 0);

    parser_rparen(parser);
    return node;
  case TOKEN_TYPE_LBRACE:
    node = parser_structlit(parser);
    return node;
  case TOKEN_TYPE_PRINT:
    node = parser_print_statement(parser);
    return node;
  case TOKEN_TYPE_IDENT:
    return parser_postfix(parser);
  default:
    return parser_prefix(parser);
  }

  parser_next_token(parser);

  return node;
}

static ASTNode parser_array_access(Parser *parser) {
  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident(parser));

  parser_lbrack(parser);

  nodes.push_back(parser_binexpr(parser, 0));

  parser_rbrack(parser);

  return parser_make_node(parser, AST_NODE_TYPE_ARR_ACCESS, nodes);
}

static ASTNode parser_funccall(Parser *parser) {
  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident(parser));

  parser_lparen(parser);

  while (1) {
    if (parser_token(parser).type == TOKEN_TYPE_COMMA) {
      parser_next_token(parser);
      nodes.push_back(parser_binexpr(parser, 0));
    } else if (parser_token(parser).type == TOKEN_TYPE_RPAREN) {
      parser_next_token(parser);
      break;
    } else {
      nodes.push_back(parser_binexpr(parser, 0));
    }
  }

  return parser_make_node(parser, AST_NODE_TYPE_FUNC_CALL, nodes);
}

static ASTNode parser_binexpr(Parser *parser, int pr) {
  ASTNode left = parser_literal(parser);

  if (!parser_operation_binary(parser)) {
    return left;
  }

  ASTNodeType op = parser_arithop(parser, (TokenType)parser_token(parser).type);

  while ((parser_operation_precedence(parser, op) > pr) ||
         (parser_operation_precedence(parser, op) == pr) &&
             parser_right_assoc(parser, (TokenType)parser_token(parser).type)) {
    std::vector<ASTNode> nodes;

    int p = parser_operation_precedence(parser, op);
    parser_next_token(parser);

    nodes.push_back(left);
    nodes.push_back(parser_binexpr(parser, p));

    left = parser_make_node(parser, op, nodes);

    if (!parser_operation_binary(parser)) {
      return left;
    }
    op = parser_arithop(parser, (TokenType)parser_token(parser).type);
  }

  return left;
}

static std::vector<ASTNode> parser_global_statements(Parser *parser) {
  std::vector<ASTNode> nodes;

  bool run = true;
  while (run) {
    ASTNode node;
    switch (parser_token(parser).type) {
    case TOKEN_TYPE_IMPORT: {
      std::vector<ASTNode> imp_nodes = parser_import_statement(parser);
      nodes.insert(nodes.end(), imp_nodes.begin(), imp_nodes.end());
      continue;
    }
    case TOKEN_TYPE_VAR:
      node = parser_var_declaration(parser);
      parser_semi(parser);
      break;
    case TOKEN_TYPE_IF:
      node = parser_if_statement(parser);
      break;
    case TOKEN_TYPE_FOR:
      node = parser_for_statement(parser);
      break;
    case TOKEN_TYPE_WHILE:
      node = parser_while_statement(parser);
      break;
    case TOKEN_TYPE_FUN:
      node = parser_fun_declaration(parser);
      break;
    case TOKEN_TYPE_PRINT:
      node = parser_print_statement(parser);
      parser_semi(parser);
      break;
      break;
    case TOKEN_TYPE_EOF:
      run = false;
      continue;
    default:
      node = parser_binexpr(parser, 0);
      parser_semi(parser);
    }

    nodes.push_back(node);
  }

  return nodes;
}

static std::vector<ASTNode> parser_struct_statements(Parser *parser) {
  std::vector<ASTNode> nodes;

  bool run = true;
  while (run) {
    ASTNode node;
    switch (parser_token(parser).type) {
    case TOKEN_TYPE_FUN:
      node = parser_fun_declaration(parser);
      break;
    case TOKEN_TYPE_RBRACE:
      run = false;
      continue;
    default:
      V_FATAL("token doesn't match struct statement type");
      token_print(parser_token(parser));
    }
    nodes.push_back(node);
  }

  return nodes;
}

static ASTNode parser_ident(Parser *parser) {
  if (parser_token(parser).type != TOKEN_TYPE_IDENT) {
    V_FATAL("vlad: expect identifier but got %s", parser_token(parser).type);
  }

  ASTNode node = parser_make_node(parser, AST_NODE_TYPE_IDENT, {},
                                  parser_token(parser).value);

  parser_next_token(parser);

  return node;
}

static void parser_semi(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_SEMI);
}

static void parser_colon(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_COLON);
}

static bool parser_colonb(Parser *parser) {
  return parser_matchb(parser, TOKEN_TYPE_COLON);
}

static void parser_lbrack(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_LBRACK);
}

static void parser_rbrack(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_RBRACK);
}

static void parser_lparen(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_LPAREN);
}

static void parser_rparen(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_RPAREN);
}

static void parser_lbrace(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_LBRACE);
}

static void parser_rbrace(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_RBRACE);
}

static ASTNode parser_while_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_WHILE);

  parser_lparen(parser);
  nodes.push_back(parser_binexpr(parser, 0));
  parser_rparen(parser);

  nodes.push_back(parser_block(parser));

  return parser_make_node(parser, AST_NODE_TYPE_WHILE, nodes);
}

static ASTNode parser_for_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_FOR);

  parser_lparen(parser);
  if (parser_token(parser).type == TOKEN_TYPE_VAR) {
    nodes.push_back(parser_var_declaration(parser));
  } else {
    nodes.push_back(parser_binexpr(parser, 0));
  }
  parser_semi(parser);
  nodes.push_back(parser_binexpr(parser, 0));
  parser_semi(parser);
  nodes.push_back(parser_binexpr(parser, 0));
  parser_rparen(parser);

  nodes.push_back(parser_block(parser));

  return parser_make_node(parser, AST_NODE_TYPE_FOR, nodes);
}

static std::vector<ASTNode> parser_import_statement(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_IMPORT);

  parser_match_str(parser);
  TokenValue value = parser_token(parser).value;
  parser_next_token(parser);

  parser_semi(parser);

  char *buf;
  if (!read_file(value.string.c_str(), &buf)) {
    V_FATAL("vlad: failed to read file %s", value.string.c_str());
  }

  Lexer *lexer = lexer_create(buf);

  std::vector<Token> tokens = lexer_scan(lexer);

  Parser *import_parser = parser_create(tokens);

  ASTNode tree = parser_build_tree(import_parser);

  free(buf);

  lexer_destroy(lexer);

  parser_destroy(import_parser);

  return tree.children;
}

static ASTNode parser_if_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_IF);

  parser_lparen(parser);
  nodes.push_back(parser_binexpr(parser, 0));
  parser_rparen(parser);

  nodes.push_back(parser_block(parser));

  if (parser_token(parser).type == TOKEN_TYPE_ELSE) {
    parser_next_token(parser);

    ASTNode value;
    if (parser_token(parser).type == TOKEN_TYPE_IF) {
      value = parser_if_statement(parser);
    } else {
      value = parser_block(parser);
    }

    nodes.push_back(parser_make_node(parser, AST_NODE_TYPE_ELSE, {value}));
  }

  return parser_make_node(parser, AST_NODE_TYPE_IF, nodes);
}

static ASTNode parser_return_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_RETURN);
  if (parser_token(parser).type != TOKEN_TYPE_SEMI) {
    nodes.push_back(parser_binexpr(parser, 0));
  }

  parser_semi(parser);

  return parser_make_node(parser, AST_NODE_TYPE_RETURN, nodes);
}

static ASTNode parser_continue_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_CONTINUE);

  if (parser_token(parser).type == TOKEN_TYPE_IDENT) {
    nodes.push_back(parser_ident(parser));
  }

  parser_semi(parser);

  return parser_make_node(parser, AST_NODE_TYPE_CONTINUE, nodes);
}

static ASTNode parser_break_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_BREAK);

  if (parser_token(parser).type == TOKEN_TYPE_IDENT) {
    nodes.push_back(parser_ident(parser));
  }

  parser_semi(parser);

  return parser_make_node(parser, AST_NODE_TYPE_BREAK, nodes);
}

static ASTNode parser_print_statement(Parser *parser) {
  std::vector<ASTNode> nodes;
  parser_match(parser, TOKEN_TYPE_PRINT);

  parser_lparen(parser);

  nodes.push_back(parser_binexpr(parser, 0));

  while (1) {
    if (parser_token(parser).type == TOKEN_TYPE_COMMA) {
      parser_next_token(parser);
      nodes.push_back(parser_binexpr(parser, 0));
    } else {
      break;
    }
  }

  parser_rparen(parser);

  return parser_make_node(parser, AST_NODE_TYPE_PRINT, nodes);
}

static ASTNode parser_fun_declaration(Parser *parser) {
  parser_match(parser, TOKEN_TYPE_FUN);

  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident(parser));

  parser_lparen(parser);

  while (1) {
    if (parser_token(parser).type == TOKEN_TYPE_IDENT) {
      nodes.push_back(parser_fun_param_declaration(parser));
    } else if (parser_token(parser).type == TOKEN_TYPE_COMMA) {
      parser_next_token(parser);
      nodes.push_back(parser_fun_param_declaration(parser));
    } else {
      break;
    }
  }

  parser_rparen(parser);

  if (parser_token(parser).type == TOKEN_TYPE_ARROW) {
    parser_next_token(parser);
    nodes.push_back(parser_type(parser));
  } else {
    nodes.push_back(parser_make_node(parser, AST_NODE_TYPE_VOID));
  }

  nodes.push_back(parser_block(parser));

  return parser_make_node(parser, AST_NODE_TYPE_FUN, nodes);
}

static ASTNode parser_var_declaration(Parser *parser, bool need_type) {
  parser_match(parser, TOKEN_TYPE_VAR);

  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident_declaration(parser, need_type));
  while (1) {
    if (parser_token(parser).type == TOKEN_TYPE_COMMA) {
      parser_next_token(parser);
      nodes.push_back(parser_ident_declaration(parser, need_type));
    } else {
      break;
    }
  }

  return parser_make_node(parser, AST_NODE_TYPE_VAR, nodes);
}

static ASTNode parser_fun_param_declaration(Parser *parser) {
  TokenValue value = parser_token(parser).value;
  parser_match(parser, TOKEN_TYPE_IDENT);
  bool arr = false;

  std::vector<ASTNode> nodes;
  std::vector<ASTNode> arr_nodes;

  if (parser_token(parser).type == TOKEN_TYPE_LBRACK) {
    parser_next_token(parser);
    if (parser_token(parser).type != TOKEN_TYPE_RBRACK) {
      arr_nodes.push_back(parser_binexpr(parser, 0));
    }
    parser_rbrack(parser);
    arr = true;
  }

  if (parser_colonb(parser)) {
    parser_next_token(parser);
    parser_match_type(parser);
    nodes.push_back(parser_type(parser));
  }

  ASTNode ident = parser_make_node(parser, AST_NODE_TYPE_IDENT, nodes, value);

  if (arr) {
    arr_nodes.push_back(ident);

    ident = parser_make_node(parser, AST_NODE_TYPE_ARRAY, arr_nodes);
  }

  return ident;
}

static ASTNode parser_ident_declaration(Parser *parser, bool need_type) {
  std::vector<ASTNode> ident_nodes;
  std::vector<ASTNode> arr_nodes;

  TokenValue ident_value = parser_token(parser).value;
  parser_match(parser, TOKEN_TYPE_IDENT);
  bool arr = false;
  bool have_type = !need_type;

  if (parser_token(parser).type == TOKEN_TYPE_LBRACK) {
    parser_next_token(parser);
    arr_nodes.push_back(parser_binexpr(parser, 0));
    parser_rbrack(parser);
    arr = true;
  }

  if (parser_colonb(parser)) {
    parser_next_token(parser);
    parser_match_type(parser);
    ident_nodes.push_back(parser_type(parser));
    have_type = true;
  }

  ASTNode ident =
      parser_make_node(parser, AST_NODE_TYPE_IDENT, ident_nodes, ident_value);

  if (arr) {
    arr_nodes.push_back(ident);

    bool init = false;
    if (parser_token(parser).type == TOKEN_TYPE_LBRACE) {
      arr_nodes.push_back(parser_structlit(parser));
      init = true;
    }

    ident = parser_make_node(parser, AST_NODE_TYPE_ARRAY, arr_nodes);
    if (init) {
      return ident;
    }
  }

  if (parser_token(parser).type == TOKEN_TYPE_ASSIGN) {
    std::vector<ASTNode> nodes;
    nodes.push_back(ident);

    parser_next_token(parser);
    nodes.push_back(parser_binexpr(parser, 0));

    ident = parser_make_node(parser, AST_NODE_TYPE_ASSIGN, nodes);
    have_type = have_type || true;
  }

  if (!have_type) {
    V_FATAL("no type is provided for ident declaration");
  }

  return ident;
}

static ASTNode parser_structlit(Parser *parser) {
  parser_lbrace(parser);

  std::vector<ASTNode> nodes;

  while (1) {
    if (parser_token(parser).type == TOKEN_TYPE_COMMA) {
      parser_next_token(parser);
    } else if (parser_token(parser).type == TOKEN_TYPE_RBRACE) {
      break;
    } else {
      nodes.push_back(parser_binexpr(parser, 0));
    }
  }

  parser_rbrace(parser);

  return parser_make_node(parser, AST_NODE_TYPE_STRUCTLIT, nodes);
}

static ASTNode parser_block(Parser *parser) {
  parser_lbrace(parser);

  std::vector<ASTNode> nodes;

  bool run = true;
  while (run) {
    ASTNode node;
    switch (parser_token(parser).type) {
    case TOKEN_TYPE_VAR:
      node = parser_var_declaration(parser);
      parser_semi(parser);
      break;
    case TOKEN_TYPE_IF:
      node = parser_if_statement(parser);
      break;
    case TOKEN_TYPE_FOR:
      node = parser_for_statement(parser);
      break;
    case TOKEN_TYPE_WHILE:
      node = parser_while_statement(parser);
      break;
    case TOKEN_TYPE_FUN:
      node = parser_fun_declaration(parser);
      break;
    case TOKEN_TYPE_RETURN:
      node = parser_return_statement(parser);
      break;
    case TOKEN_TYPE_CONTINUE:
      node = parser_continue_statement(parser);
      break;
    case TOKEN_TYPE_BREAK:
      node = parser_break_statement(parser);
      break;
    case TOKEN_TYPE_PRINT:
      node = parser_print_statement(parser);
      parser_semi(parser);
      break;
    case TOKEN_TYPE_RBRACE:
      parser_next_token(parser);
      run = false;
      continue;
    default:
      node = parser_binexpr(parser, 0);
      parser_semi(parser);
    }

    nodes.push_back(node);
  }

  return parser_make_node(parser, AST_NODE_TYPE_BLOCK, nodes);
}

static ASTNode parser_type(Parser *parser) {
  ASTNode node;
  switch (parser_token(parser).type) {
  case TOKEN_TYPE_INT:
    node = parser_make_node(parser, AST_NODE_TYPE_INT);
    break;
  case TOKEN_TYPE_CHAR:
    node = parser_make_node(parser, AST_NODE_TYPE_CHAR);
    break;
  case TOKEN_TYPE_FLOAT:
    node = parser_make_node(parser, AST_NODE_TYPE_FLOAT);
    break;
  case TOKEN_TYPE_VOID:
    node = parser_make_node(parser, AST_NODE_TYPE_VOID);
    break;
  case TOKEN_TYPE_STRING:
    node = parser_make_node(parser, AST_NODE_TYPE_STRING);
    break;
  case TOKEN_TYPE_IDENT:
    node = parser_make_node(parser, AST_NODE_TYPE_IDENT, {},
                            parser_token(parser).value);
    break;
  default:
    V_FATAL("token type doesn't match \"type\" type");
  }
  parser_next_token(parser);
  if (parser_token(parser).type == TOKEN_TYPE_LBRACK) {
    std::vector<ASTNode> nodes;
    nodes.push_back(node);

    parser_next_token(parser);
    nodes.push_back(parser_binexpr(parser, 0));
    parser_rbrack(parser);

    parser_next_token(parser);

    return parser_make_node(parser, AST_NODE_TYPE_ARRAY, nodes);
  }

  return node;
}
