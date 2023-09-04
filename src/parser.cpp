#include "parser.h"

#include "logger.h"

typedef struct Parser {
  std::vector<Token> *tokens;
  u32 current_token;
} Parser;

static Parser *parser;

b8 parser_system_initialize(std::vector<Token> &tokens) {
  V_ASSERT_MSG(!parser, "vlad: parser already initialized!");

  parser = (Parser *)malloc(sizeof(*parser));
  parser->tokens = &tokens;
  parser->current_token = 0;

  return true;
}

void parser_system_shutdown() { free(parser); }

ASTNode parser_build_tree() { 
  return parser_make_node(AST_NODE_TYPE_PROGRAMM, parser_global_statements()); 
}

ASTNode parser_make_node(ASTNodeType type, std::vector<ASTNode> children, TokenValue value) {
  ASTNode node;
  node.type = type;
  node.children = children;
  node.value = value;
  return node;
}

Token &parser_token() {
  if (parser->current_token >= parser->tokens->size()) {
    V_FATAL("vlad: recieved end of tokens when trying to get current token");
  }

  return (*parser->tokens)[parser->current_token];
}

void parser_next_token() {
  if (parser->current_token == parser->tokens->size()) {
    V_FATAL("recieved end of tokens when trying to get next token");
  }
  parser->current_token++;
}

void parser_prev_token() {
  if (parser->current_token == 0) {
	  V_FATAL("received start of tokens when trying to get prev token");
  }
  parser->current_token--;
}

ASTNodeType parser_arithop(TokenType type) {
  ASTNodeType o;
  switch(type) {
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
    token_print(parser_token());
  }
  return o;
}

int parser_operation_precedence(ASTNodeType type) {
  if ((int)type > 12) {
    V_FATAL("unknown operation precedence: ", (int)type);
  }

  const int operation_precedence[] = {
    7, 7, // * /
    6, 6, // + -
    5, 5, 5, 5, // > < >= <=
    4, 4, // == !==
    3, // &&
    2, // ||
    1, // =
  };

  return operation_precedence[(int)type];
}

bool parser_right_assoc(TokenType type) {
  if (type == TOKEN_TYPE_ASSIGN) {
	  return true;
  } else if (type == TOKEN_TYPE_STAR) {
	  return true;
  }
  return false;
}

bool parser_operation_binary() {
  return (int)parser_token().type > 1 && (int)parser_token().type < 15;
}

void parser_match(TokenType type) {  
  if (parser_token().type != type) {
    V_FATAL("no matches for provided token type");
  }
  parser_next_token();
}

bool parser_matchb(TokenType type) {
  return parser_token().type == type;
}

void parser_match_str() {
  if (parser_token().type != TOKEN_TYPE_STRLIT) {
	  V_FATAL("token type doesn't match string literal type");
  }
}

void parser_match_type() {
  TokenType type = (TokenType)parser_token().type;
  if(type != TOKEN_TYPE_INT
     && type != TOKEN_TYPE_FLOAT
     && type != TOKEN_TYPE_CHAR
     && type != TOKEN_TYPE_VOID
     && type != TOKEN_TYPE_STRING
     && type != TOKEN_TYPE_IDENT) {
	  V_FATAL("token type doesn't match \"type\" type");
  }
}

ASTNode parser_prefix() {
  ASTNode node;
  switch((TokenType)parser_token().type) {
  case TOKEN_TYPE_EXMARK:
    parser_next_token();
    node = parser_make_node(AST_NODE_TYPE_NOT, { parser_literal() });
    break;
  default:    
    V_FATAL("token type doesn't match prefix type");
    token_print(parser_token());
  }
  return node;
}

ASTNode parser_postfix() {
  TokenValue value = parser_token().value;
  parser_next_token();

  ASTNode node;
    
  switch(parser_token().type) {
  case TOKEN_TYPE_LPAREN:
	  parser_prev_token();
  	node = parser_funccall();
	  break;
  case TOKEN_TYPE_LBRACK:
	  parser_prev_token();
  	node = parser_array_access();
    break;
  case TOKEN_TYPE_INC:
	  node = parser_make_node(AST_NODE_TYPE_POSTINC, {}, value);
  	parser_next_token();
	  break;
  case TOKEN_TYPE_DEC:
	  node = parser_make_node(AST_NODE_TYPE_POSTDEC, {}, value);
  	parser_next_token();
	  break;
  case TOKEN_TYPE_DOT:
	  parser_next_token();
  	node = parser_make_node(AST_NODE_TYPE_MEMBER_ACCESS, { parser_postfix() }, value);
	  break;
  case TOKEN_TYPE_LBRACE:
	  node = parser_make_node(AST_NODE_TYPE_STRUCT_INIT, { parser_structlit() }, value);
  	break;
  default: node = parser_make_node(AST_NODE_TYPE_IDENT, {}, value);
  }

  return node;
}

