#include "ast_node.h"

#include "logger.h"
#include "vector.h"

void ASTNodeDestroy(ASTNode *node) {
  if (node->children) {
    for (u32 i = 0; i < vectorLength(node->children); ++i) {
      ASTNodeDestroy(&node->children[i]);
    }

    vectorDestroy(node->children);
  }
}

void ASTPrint(ASTNode *root) {
  ASTNodePrint(root);
  if (root->children) {
    for (u32 i = 0; i < vectorLength(root->children); ++i) {
      ASTPrint(&root->children[i]);
    }
  }
}

void ASTNodePrint(ASTNode *node) {
  const char *types[AST_NODE_TYPE_MAX + 1] = {
      "MULT",   "DIV",     "PLUS",      "MINUS",  "GT",        "LT",
      "GE",     "LE",      "EQ",        "NE",     "AND",       "OR",
      "ASSIGN", "POSTINC", "POSTDEC",   "IDENT",  "INTLIT",    "FLOATLIT",
      "STRLIT", "CHARLIT", "STRUCTLIT", "ARRAY",  "FUNC_CALL", "ARR_ACCESS",
      "NOT",    "VAR",     "IF",        "ELSE",   "WHILE",     "FOR",
      "FUN",    "RETURN",  "CONTINUE",  "BREAK",  "PRINT",     "INT",
      "CHAR",   "FLOAT",   "VOID",      "STRING", "PROGRAMM",  "BLOCK",
  };

  switch (node->type) {
  case AST_NODE_TYPE_INTLIT: {
    DEBUG("%s %ld", types[node->type], node->value.integer);
  } break;
  case AST_NODE_TYPE_FLOATLIT: {
    DEBUG("%s %f", types[node->type], node->value.floating);
  } break;
  case AST_NODE_TYPE_CHARLIT: {
    DEBUG("%s %c", types[node->type], node->value.character);
  } break;
  case AST_NODE_TYPE_STRLIT: {
    DEBUG("%s %s", types[node->type], node->value.string);
  } break;
  case AST_NODE_TYPE_IDENT: {
    DEBUG("%s %s", types[node->type], node->value.identifier);
  } break;
  default: {
    DEBUG("%s", types[node->type]);
  } break;
  };
}