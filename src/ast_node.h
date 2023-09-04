#ifndef AST_NODE_H
#define AST_NODE_H

#include <vector>

#include "defines.h"
#include "token.h"

typedef enum ASTNodeType {
  /* * */
  AST_NODE_TYPE_MULT,
  /* / */
  AST_NODE_TYPE_DIV,
  /* + */
  AST_NODE_TYPE_PLUS,
  /* - */
  AST_NODE_TYPE_MINUS,
  /* > */
  AST_NODE_TYPE_GT,
  /* < */
  AST_NODE_TYPE_LT,
  /* >= */
  AST_NODE_TYPE_GE,
  /* <= */
  AST_NODE_TYPE_LE,
  /* == */
  AST_NODE_TYPE_EQ,
  /* != */
  AST_NODE_TYPE_NE,
  /* && */
  AST_NODE_TYPE_AND,
  /* || */
  AST_NODE_TYPE_OR,
  /* = */
  AST_NODE_TYPE_ASSIGN,
  /* x++ */
  AST_NODE_TYPE_POSTINC,
  /* x-- */
  AST_NODE_TYPE_POSTDEC,
  /* identifier */
  AST_NODE_TYPE_IDENT,
  /* int literal */
  AST_NODE_TYPE_INTLIT, 
  /* float literal */
  AST_NODE_TYPE_FLOATLIT, 
  /* string literal */
  AST_NODE_TYPE_STRLIT, 
  /* char literal */
  AST_NODE_TYPE_CHARLIT, 
  /* struct literal {} */
  AST_NODE_TYPE_STRUCTLIT,
  /* array [] */
  AST_NODE_TYPE_ARRAY,
  /* struct S {
   *  x: int; // struct member
   * };
   */
  AST_NODE_TYPE_STRUCT_MEMBER,
  /*  var s: S = S{}; // struct init
   *
   */
  AST_NODE_TYPE_STRUCT_INIT,
  /* function call func_name(args) */
  AST_NODE_TYPE_FUNC_CALL,
  /* aray access arr[0] */
  AST_NODE_TYPE_ARR_ACCESS,
  /* struct member access s.x */
  AST_NODE_TYPE_MEMBER_ACCESS,
  /* ! */
  AST_NODE_TYPE_NOT,
  /* var */
  AST_NODE_TYPE_VAR, 
  /* if */
  AST_NODE_TYPE_IF,
  /* else */
  AST_NODE_TYPE_ELSE,
  /* while */
  AST_NODE_TYPE_WHILE,
  /* for */
  AST_NODE_TYPE_FOR,  
  /* fun */
  AST_NODE_TYPE_FUN, 
  /* return */
  AST_NODE_TYPE_RETURN,
  /* continue */
  AST_NODE_TYPE_CONTINUE, 
  /* break */
  AST_NODE_TYPE_BREAK, 
  /* print */
  AST_NODE_TYPE_PRINT,
  /* struct */
  AST_NODE_TYPE_STRUCT,
  /* int */
  AST_NODE_TYPE_INT, 
  /* char */
  AST_NODE_TYPE_CHAR,
  /* float */
  AST_NODE_TYPE_FLOAT, 
  /* void */
  AST_NODE_TYPE_VOID,
  /* string */
  AST_NODE_TYPE_STRING,
  /* program enter point */
  AST_NODE_TYPE_PROGRAMM, 
  /* {
   *    var a = 10;
   * } // block of code;
   */
  AST_NODE_TYPE_BLOCK,
} ASTNodeType;

typedef struct ASTNode {
  u8 type;
  std::vector<ASTNode> children;
  TokenValue value;
} ASTNode;

#endif // AST_NODE_H