ASTNode parser_literal() {
  ASTNode node;
    
  switch(parser_token().type) {
  case TOKEN_TYPE_INTLIT:
	  node = parser_make_node(AST_NODE_TYPE_INTLIT, {}, parser_token().value);
  	break;
  case TOKEN_TYPE_FLOATLIT:
	  node = parser_make_node(AST_NODE_TYPE_FLOATLIT, {}, parser_token().value);
  	break;	
  case TOKEN_TYPE_CHARLIT:
	  node = parser_make_node(AST_NODE_TYPE_CHARLIT, {}, parser_token().value);
  	break;
  case TOKEN_TYPE_STRLIT:
	  node = parser_make_node(AST_NODE_TYPE_STRLIT, {}, parser_token().value);
  	break;
  case TOKEN_TYPE_LPAREN:
	  parser_next_token();

  	node = parser_binexpr(0);

	  parser_rparen();
  	return node;
  case TOKEN_TYPE_LBRACE:	       
	  node = parser_structlit();	
  	return node;
  case TOKEN_TYPE_PRINT:
	  node = parser_print_statement();
  	return node;
  case TOKEN_TYPE_IDENT:
	  return parser_postfix();
  default: return parser_prefix();
  }
    
  parser_next_token();
    
  return node;
}

ASTNode parser_array_access() {
  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident());
  
  parser_lbrack();

  nodes.push_back(parser_binexpr(0));

  parser_rbrack();

  return parser_make_node(AST_NODE_TYPE_ARR_ACCESS, nodes);
}

ASTNode parser_funccall() {
  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident());
  
  parser_lparen();

  while(1) {
    if (parser_token().type == TOKEN_TYPE_COMMA) {
      parser_next_token();
      nodes.push_back(parser_binexpr(0));
    } else if (parser_token().type == TOKEN_TYPE_RPAREN) {
      parser_next_token();
      break;
    } else {
      nodes.push_back(parser_binexpr(0));
    }
  }

  return parser_make_node(AST_NODE_TYPE_FUNC_CALL, nodes);
}

ASTNode parser_binexpr(int pr) {
  ASTNode left = parser_literal();
  
  if (!parser_operation_binary())
    return left;

  ASTNodeType op = parser_arithop((TokenType)parser_token().type);

  while((parser_operation_precedence(op) > pr)
    || (parser_operation_precedence(op) == pr) && parser_right_assoc((TokenType)parser_token().type)) {
    std::vector<ASTNode> nodes;

    int p = parser_operation_precedence(op);
    parser_next_token();

    nodes.push_back(left);
    nodes.push_back(parser_binexpr(p));

    left = parser_make_node(op, nodes);

    if (!parser_operation_binary())
      return left;
    op = parser_arithop((TokenType)parser_token().type);
  }

  return left;
}


std::vector<ASTNode> parser_global_statements() {
  std::vector<ASTNode> nodes;

  bool run = true;
  while(run) {
    ASTNode node;
    switch(parser_token().type) {
    case TOKEN_TYPE_IMPORT: {	   
      std::vector<ASTNode> imp_nodes = parser_import_statement();
      nodes.insert(nodes.end(), imp_nodes.begin(), imp_nodes.end());	    
      continue;
    }
    case TOKEN_TYPE_VAR:
      node = parser_var_declaration();	    
      parser_semi();
      break;
    case TOKEN_TYPE_IF:
      node = parser_if_statement();
      break;
    case TOKEN_TYPE_FOR:
      node = parser_for_statement();
      break;	
    case TOKEN_TYPE_WHILE:
      node = parser_while_statement();
      break;	
    case TOKEN_TYPE_FUN:
      node = parser_fun_declaration();
      break;
    case TOKEN_TYPE_PRINT:
      node = parser_print_statement();
      parser_semi();
      break;
    case TOKEN_TYPE_STRUCT:
      node = parser_struct_declaration();
      break;
    case TOKEN_TYPE_EOF:
      run = false;
      continue;
    default:
      node = parser_binexpr(0);
      parser_semi();
    }

    nodes.push_back(node);
  }

  return nodes;
}

