#include "eval.h"

#include "defines.h"
#include "logger.h"
#include "vector.h"

#include <stdio.h>
#include <stdlib.h>

static EvalValue evalProgram(ASTNode *node, Environment *env);
static EvalValue evalBlock(ASTNode *node, Environment *env);

static EvalValue evalPlus(ASTNode *node, Environment *env);
static EvalValue evalMinus(ASTNode *node, Environment *env);
static EvalValue evalMult(ASTNode *node, Environment *env);
static EvalValue evalDiv(ASTNode *node, Environment *env);

static EvalValue evalGt(ASTNode *node, Environment *env);
static EvalValue evalLt(ASTNode *node, Environment *env);
static EvalValue evalGe(ASTNode *node, Environment *env);
static EvalValue evalLe(ASTNode *node, Environment *env);
static EvalValue evalEq(ASTNode *node, Environment *env);
static EvalValue evalNe(ASTNode *node, Environment *env);

static EvalValue evalAnd(ASTNode *node, Environment *env);
static EvalValue evalOr(ASTNode *node, Environment *env);
static EvalValue evalNot(ASTNode *node, Environment *env);

static EvalValue evalAssign(ASTNode *node, Environment *env);
static EvalValue evalPostinc(ASTNode *node, Environment *env);
static EvalValue evalPostdec(ASTNode *node, Environment *env);

static EvalValue evalIdent(ASTNode *node, Environment *env);

static EvalValue evalIntlit(ASTNode *node, Environment *env);
static EvalValue evalFloatlit(ASTNode *node, Environment *env);
static EvalValue evalCharlit(ASTNode *node, Environment *env);
static EvalValue evalStrlit(ASTNode *node, Environment *env);
static EvalValue evalStructlit(ASTNode *node, Environment *env);

static EvalValue evalArray(ASTNode *node, Environment *env);
static EvalValue evalArrAccess(ASTNode *node, Environment *env);

static EvalValue evalIf(ASTNode *node, Environment *env);
static EvalValue evalElse(ASTNode *node, Environment *env);

static EvalValue evalWhile(ASTNode *node, Environment *env);
static EvalValue evalFor(ASTNode *node, Environment *env);

static EvalValue evalVar(ASTNode *node, Environment *env);
static EvalValue evalFun(ASTNode *node, Environment *env);
static EvalValue evalFuncCall(ASTNode *node, Environment *env);
static EvalValue evalInt(ASTNode *node, Environment *env);
static EvalValue evalFloat(ASTNode *node, Environment *env);
static EvalValue evalChar(ASTNode *node, Environment *env);
static EvalValue evalString(ASTNode *node, Environment *env);
static EvalValue evalStruct(ASTNode *node, Environment *env);
static EvalValue evalVoid(ASTNode *node, Environment *env);

static EvalValue evalReturn(ASTNode *node, Environment *env);
static EvalValue evalContinue(ASTNode *node, Environment *env);
static EvalValue evalBreak(ASTNode *node, Environment *env);

static EvalValue evalPrint(ASTNode *node, Environment *env);

/* astnodetype to EvalValueType */
static u8 evalAnttoevt(u8 type);
static f64 evalRetrieveNumber(EvalValue *value);
static void evalSetNumberByType(EvalValue *eval_value, f64 value);
static u8 evalDominantType(u8 left, u8 right);
static b8 evalIsNumber(u8 type);

