#include "ast_node.h"

#include "logger.h"

void ast_print(ASTNode &root) {
  ast_node_print(root);
  for (int i = 0; i < root.children.size(); ++i) {
    ast_print(root.children[i]);
  }
}

void ast_node_print(ASTNode &node) {
  const char *types[AST_NODE_TYPE_MAX + 1] = {
      "MULT",   "DIV",     "PLUS",      "MINUS",  "GT",        "LT",
      "GE",     "LE",      "EQ",        "NE",     "AND",       "OR",
      "ASSIGN", "POSTINC", "POSTDEC",   "IDENT",  "INTLIT",    "FLOATLIT",
      "STRLIT", "CHARLIT", "STRUCTLIT", "ARRAY",  "FUNC_CALL", "ARR_ACCESS",
      "NOT",    "VAR",     "IF",        "ELSE",   "WHILE",     "FOR",
      "FUN",    "RETURN",  "CONTINUE",  "BREAK",  "PRINT",     "INT",
      "CHAR",   "FLOAT",   "VOID",      "STRING", "PROGRAMM",  "BLOCK",
  };

  switch (node.type) {
  case AST_NODE_TYPE_INTLIT: {
    V_TRACE("%s %ld", types[node.type], node.value.integer);
  } break;
  case AST_NODE_TYPE_FLOATLIT: {
    V_TRACE("%s %f", types[node.type], node.value.floating);
  } break;
  case AST_NODE_TYPE_CHARLIT: {
    V_TRACE("%s %c", types[node.type], node.value.character);
  } break;
  case AST_NODE_TYPE_STRLIT: {
    V_TRACE("%s %s", types[node.type], node.value.string.c_str());
  } break;
  case AST_NODE_TYPE_IDENT: {
    V_TRACE("%s %s", types[node.type], node.value.identifier.c_str());
  } break;
  default: {
    V_TRACE("%s", types[node.type]);
  } break;
  };
}