std::vector<ASTNode> parser_struct_statements() {
  std::vector<ASTNode> nodes;

  bool run = true;
  while(run) {
    ASTNode node;
    switch(parser_token().type) {
    case TOKEN_TYPE_IDENT:
      node = parser_struct_member_declaration();
      break;
    case TOKEN_TYPE_FUN:
      node = parser_fun_declaration();
      break;
    case TOKEN_TYPE_RBRACE:
      run = false;
      continue;
    default:
      V_FATAL("token doesn't match struct statement type");
      token_print(parser_token());
    }
    nodes.push_back(node);
  }

  return nodes;
}

ASTNode parser_ident() {
  if (parser_token().type != TOKEN_TYPE_IDENT) {
    V_FATAL("vlad: expect identifier but got %s", parser_token().type);
  }

  ASTNode node = parser_make_node(AST_NODE_TYPE_IDENT, {}, parser_token().value);

  parser_next_token();

  return node;
}

void parser_semi() {
  parser_match(TOKEN_TYPE_SEMI);
}

void parser_colon() {
  parser_match(TOKEN_TYPE_COLON);
}

bool parser_colonb() {
  return parser_matchb(TOKEN_TYPE_COLON);
}

void parser_lbrack() {
  parser_match(TOKEN_TYPE_LBRACK);
}

void parser_rbrack() {
  parser_match(TOKEN_TYPE_RBRACK);
}

void parser_lparen() {
  parser_match(TOKEN_TYPE_LPAREN);
}

void parser_rparen() {
  parser_match(TOKEN_TYPE_RPAREN);
}

void parser_lbrace() {
  parser_match(TOKEN_TYPE_LBRACE);
}

void parser_rbrace() {
  parser_match(TOKEN_TYPE_RBRACE);
}

ASTNode parser_while_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_WHILE);

  parser_lparen();
  nodes.push_back(parser_binexpr(0));
  parser_rparen();
  
  nodes.push_back(parser_block());

  return parser_make_node(AST_NODE_TYPE_WHILE, nodes);
}

ASTNode parser_for_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_FOR);

  parser_lparen();
  if (parser_token().type == TOKEN_TYPE_VAR) {
    nodes.push_back(parser_var_declaration());
  } else {
    nodes.push_back(parser_binexpr(0));
  }
  parser_semi();
  nodes.push_back(parser_binexpr(0));
  parser_semi();
  nodes.push_back(parser_binexpr(0));
  parser_rparen();
  
  nodes.push_back(parser_block());
  
  return parser_make_node(AST_NODE_TYPE_FOR, nodes);
}

std::vector<ASTNode> parser_import_statement() {
  // TODO: lexer has no functionality for this
  //parser_match(TOKEN_TYPE_IMPORT);

  //parser_match_str();
  //TokenValue value = parser_token().value;
  //parser_next_token();
  //
  //parser_semi();

  //return ast_build(tokens_strlit[index]);
  
  return {};
}

ASTNode parser_if_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_IF);

  parser_lparen();
  nodes.push_back(parser_binexpr(0));
  parser_rparen();
  
  nodes.push_back(parser_block());

  if (parser_token().type == TOKEN_TYPE_ELSE) {	
    parser_next_token();	

    ASTNode value;
    if (parser_token().type == TOKEN_TYPE_IF) {
      value = parser_if_statement();	    
    } else {
      value = parser_block();
    }
  
    nodes.push_back(parser_make_node(AST_NODE_TYPE_ELSE, { value }));	
  }

  return parser_make_node(AST_NODE_TYPE_IF, nodes);
}

ASTNode parser_return_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_RETURN);
  if (parser_token().type != TOKEN_TYPE_SEMI) {       
    nodes.push_back(parser_binexpr(0));
  }
  
  parser_semi();
  
  return parser_make_node(AST_NODE_TYPE_RETURN, nodes);
}

ASTNode parser_continue_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_CONTINUE);

  if (parser_token().type == TOKEN_TYPE_IDENT) {
    nodes.push_back(parser_make_node(AST_NODE_TYPE_IDENT, {}, parser_token().value));
    parser_next_token();
  }

  parser_semi();
  
  return parser_make_node(AST_NODE_TYPE_CONTINUE, nodes);
}

ASTNode parser_break_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_BREAK);

  if (parser_token().type == TOKEN_TYPE_IDENT) {
    nodes.push_back(parser_make_node(AST_NODE_TYPE_IDENT, {}, parser_token().value));
    parser_next_token();
  }

  parser_semi();
  
  return parser_make_node(AST_NODE_TYPE_BREAK, nodes);
}