EvalValue eval(ASTNode *node, Environment *env) {
  switch (node->type) {

  case AST_NODE_TYPE_PROGRAMM: {
    return evalProgram(node, env);
  } break;
  case AST_NODE_TYPE_BLOCK: {
    return evalBlock(node, env);
  } break;

  case AST_NODE_TYPE_MULT: {
    return evalMult(node, env);
  } break;
  case AST_NODE_TYPE_DIV: {
    return evalDiv(node, env);
  } break;
  case AST_NODE_TYPE_PLUS: {
    return evalPlus(node, env);
  } break;
  case AST_NODE_TYPE_MINUS: {
    return evalMinus(node, env);
  } break;

  case AST_NODE_TYPE_GT: {
    return evalGt(node, env);
  } break;
  case AST_NODE_TYPE_LT: {
    return evalLt(node, env);
  } break;
  case AST_NODE_TYPE_GE: {
    return evalGe(node, env);
  } break;
  case AST_NODE_TYPE_LE: {
    return evalLe(node, env);
  } break;
  case AST_NODE_TYPE_EQ: {
    return evalEq(node, env);
  } break;
  case AST_NODE_TYPE_NE: {
    return evalNe(node, env);
  } break;

  case AST_NODE_TYPE_AND: {
    return evalAnd(node, env);
  } break;
  case AST_NODE_TYPE_OR: {
    return evalOr(node, env);
  } break;
  case AST_NODE_TYPE_NOT: {
    return evalNot(node, env);
  } break;

  case AST_NODE_TYPE_ASSIGN: {
    return evalAssign(node, env);
  } break;
  case AST_NODE_TYPE_POSTINC: {
    return evalPostinc(node, env);
  } break;
  case AST_NODE_TYPE_POSTDEC: {
    return evalPostdec(node, env);
  } break;

  case AST_NODE_TYPE_IDENT: {
    return evalIdent(node, env);
  } break;

  case AST_NODE_TYPE_INTLIT: {
    return evalIntlit(node, env);
  } break;
  case AST_NODE_TYPE_FLOATLIT: {
    return evalFloatlit(node, env);
  } break;
  case AST_NODE_TYPE_STRLIT: {
    return evalStrlit(node, env);
  } break;
  case AST_NODE_TYPE_CHARLIT: {
    return evalCharlit(node, env);
  } break;
  case AST_NODE_TYPE_STRUCTLIT: {
    return evalStructlit(node, env);
  } break;

  case AST_NODE_TYPE_ARRAY: {
    return evalArray(node, env);
  } break;
  case AST_NODE_TYPE_ARR_ACCESS: {
    return evalArrAccess(node, env);
  } break;

  case AST_NODE_TYPE_VAR: {
    return evalVar(node, env);
  } break;
  case AST_NODE_TYPE_FUN: {
    return evalFun(node, env);
  } break;
  case AST_NODE_TYPE_FUNC_CALL: {
    return evalFuncCall(node, env);
  } break;
  case AST_NODE_TYPE_INT: {
    return evalInt(node, env);
  } break;
  case AST_NODE_TYPE_CHAR: {
    return evalChar(node, env);
  } break;
  case AST_NODE_TYPE_FLOAT: {
    return evalFloat(node, env);
  } break;
  case AST_NODE_TYPE_VOID: {
    return evalVoid(node, env);
  } break;
  case AST_NODE_TYPE_STRING: {
    return evalString(node, env);
  } break;

  case AST_NODE_TYPE_IF: {
    return evalIf(node, env);
  } break;
  case AST_NODE_TYPE_ELSE: {
    return evalElse(node, env);
  } break;

  case AST_NODE_TYPE_WHILE: {
    return evalWhile(node, env);
  } break;
  case AST_NODE_TYPE_FOR: {
    return evalFor(node, env);
  } break;

  case AST_NODE_TYPE_RETURN: {
    return evalReturn(node, env);
  } break;
  case AST_NODE_TYPE_CONTINUE: {
    return evalContinue(node, env);
  } break;
  case AST_NODE_TYPE_BREAK: {
    return evalBreak(node, env);
  } break;

  case AST_NODE_TYPE_PRINT: {
    return evalPrint(node, env);
  } break;
  };

  FATAL("liv: unknown node type\n");
  exit(1);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;
  return result;
}

static EvalValue evalProgram(ASTNode *node, Environment *env) {
  for (u32 i = 0; i < vectorLength(node->children); ++i) {
    eval(&node->children[i], env);
  }

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;
  return result;
}

static EvalValue evalBlock(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  Environment local_env = {};
  environmentCreate(env, &local_env);

  for (int i = 0; i < vectorLength(node->children); ++i) {
    ASTNode *element = &node->children[i];

    result = eval(element, &local_env);

    if (result.payload != EVAL_PAYLOAD_TYPE_NONE) {
      environmentDestroy(&local_env);

      return result;
    }
  }

  environmentDestroy(&local_env);

  return result;
}

/* TODO: add string concatenation support */
static EvalValue evalPlus(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: + argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: + argument is not a number!");
    exit(1);
  }

  f64 left_value = evalRetrieveNumber(&left);
  f64 right_value = evalRetrieveNumber(&right);

  u8 type = evalDominantType(left.type, right.type);

  EvalValue result = {};
  result.type = type;

  evalSetNumberByType(&result, left_value + right_value);

  return result;
}