ASTNode parser_print_statement() {
  std::vector<ASTNode> nodes;
  parser_match(TOKEN_TYPE_PRINT);

  parser_lparen();

  nodes.push_back(parser_binexpr(0));
  
  while(1) {
    if (parser_token().type == TOKEN_TYPE_COMMA) {
      parser_next_token();
      nodes.push_back(parser_binexpr(0));
    } else {
      break;
    }
  }

  parser_rparen();
  
  return parser_make_node(AST_NODE_TYPE_PRINT, nodes);
}

ASTNode parser_fun_declaration() {
  parser_match(TOKEN_TYPE_FUN);

  std::vector<ASTNode> nodes;
  
  nodes.push_back(parser_ident());

  parser_lparen();

  while(1) {
    if (parser_token().type == TOKEN_TYPE_IDENT) {
      nodes.push_back(parser_fun_param_declaration());
    } else if (parser_token().type == TOKEN_TYPE_COMMA) {
      parser_next_token();
      nodes.push_back(parser_fun_param_declaration());	    
    } else {
      break;
    }
  }

  parser_rparen();

  if (parser_token().type == TOKEN_TYPE_ARROW) {
    parser_next_token();
    nodes.push_back(parser_type());
  } else {
    nodes.push_back(parser_make_node(AST_NODE_TYPE_VOID));
  }
  
  nodes.push_back(parser_block());
 
  return parser_make_node(AST_NODE_TYPE_FUN, nodes);
}

ASTNode parser_var_declaration(bool need_type) {
  parser_match(TOKEN_TYPE_VAR);

  std::vector<ASTNode> nodes;
  
  nodes.push_back(parser_ident_declaration(need_type));
  while(1) {
    if (parser_token().type == TOKEN_TYPE_COMMA) {
      parser_next_token();
      nodes.push_back(parser_ident_declaration(need_type));   
    } else {
      break;
    }
  }
  
  return parser_make_node(AST_NODE_TYPE_VAR, nodes);
}

ASTNode parser_struct_member_declaration() {
  std::vector<ASTNode> nodes;

  nodes.push_back(parser_ident_declaration());
  
  while(1) {
    if (parser_token().type == TOKEN_TYPE_COMMA) {
        parser_next_token();
        nodes.push_back(parser_ident_declaration());
    } else if (parser_token().type == TOKEN_TYPE_SEMI) {
        parser_next_token();
        break;
    } else {
        nodes.push_back(parser_ident_declaration());
    }
  }

  return parser_make_node(AST_NODE_TYPE_STRUCT_MEMBER, nodes);
}

ASTNode parser_fun_param_declaration() {
  TokenValue value = parser_token().value;
  parser_match(TOKEN_TYPE_IDENT);
  bool arr = false;

  std::vector<ASTNode> nodes;
  std::vector<ASTNode> arr_nodes;

  if (parser_token().type == TOKEN_TYPE_LBRACK) {
    parser_next_token();
    if (parser_token().type != TOKEN_TYPE_RBRACK) {
      arr_nodes.push_back(parser_binexpr(0));
    }
    parser_rbrack();
    arr = true;
  }
  
  if (parser_colonb()) {
    parser_next_token();
    parser_match_type();
    nodes.push_back(parser_type());
  }
  
  ASTNode ident = parser_make_node(AST_NODE_TYPE_IDENT, nodes, value);

  if (arr) {
    arr_nodes.push_back(ident);

    ident = parser_make_node(AST_NODE_TYPE_ARRAY, arr_nodes);	
  }

  return ident;
}

ASTNode parser_struct_declaration() {
  parser_match(TOKEN_TYPE_STRUCT);

  std::vector<ASTNode> nodes;
  
  nodes.push_back(parser_ident());

  parser_lbrace();

  std::vector<ASTNode> statements = parser_struct_statements();

  parser_rbrace();
  
  nodes.push_back(parser_make_node(AST_NODE_TYPE_BLOCK, statements));

  return parser_make_node(AST_NODE_TYPE_STRUCT, nodes);
}