static EvalValue evalMinus(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: - argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: - argument is not a number!");
    exit(1);
  }

  f64 left_value = evalRetrieveNumber(&left);
  f64 right_value = evalRetrieveNumber(&right);

  u8 type = evalDominantType(left.type, right.type);

  EvalValue result = {};
  result.type = type;

  evalSetNumberByType(&result, left_value - right_value);

  return result;
}

static EvalValue evalMult(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: * argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: * argument is not a number!");
    exit(1);
  }

  f64 left_value = evalRetrieveNumber(&left);
  f64 right_value = evalRetrieveNumber(&right);

  u8 type = evalDominantType(left.type, right.type);

  EvalValue result = {};
  result.type = type;

  evalSetNumberByType(&result, left_value * right_value);

  return result;
}

static EvalValue evalDiv(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: / argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: / argument is not a number!");
    exit(1);
  }

  f64 left_value = evalRetrieveNumber(&left);
  f64 right_value = evalRetrieveNumber(&right);

  u8 type = evalDominantType(left.type, right.type);

  EvalValue result = {};
  result.type = type;

  evalSetNumberByType(&result, left_value / right_value);

  return result;
}

static EvalValue evalGt(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: > argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: > argument is not a number!");
    exit(1);
  }

  f64 left_val = evalRetrieveNumber(&left);
  f64 right_val = evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = left_val > right_val;

  return result;
}

static EvalValue evalLt(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: < argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: < argument is not a number!");
    exit(1);
  }

  f64 left_val = evalRetrieveNumber(&left);
  f64 right_val = evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = left_val < right_val;

  return result;
}

static EvalValue evalGe(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: >= argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: >= argument is not a number!");
    exit(1);
  }

  f64 left_val = evalRetrieveNumber(&left);
  f64 right_val = evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = left_val >= right_val;

  return result;
}

static EvalValue evalLe(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: <= argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: <= argument is not a number!");
    exit(1);
  }

  f64 left_val = evalRetrieveNumber(&left);
  f64 right_val = evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = left_val <= right_val;

  return result;
}

static EvalValue evalEq(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: == argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: == argument is not a number!");
    exit(1);
  }

  f64 left_val = evalRetrieveNumber(&left);
  f64 right_val = evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = left_val == right_val;

  return result;
}

static EvalValue evalNe(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: != argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: != argument is not a number!");
    exit(1);
  }

  f64 left_val = evalRetrieveNumber(&left);
  f64 right_val = evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = left_val != right_val;

  return result;
}

static EvalValue evalAnd(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: && argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: && argument is not a number!");
    exit(1);
  }

  i32 left_result = (i32)evalRetrieveNumber(&left);
  i32 right_result = (i32)evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  if (left_result == 0 || right_result == 0) {
    result.value.character = 0;
  } else {
    result.value.character = 1;
  }

  return result;
}

static EvalValue evalOr(ASTNode *node, Environment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  EvalValue left = eval(lhs, env);
  if (!evalIsNumber(left.type)) {
    FATAL("liv: || argument is not a number!");
    exit(1);
  }
  EvalValue right = eval(rhs, env);
  if (!evalIsNumber(right.type)) {
    FATAL("liv: || argument is not a number!");
    exit(1);
  }

  i32 left_result = (i32)evalRetrieveNumber(&left);
  i32 right_result = (i32)evalRetrieveNumber(&right);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  if (left_result == 0 && right_result == 0) {
    result.value.character = 0;
  } else {
    result.value.character = 1;
  }

  return result;
}

static EvalValue evalNot(ASTNode *node, Environment *env) {
  ASTNode *exp = &node->children[0];

  EvalValue value = eval(exp, env);
  if (!evalIsNumber(value.type)) {
    FATAL("liv: ! argument is not a number!");
    exit(1);
  }

  i32 val = (i32)evalRetrieveNumber(&value);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  if (val == 0) {
    result.value.character = 1;
  } else {
    result.value.character = 0;
  }

  return result;
}

static EvalValue evalAssign(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  ASTNode *left = &node->children[0];
  ASTNode *right = &node->children[1];
  if (left->type == AST_NODE_TYPE_IDENT) {
    char *name = left->value.identifier;

    EvalValue value = {};
    if (!environmentSearch(env, name, &value)) {
      FATAL("liv: unbound symbol %s", name);
      exit(1);
    }

    result = eval(right, env);
    environmentSet(env, name, result);
  } else if (left->type == AST_NODE_TYPE_ARR_ACCESS) {
    ASTNode *ident_node = &left->children[0];
    ASTNode *index_node = &left->children[1];

    EvalValue size_value = eval(index_node, env);
    if (!evalIsNumber(size_value.type)) {
      FATAL("liv: = argument is not a number!");
      exit(1);
    }

    char *name = ident_node->value.identifier;
    i32 index = (i32)evalRetrieveNumber(&size_value);

    EvalValue value = {};
    if (!environmentSearch(env, name, &value)) {
      FATAL("liv: unbound symbol %s", name);
      exit(1);
    }

    EvalValue *vec = value.value.array;

    result = eval(right, env);
    vec[index] = result;

    environmentSet(env, name, value);
  }

  return result;
}

static EvalValue evalPostinc(ASTNode *node, Environment *env) {
  char *name = node->value.identifier;

  EvalValue value = {};
  if (!environmentSearch(env, name, &value)) {
    FATAL("liv: unbound symbol %s", name);
    exit(1);
  }

  if (!evalIsNumber(value.type)) {
    FATAL("liv: ++ argument is not a number!");
    exit(1);
  }

  f64 val = evalRetrieveNumber(&value);

  EvalValue result = {};
  result.type = value.type;
  evalSetNumberByType(&result, val + 1);

  environmentSet(env, name, result);

  return result;
}

static EvalValue evalPostdec(ASTNode *node, Environment *env) {
  char *name = node->value.identifier;

  EvalValue value = {};
  if (!environmentSearch(env, name, &value)) {
    FATAL("liv: unbound symbol %s", name);
    exit(1);
  }

  if (!evalIsNumber(value.type)) {
    FATAL("liv: -- argument is not a number!");
    exit(1);
  }

  f64 val = evalRetrieveNumber(&value);

  EvalValue result = {};
  result.type = value.type;
  evalSetNumberByType(&result, val - 1);

  environmentSet(env, name, result);

  return result;
}

static EvalValue evalIdent(ASTNode *node, Environment *env) {
  char *ident = node->value.identifier;

  EvalValue result = {};
  if (!environmentSearch(env, ident, &result)) {
    FATAL("liv: unbound symbol %s", ident);
    exit(1);
  }

  return result;
}

static EvalValue evalIntlit(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_INT;
  result.value.integer = node->value.integer;

  return result;
}

static EvalValue evalFloatlit(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_FLOAT;
  result.value.floating = node->value.floating;

  return result;
}

static EvalValue evalCharlit(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_CHAR;
  result.value.character = node->value.character;

  return result;
}

static EvalValue evalStrlit(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_STRING;
  /* TODO: should we copy the string value to the new memory block? */
  result.value.string = node->value.string;

  return result;
}