ASTNode parser_ident_declaration(bool need_type) {
  std::vector<ASTNode> ident_nodes;
  std::vector<ASTNode> arr_nodes;
  
  TokenValue ident_value = parser_token().value;
  parser_match(TOKEN_TYPE_IDENT);
  bool arr = false;
  bool have_type = !need_type;

  if (parser_token().type == TOKEN_TYPE_LBRACK) {
    parser_next_token();
    arr_nodes.push_back(parser_binexpr(0));
    parser_rbrack();
    arr = true;
  }
  
  if (parser_colonb()) {
    parser_next_token();
    parser_match_type();
    ident_nodes.push_back(parser_type());
    have_type = true;
  }
  
  ASTNode ident = parser_make_node(AST_NODE_TYPE_IDENT, ident_nodes, ident_value);
  
  if (arr) {
    arr_nodes.push_back(ident);

    bool init = false;
    if (parser_token().type == TOKEN_TYPE_LBRACE) {
      arr_nodes.push_back(parser_structlit());
      init = true;
    }

    ident = parser_make_node(AST_NODE_TYPE_ARRAY, arr_nodes);
    if (init) {
      return ident;
    }
  }
  
  if (parser_token().type == TOKEN_TYPE_ASSIGN) {
    std::vector<ASTNode> nodes;
    nodes.push_back(ident);

    parser_next_token();
    nodes.push_back(parser_binexpr(0));

    ident = parser_make_node(AST_NODE_TYPE_ASSIGN, nodes);
    have_type = have_type || true;
  }

  if (!have_type) {
    V_FATAL("no type is provided for ident declaration");
  }

  return ident;
}

ASTNode parser_structlit() {
  parser_lbrace();

  std::vector<ASTNode> nodes;
  
  while(1) {
    if (parser_token().type == TOKEN_TYPE_COMMA) {
      parser_next_token();	    
    } else if (parser_token().type == TOKEN_TYPE_RBRACE) {
      break;
    } else {
      nodes.push_back(parser_binexpr(0));
    }	
  }

  parser_rbrace();

  return parser_make_node(AST_NODE_TYPE_STRUCTLIT, nodes);
}

ASTNode parser_block() {
  parser_lbrace();

  std::vector<ASTNode> nodes;

  bool run = true;
  while(run) {
    ASTNode node;	
    switch(parser_token().type) {
    case TOKEN_TYPE_VAR:
      node = parser_var_declaration();
      parser_semi();
      break;
    case TOKEN_TYPE_IF:
      node = parser_if_statement();
      break;
    case TOKEN_TYPE_FOR:
      node = parser_for_statement();
      break;	
    case TOKEN_TYPE_WHILE:
      node = parser_while_statement();
      break;
    case TOKEN_TYPE_FUN:
      node = parser_fun_declaration();
      break;
    case TOKEN_TYPE_RETURN:
      node = parser_return_statement();
      break;
    case TOKEN_TYPE_CONTINUE:
      node = parser_continue_statement();
      break;
    case TOKEN_TYPE_BREAK:
      node = parser_break_statement();
      break;
    case TOKEN_TYPE_PRINT:
      node = parser_print_statement();
      parser_semi();
      break;
    case TOKEN_TYPE_STRUCT:
      node = parser_struct_declaration();
      break;
    case TOKEN_TYPE_RBRACE:
      parser_next_token();
      run = false;
      continue;
    default:
      node = parser_binexpr(0);
      parser_semi();
    }
      
    nodes.push_back(node);
  }

  return parser_make_node(AST_NODE_TYPE_BLOCK, nodes);
}

ASTNode parser_type() {
  ASTNode node;
  switch(parser_token().type) {
  case TOKEN_TYPE_INT:
    node = parser_make_node(AST_NODE_TYPE_INT);
    break;
  case TOKEN_TYPE_CHAR:
    node = parser_make_node(AST_NODE_TYPE_CHAR);
    break;
  case TOKEN_TYPE_FLOAT:
    node = parser_make_node(AST_NODE_TYPE_FLOAT);
    break;
  case TOKEN_TYPE_VOID:
    node = parser_make_node(AST_NODE_TYPE_VOID);
    break;
  case TOKEN_TYPE_STRING:
    node = parser_make_node(AST_NODE_TYPE_STRING);
    break;
  case TOKEN_TYPE_IDENT:
    node = parser_make_node(AST_NODE_TYPE_IDENT, {}, parser_token().value);
    break;
  default: V_FATAL("token type doesn't match \"type\" type");
  }
  parser_next_token();
  if (parser_token().type == TOKEN_TYPE_LBRACK) {
    std::vector<ASTNode> nodes;
    nodes.push_back(node);

    parser_next_token();
    nodes.push_back(parser_binexpr(0));
    parser_rbrack();

    parser_next_token();

    return parser_make_node(AST_NODE_TYPE_ARRAY, nodes);
  }

  return node;
}