/* TODO: add struct support */
static EvalValue evalStructlit(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalArray(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalArrAccess(ASTNode *node, Environment *env) {
  ASTNode *ident_node = &node->children[0];
  ASTNode *index_node = &node->children[1];

  char *name = ident_node->value.identifier;

  EvalValue index_value = eval(index_node, env);
  if (!evalIsNumber(index_value.type)) {
    FATAL("liv: [] argument is not a number!");
    exit(1);
  }

  int index = evalRetrieveNumber(&index_value);

  EvalValue value = {};
  if (!environmentSearch(env, name, &value)) {
    FATAL("liv: unbound symbol %s", name);
    exit(1);
  }

  return value.value.array[index];
}

static EvalValue evalIf(ASTNode *node, Environment *env) {
  EvalValue cond = eval(&node->children[0], env);
  if (!evalIsNumber(cond.type)) {
    FATAL("liv: if argument is not a number!");
    exit(1);
  }

  u8 result = cond.value.character;

  if (result) {
    ASTNode *block = &node->children[1];

    EvalValue result = eval(block, env);
    return result;
  } else if (vectorLength(node->children) == 3) { /* have else/else if clause */
    ASTNode *clause = &node->children[2];

    return evalElse(clause, env);
  }

  EvalValue result_value = {};
  result_value.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result_value;
}

static EvalValue evalElse(ASTNode *node, Environment *env) {
  ASTNode *next = &node->children[0];

  /* else if clause */
  if (next->type == AST_NODE_TYPE_IF) {
    return evalIf(next, env);
  } else if (next->type == AST_NODE_TYPE_BLOCK) { /* else clause */
    EvalValue result = eval(next, env);

    return result;
  }

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalWhile(ASTNode *node, Environment *env) {
  ASTNode *cond = &node->children[0];
  ASTNode *block = &node->children[1];

  /* TODO: create a local environment */

  for (;;) {
    EvalValue cond_value = eval(cond, env);
    if (!evalIsNumber(cond_value.type)) {
      FATAL("liv: while argument is not a number!");
      exit(1);
    }

    u8 val = cond_value.value.character;

    /* condition is not accomplished */
    if (val == 0) {
      break;
    }

    EvalValue result = eval(block, env);
    switch (result.payload) {
    case EVAL_PAYLOAD_TYPE_RETURN: {
      return result;
    } break;
    case EVAL_PAYLOAD_TYPE_BREAK: {
      EvalValue result = {};
      result.type = EVAL_VALUE_TYPE_UNKNOWN;

      return result;
    } break;
    case EVAL_PAYLOAD_TYPE_CONTINUE: {
      /* do nothing, since result's evaluation is already stopped */
    } break;
    };
  }

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalFor(ASTNode *node, Environment *env) {
  /* variable declaration */
  ASTNode *declare = &node->children[0];
  /* loop termination condition */
  ASTNode *cond = &node->children[1];
  ASTNode *post = &node->children[2];
  ASTNode *block = &node->children[3];

  Environment local_env = {};
  environmentCreate(env, &local_env);

  eval(declare, &local_env);
  for (;;) {
    EvalValue cond_value = eval(cond, &local_env);
    if (!evalIsNumber(cond_value.type)) {
      FATAL("liv: for argument is not a number!");
      exit(1);
    }

    u8 val = cond_value.value.character;

    /* condition is not accomplished */
    if (val == 0) {
      break;
    }

    /* evaluated the loop body */
    EvalValue result = eval(block, &local_env);
    switch (result.payload) {
    case EVAL_PAYLOAD_TYPE_RETURN: {
      return result;
    } break;
    case EVAL_PAYLOAD_TYPE_BREAK: {
      EvalValue result = {};
      result.type = EVAL_VALUE_TYPE_UNKNOWN;

      return result;
    } break;
    case EVAL_PAYLOAD_TYPE_CONTINUE: {
      /* do nothing, since result's evaluation is already stopped */
    } break;
    };

    eval(post, &local_env);
  }

  environmentDestroy(&local_env);

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalVar(ASTNode *node, Environment *env) {
  /* store the last evaluated value */
  EvalValue fin = {};
  /* loop over multiple definitions (var a = 0, b = 0;) */
  for (int i = 0; i < vectorLength(node->children); ++i) {
    ASTNode *child = &node->children[i];
    EvalValue result = {};

    /* variable with a value */
    if (child->type == AST_NODE_TYPE_ASSIGN) {
      ASTNode *lhs = &child->children[0];
      ASTNode *rhs = &child->children[1];

      char *var_name = lhs->value.identifier;

      result = eval(rhs, env);

      /* type is specified */
      if (vectorLength(lhs->children) == 1) {
        ASTNode *type_node = &lhs->children[0];
        if (result.type != evalAnttoevt(type_node->type)) {
          FATAL("liv: var argument does not match the specified type!");
          exit(1);
        }
      }

      if (!environmentEmplace(env, var_name, result)) {
        FATAL("liv: symbol %s already bound", var_name);
        exit(1);
      }
    } else if (child->type ==
               AST_NODE_TYPE_IDENT) { /* variable with a specified type, with
                                       no value */
      ASTNode *type = &child->children[0];

      char *var_name = child->value.identifier;
      result.type = evalAnttoevt(type->type);

      if (!environmentEmplace(env, var_name, result)) {
        FATAL("liv: symbol %s already bound", var_name);
        exit(1);
      }
    } else if (child->type == AST_NODE_TYPE_ARRAY) {
      ASTNode *num_node = &child->children[0];
      ASTNode *ident_node = &child->children[1];
      ASTNode *type_node = &ident_node->children[0];
      char *var_name = ident_node->value.identifier;

      EvalValue len_value = eval(num_node, env);
      if (!evalIsNumber(len_value.type)) {
        FATAL("liv: var argument is not a number!");
        exit(1);
      }

      i32 num_elements = (i32)evalRetrieveNumber(&len_value);

      /* reserve elements */
      EvalValue *vec = vectorReserve(EvalValue, num_elements);

      for (u32 i = 0; i < num_elements; ++i) {
        EvalValue element;
        element.type = evalAnttoevt(type_node->type);
        vectorPush(vec, element);
      }

      /* array with initialization */
      if (vectorLength(child->children) == 3) {
        ASTNode *init = &child->children[2];
        if (vectorLength(init->children) != num_elements) {
          FATAL("liv: specified array size does not match to number of "
                "elements!");
          exit(1);
        }

        /* TODO: check if sizes are correct */
        for (u32 i = 0; i < vectorLength(init->children); ++i) {
          ASTNode *lit = &init->children[i];
          /* TODO: check that type is match (or can be converted) */
          EvalValue val = eval(lit, env);

          vec[i] = val;
        }
      }

      result.type = EVAL_VALUE_TYPE_ARRAY;
      result.value.array = vec;

      if (!environmentEmplace(env, var_name, result)) {
        FATAL("liv: symbol %s already bound", var_name);
        exit(1);
      }
    }

    fin = result;
  }

  return fin;
}

static EvalValue evalFun(ASTNode *node, Environment *env) {
  EvalFunData data = {};
  data.arguments = vectorCreate(EvalVariable);

  ASTNode *name_node = &node->children[0];

  char *fn_name = name_node->value.identifier;
  /* foreach function argument */
  for (u32 i = 1; i < vectorLength(node->children) - 2; ++i) {
    ASTNode *arg_node = &node->children[i];
    ASTNode *type_node = &arg_node->children[0];

    char *arg_name = arg_node->value.identifier;
    u8 type = evalAnttoevt(type_node->type);

    EvalValue argument_value = {};
    argument_value.type = type;

    EvalVariable argument = {};
    argument.identifier = arg_name;
    argument.value = argument_value;

    vectorPush(data.arguments, argument);
  }

  ASTNode *block = &node->children[vectorLength(node->children) - 1];
  ASTNode *return_value = &node->children[vectorLength(node->children) - 2];

  data.return_value = evalAnttoevt(return_value->type);
  data.block = *block;

  EvalValue fun = {};
  fun.type = EVAL_VALUE_TYPE_FUN;
  fun.value.function = data;

  environmentEmplace(env, fn_name, fun);

  return fun;
}

static EvalValue evalFuncCall(ASTNode *node, Environment *env) {
  Environment function_env = {};
  environmentCreate(env, &function_env);

  ASTNode *name_node = &node->children[0];
  char *fn_name = name_node->value.identifier;

  EvalValue function = {};
  if (!environmentSearch(env, fn_name, &function)) {
    FATAL("liv: unbound symbol %s", fn_name);
    exit(1);
  }

  if (function.type != EVAL_VALUE_TYPE_FUN) {
    FATAL("liv: %s is not callable!", fn_name);
    exit(1);
  }

  EvalFunData *data = &function.value.function;
  u8 return_value_type = data->return_value;
  ASTNode *block = &data->block;
  EvalVariable *arguments = data->arguments;

  int argc = vectorLength(node->children) - 1;
  if (argc != vectorLength(arguments)) {
    FATAL("liv: number of provided argument to function %s does not match the "
          "required number of arguments!",
          fn_name);
    exit(1);
  }

  for (int i = 1; i < vectorLength(node->children); ++i) {
    ASTNode *arg_node = &node->children[i];

    EvalValue eval_arg = eval(arg_node, env);

    EvalVariable *argument = &data->arguments[i - 1];

    u8 got = eval_arg.type;
    u8 expected = argument->value.type;

    if (got != expected) {
      /* TODO: check if got type can be converted to the expected type */
    }

    if (!environmentEmplace(&function_env, argument->identifier, eval_arg)) {
      FATAL("liv: symbol %s already bound", arg_node->value.identifier);
      exit(1);
    }
  }

  EvalValue eval_result = eval(block, &function_env);
  if (eval_result.type != return_value_type) {
    /* TODO: check if got type can be converted to the expected type */
  }

  environmentDestroy(&function_env);

  return eval_result;
}

/* TODO: add conversion support */
static EvalValue evalInt(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalFloat(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalChar(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalString(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalStruct(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalVoid(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

static EvalValue evalReturn(ASTNode *node, Environment *env) {
  EvalValue result = {};
  /* has return value */
  if (vectorLength(node->children) > 0) {
    ASTNode *return_value = &node->children[0];

    result = eval(return_value, env);
  }

  result.payload = EVAL_PAYLOAD_TYPE_RETURN;
  return result;
}

static EvalValue evalContinue(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.payload = EVAL_PAYLOAD_TYPE_CONTINUE;

  return result;
}

static EvalValue evalBreak(ASTNode *node, Environment *env) {
  EvalValue result = {};
  result.payload = EVAL_PAYLOAD_TYPE_BREAK;

  return result;
}

static EvalValue evalPrint(ASTNode *node, Environment *env) {
  ASTNode *child = &node->children[0];

  EvalValue val = eval(child, env);

  switch (val.type) {
  case EVAL_VALUE_TYPE_INT: {
    printf("%ld\n", val.value.integer);
  } break;
  case EVAL_VALUE_TYPE_FLOAT: {
    printf("%f\n", val.value.floating);
  } break;
  case EVAL_VALUE_TYPE_CHAR: {
    printf("%d\n", val.value.character);
  } break;
  case EVAL_VALUE_TYPE_STRING: {
    printf("%s\n", val.value.string);
  } break;
  default: {
    ERROR("liv: failed to print type!");
  } break;
  };

  EvalValue result = {};
  result.type = EVAL_VALUE_TYPE_UNKNOWN;

  return result;
}

/* astnodetype to EvalValueType */
static u8 evalAnttoevt(u8 type) {
  switch (type) {
  case AST_NODE_TYPE_INT: {
    return EVAL_VALUE_TYPE_INT;
  } break;
  case AST_NODE_TYPE_FLOAT: {
    return EVAL_VALUE_TYPE_FLOAT;
  } break;
  case AST_NODE_TYPE_CHAR: {
    return EVAL_VALUE_TYPE_CHAR;
  } break;
  case AST_NODE_TYPE_STRING: {
    return EVAL_VALUE_TYPE_STRING;
  } break;
  case AST_NODE_TYPE_ARRAY: {
    return EVAL_VALUE_TYPE_ARRAY;
  } break;
  case AST_NODE_TYPE_FUN: {
    return EVAL_VALUE_TYPE_FUN;
  } break;
  };

  return EVAL_VALUE_TYPE_UNKNOWN;
}

static f64 evalRetrieveNumber(EvalValue *value) {
  f64 val = 0;
  switch (value->type) {
  case EVAL_VALUE_TYPE_INT: {
    val = value->value.integer;
  } break;
  case EVAL_VALUE_TYPE_FLOAT: {
    val = value->value.floating;
  } break;
  case EVAL_VALUE_TYPE_CHAR: {
    val = value->value.character;
  } break;
  };

  return val;
}

static void evalSetNumberByType(EvalValue *eval_value, f64 value) {
  switch (eval_value->type) {
  case EVAL_VALUE_TYPE_INT: {
    eval_value->value.integer = (i64)value;
  } break;
  case EVAL_VALUE_TYPE_FLOAT: {
    eval_value->value.floating = value;
  } break;
  case EVAL_VALUE_TYPE_CHAR: {
    eval_value->value.character = (char)value;
  } break;
  };
}

static u8 evalDominantType(u8 left, u8 right) {
  u8 result = 0;

  if (left == EVAL_VALUE_TYPE_INT) {
    result = right == EVAL_VALUE_TYPE_FLOAT ? right : left;
  } else if (left == EVAL_VALUE_TYPE_FLOAT) {
    result = left;
  } else if (left == EVAL_VALUE_TYPE_CHAR) {
    result = right;
  }

  return result;
}

static b8 evalIsNumber(u8 type) {
  return type == EVAL_VALUE_TYPE_CHAR || EVAL_VALUE_TYPE_FLOAT ||
         EVAL_VALUE_TYPE_INT;
}