#include "eval.h"

#include "logger.h"

#define V_ASSERT_IS_NUMBER(type, sym)                                          \
  if (type == VLAD_VALUE_TYPE_INT || type == VLAD_VALUE_TYPE_FLOAT ||          \
      type == VLAD_VALUE_TYPE_CHAR) {                                          \
  } else {                                                                     \
    V_FATAL("vlad: %s is not a number", sym);                                  \
  }

#define V_ASSERT_TYPE(got, expected, name, expected_type)                      \
  if (got == expected) {                                                       \
  } else {                                                                     \
    V_FATAL("vlad: %s is not a", got, expected_type);                          \
  }

#define V_ASSERT_FUN_ARGC(got, expected, name)                                 \
  if (got == expected) {                                                       \
  } else {                                                                     \
    V_FATAL(                                                                   \
        "vlad: the number of arguments supplied to the function %s does not "  \
        "match the number specified",                                          \
        name);                                                                 \
  }
#define V_ASSERT_ARRAY_SIZE(got, expected, name)                               \
  if (got <= expected) {                                                       \
  } else {                                                                     \
    V_FATAL("vlad: array %s too low for specified initialization", name);      \
  }

static VladValue eval_programm(ASTNode *node, Enviroinment *env);
static VladValue eval_block(ASTNode *node, Enviroinment *env);

static VladValue eval_plus(ASTNode *node, Enviroinment *env);
static VladValue eval_minus(ASTNode *node, Enviroinment *env);
static VladValue eval_mult(ASTNode *node, Enviroinment *env);
static VladValue eval_div(ASTNode *node, Enviroinment *env);

static VladValue eval_gt(ASTNode *node, Enviroinment *env);
static VladValue eval_lt(ASTNode *node, Enviroinment *env);
static VladValue eval_ge(ASTNode *node, Enviroinment *env);
static VladValue eval_le(ASTNode *node, Enviroinment *env);
static VladValue eval_eq(ASTNode *node, Enviroinment *env);
static VladValue eval_ne(ASTNode *node, Enviroinment *env);

static VladValue eval_and(ASTNode *node, Enviroinment *env);
static VladValue eval_or(ASTNode *node, Enviroinment *env);
static VladValue eval_not(ASTNode *node, Enviroinment *env);

static VladValue eval_assign(ASTNode *node, Enviroinment *env);
static VladValue eval_postinc(ASTNode *node, Enviroinment *env);
static VladValue eval_postdec(ASTNode *node, Enviroinment *env);

static VladValue eval_ident(ASTNode *node, Enviroinment *env);

static VladValue eval_intlit(ASTNode *node, Enviroinment *env);
static VladValue eval_floatlit(ASTNode *node, Enviroinment *env);
static VladValue eval_charlit(ASTNode *node, Enviroinment *env);
static VladValue eval_strlit(ASTNode *node, Enviroinment *env);
static VladValue eval_structlit(ASTNode *node, Enviroinment *env);

static VladValue eval_array(ASTNode *node, Enviroinment *env);
static VladValue eval_arr_access(ASTNode *node, Enviroinment *env);

static VladValue eval_struct(ASTNode *node, Enviroinment *env);

static VladValue eval_if(ASTNode *node, Enviroinment *env);
static VladValue eval_else(ASTNode *node, Enviroinment *env);

static VladValue eval_while(ASTNode *node, Enviroinment *env);
static VladValue eval_for(ASTNode *node, Enviroinment *env);

static VladValue eval_var(ASTNode *node, Enviroinment *env);
static VladValue eval_fun(ASTNode *node, Enviroinment *env);
static VladValue eval_func_call(ASTNode *node, Enviroinment *env);
static VladValue eval_int(ASTNode *node, Enviroinment *env);
static VladValue eval_float(ASTNode *node, Enviroinment *env);
static VladValue eval_char(ASTNode *node, Enviroinment *env);
static VladValue eval_string(ASTNode *node, Enviroinment *env);
static VladValue eval_void(ASTNode *node, Enviroinment *env);

static VladValue eval_return(ASTNode *node, Enviroinment *env);
static VladValue eval_continue(ASTNode *node, Enviroinment *env);
static VladValue eval_break(ASTNode *node, Enviroinment *env);

static VladValue eval_print(ASTNode *node, Enviroinment *env);

/* astnodetype to vladvaluetype */
static u8 eval_anttovvt(u8 type);
static f64 eval_retrieve_number(VladValue *value);
static void eval_set_number_by_type(VladValue *vlad_value, f64 value);
static u8 eval_dominant_type(u8 left, u8 right);

VladValue eval(ASTNode *node, Enviroinment *env) {
  switch (node->type) {

  case AST_NODE_TYPE_PROGRAMM: {
    return eval_programm(node, env);
  } break;
  case AST_NODE_TYPE_BLOCK: {
    return eval_block(node, env);
  } break;

  case AST_NODE_TYPE_MULT: {
    return eval_mult(node, env);
  } break;
  case AST_NODE_TYPE_DIV: {
    return eval_div(node, env);
  } break;
  case AST_NODE_TYPE_PLUS: {
    return eval_plus(node, env);
  } break;
  case AST_NODE_TYPE_MINUS: {
    return eval_minus(node, env);
  } break;

  case AST_NODE_TYPE_GT: {
    return eval_gt(node, env);
  } break;
  case AST_NODE_TYPE_LT: {
    return eval_lt(node, env);
  } break;
  case AST_NODE_TYPE_GE: {
    return eval_ge(node, env);
  } break;
  case AST_NODE_TYPE_LE: {
    return eval_le(node, env);
  } break;
  case AST_NODE_TYPE_EQ: {
    return eval_eq(node, env);
  } break;
  case AST_NODE_TYPE_NE: {
    return eval_ne(node, env);
  } break;

  case AST_NODE_TYPE_AND: {
    return eval_and(node, env);
  } break;
  case AST_NODE_TYPE_OR: {
    return eval_or(node, env);
  } break;
  case AST_NODE_TYPE_NOT: {
    return eval_not(node, env);
  } break;

  case AST_NODE_TYPE_ASSIGN: {
    return eval_assign(node, env);
  } break;
  case AST_NODE_TYPE_POSTINC: {
    return eval_postinc(node, env);
  } break;
  case AST_NODE_TYPE_POSTDEC: {
    return eval_postdec(node, env);
  } break;

  case AST_NODE_TYPE_IDENT: {
    return eval_ident(node, env);
  } break;

  case AST_NODE_TYPE_INTLIT: {
    return eval_intlit(node, env);
  } break;
  case AST_NODE_TYPE_FLOATLIT: {
    return eval_floatlit(node, env);
  } break;
  case AST_NODE_TYPE_STRLIT: {
    return eval_strlit(node, env);
  } break;
  case AST_NODE_TYPE_CHARLIT: {
    return eval_charlit(node, env);
  } break;
  case AST_NODE_TYPE_STRUCTLIT: {
    return eval_structlit(node, env);
  } break;

  case AST_NODE_TYPE_ARRAY: {
    return eval_array(node, env);
  } break;
  case AST_NODE_TYPE_ARR_ACCESS: {
    return eval_arr_access(node, env);
  } break;

  case AST_NODE_TYPE_VAR: {
    return eval_var(node, env);
  } break;
  case AST_NODE_TYPE_FUN: {
    return eval_fun(node, env);
  } break;
  case AST_NODE_TYPE_FUNC_CALL: {
    return eval_func_call(node, env);
  } break;
  case AST_NODE_TYPE_INT: {
    return eval_int(node, env);
  } break;
  case AST_NODE_TYPE_CHAR: {
    return eval_char(node, env);
  } break;
  case AST_NODE_TYPE_FLOAT: {
    return eval_float(node, env);
  } break;
  case AST_NODE_TYPE_VOID: {
    return eval_void(node, env);
  } break;
  case AST_NODE_TYPE_STRING: {
    return eval_string(node, env);
  } break;

  case AST_NODE_TYPE_IF: {
    return eval_if(node, env);
  } break;
  case AST_NODE_TYPE_ELSE: {
    return eval_else(node, env);
  } break;

  case AST_NODE_TYPE_WHILE: {
    return eval_while(node, env);
  } break;
  case AST_NODE_TYPE_FOR: {
    return eval_for(node, env);
  } break;

  case AST_NODE_TYPE_RETURN: {
    return eval_return(node, env);
  } break;
  case AST_NODE_TYPE_CONTINUE: {
    return eval_continue(node, env);
  } break;
  case AST_NODE_TYPE_BREAK: {
    return eval_break(node, env);
  } break;

  case AST_NODE_TYPE_PRINT: {
    return eval_print(node, env);
  } break;
  };

  V_FATAL("unknown vlad value\n");
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static VladValue eval_programm(ASTNode *node, Enviroinment *env) {
  for (int i = 0; i < node->children.size(); ++i) {
    eval(&node->children[i], env);
  }

  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static VladValue eval_block(ASTNode *node, Enviroinment *env) {
  VladValue result{VLAD_VALUE_TYPE_UNKNOWN};
  Enviroinment *local_env = enviroinment_create(env);

  for (int i = 0; i < node->children.size(); ++i) {
    ASTNode *element = &node->children[i];

    result = eval(element, local_env);

    if (result.value.payload != VLAD_PAYLOAD_TYPE_NONE) {
      enviroinment_destroy(local_env);
      return result;
    }
  }

  enviroinment_destroy(local_env);

  return result;
}

/* TODO: add string concatenation support */
static VladValue eval_plus(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left.type, "+ argument");
  VladValue right = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right.type, "+ argument");

  f64 left_value = eval_retrieve_number(&left);
  f64 right_value = eval_retrieve_number(&right);

  u8 type = eval_dominant_type(left.type, right.type);

  VladValue result = VladValue{type};
  eval_set_number_by_type(&result, left_value + right_value);

  return result;
}
static VladValue eval_minus(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left.type, "- argument");
  VladValue right = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right.type, "- argument");

  f64 left_value = eval_retrieve_number(&left);
  f64 right_value = eval_retrieve_number(&right);

  u8 type = eval_dominant_type(left.type, right.type);

  VladValue result = VladValue{type};
  eval_set_number_by_type(&result, left_value - right_value);

  return result;
}
static VladValue eval_mult(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left.type, "* argument");
  VladValue right = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right.type, "* argument");

  f64 left_value = eval_retrieve_number(&left);
  f64 right_value = eval_retrieve_number(&right);

  u8 type = eval_dominant_type(left.type, right.type);

  VladValue result = VladValue{type};
  eval_set_number_by_type(&result, left_value * right_value);

  return result;
}
static VladValue eval_div(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left.type, "/ argument");
  VladValue right = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right.type, "/ argument");

  f64 left_value = eval_retrieve_number(&left);
  f64 right_value = eval_retrieve_number(&right);

  u8 type = eval_dominant_type(left.type, right.type);

  VladValue result = VladValue{type};
  eval_set_number_by_type(&result, left_value / right_value);

  return result;
}

static VladValue eval_gt(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left_value = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left_value.type, "> operand");
  VladValue right_value = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right_value.type, "> operand");

  f64 left_val = eval_retrieve_number(&left_value);
  f64 right_val = eval_retrieve_number(&right_value);

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = left_val > right_val;

  return result;
}
static VladValue eval_lt(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left_value = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left_value.type, "< operand");
  VladValue right_value = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right_value.type, "< operand");

  f64 left_val = eval_retrieve_number(&left_value);
  f64 right_val = eval_retrieve_number(&right_value);

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = left_val < right_val;

  return result;
}
static VladValue eval_ge(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left_value = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left_value.type, ">= operand");
  VladValue right_value = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right_value.type, ">= operand");

  f64 left_val = eval_retrieve_number(&left_value);
  f64 right_val = eval_retrieve_number(&right_value);

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = left_val >= right_val;

  return result;
}
static VladValue eval_le(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left_value = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left_value.type, "<= operand");
  VladValue right_value = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right_value.type, "<= operand");

  f64 left_val = eval_retrieve_number(&left_value);
  f64 right_val = eval_retrieve_number(&right_value);

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = left_val <= right_val;

  return result;
}
static VladValue eval_eq(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left_value = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left_value.type, "== operand");
  VladValue right_value = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right_value.type, "== operand");

  f64 left_val = eval_retrieve_number(&left_value);
  f64 right_val = eval_retrieve_number(&right_value);

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = left_val == right_val;

  return result;
}
static VladValue eval_ne(ASTNode *node, Enviroinment *env) {
  ASTNode *lhs = &node->children[0];
  ASTNode *rhs = &node->children[1];

  VladValue left_value = eval(lhs, env);
  V_ASSERT_IS_NUMBER(left_value.type, "!= operand");
  VladValue right_value = eval(rhs, env);
  V_ASSERT_IS_NUMBER(right_value.type, "!= operand");

  f64 left_val = eval_retrieve_number(&left_value);
  f64 right_val = eval_retrieve_number(&right_value);

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = left_val != right_val;

  return result;
}

static VladValue eval_and(ASTNode *node, Enviroinment *env) {
  ASTNode *left = &node->children[0];
  ASTNode *right = &node->children[1];

  VladValue left_value = eval(left, env);
  V_ASSERT_TYPE(left_value.type, VLAD_VALUE_TYPE_CHAR, "and argument", "char");
  VladValue right_value = eval(right, env);
  V_ASSERT_TYPE(right_value.type, VLAD_VALUE_TYPE_CHAR, "and argument", "char");

  u8 left_result = left_value.value.character;
  u8 right_result = right_value.value.character;

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  if (left_result == 0 || right_result == 0) {
    result.value.character = 0;
  } else {
    result.value.character = 1;
  }

  return result;
}
static VladValue eval_or(ASTNode *node, Enviroinment *env) {
  ASTNode *left = &node->children[0];
  ASTNode *right = &node->children[1];

  VladValue left_value = eval(left, env);
  V_ASSERT_TYPE(left_value.type, VLAD_VALUE_TYPE_CHAR, "or argument", "char");
  VladValue right_value = eval(right, env);
  V_ASSERT_TYPE(right_value.type, VLAD_VALUE_TYPE_CHAR, "or argument", "char");

  u8 left_result = left_value.value.character;
  u8 right_result = right_value.value.character;

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  if (left_result == 0 && right_result == 0) {
    result.value.character = 0;
  } else {
    result.value.character = 1;
  }

  return result;
}
static VladValue eval_not(ASTNode *node, Enviroinment *env) {
  ASTNode *exp = &node->children[0];

  VladValue vlad_value = eval(exp, env);
  V_ASSERT_TYPE(vlad_value.type, VLAD_VALUE_TYPE_CHAR, "! argument", "char");

  u8 val = vlad_value.value.character;

  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  if (val == 0) {
    result.value.character = 1;
  } else {
    result.value.character = 0;
  }

  return result;
}

static VladValue eval_assign(ASTNode *node, Enviroinment *env) {
  VladValue result = VladValue{VLAD_VALUE_TYPE_UNKNOWN};

  ASTNode *left = &node->children[0];
  ASTNode *right = &node->children[1];
  if (left->type == AST_NODE_TYPE_IDENT) {
    std::string &name = left->value.identifier;

    VladValue value;
    if (!enviroinment_search(env, name, &value)) {
      V_FATAL("vlad: unbound symbol %s", name.c_str());
    }

    result = eval(right, env);

    enviroinment_set(env, name, result);
  } else if (left->type == AST_NODE_TYPE_ARR_ACCESS) {
    ASTNode *ident_node = &left->children[0];
    ASTNode *index_node = &left->children[1];

    VladValue size_value = eval(index_node, env);
    V_ASSERT_IS_NUMBER(size_value.type, "array access");

    std::string &name = ident_node->value.identifier;
    int index = (int)eval_retrieve_number(&size_value);

    VladValue value;
    if (!enviroinment_search(env, name, &value)) {
      V_FATAL("vlad: unbound symbol %s", name.c_str());
    }

    std::vector<VladValue> &vec = value.value.array;

    result = eval(right, env);
    vec[index] = result;

    enviroinment_set(env, name, value);
  }

  return result;
}
static VladValue eval_postinc(ASTNode *node, Enviroinment *env) {
  std::string &name = node->value.identifier;

  VladValue value;
  if (!enviroinment_search(env, name, &value)) {
    V_FATAL("vlad: unbound symbol %s", name.c_str());
  }

  V_ASSERT_IS_NUMBER(value.type, "++ value");
  f64 val = eval_retrieve_number(&value);

  VladValue result = VladValue{value.type};
  eval_set_number_by_type(&result, val + 1);

  enviroinment_set(env, name, result);

  return result;
}
static VladValue eval_postdec(ASTNode *node, Enviroinment *env) {
  std::string &name = node->value.identifier;

  VladValue value;
  if (!enviroinment_search(env, name, &value)) {
    V_FATAL("vlad: unbound symbol %s", name.c_str());
  }

  V_ASSERT_IS_NUMBER(value.type, "-- value");
  f64 val = eval_retrieve_number(&value);

  VladValue result = VladValue{value.type};
  eval_set_number_by_type(&result, val - 1);

  enviroinment_set(env, name, result);

  return result;
}

static VladValue eval_ident(ASTNode *node, Enviroinment *env) {
  std::string &ident = node->value.identifier;

  VladValue result;
  if (!enviroinment_search(env, ident, &result)) {
    V_FATAL("vlad: unbound symbol %s", ident.c_str());
  }

  return result;
}

static VladValue eval_intlit(ASTNode *node, Enviroinment *env) {
  VladValue result = VladValue{VLAD_VALUE_TYPE_INT};
  result.value.integer = node->value.integer;

  return result;
}
static VladValue eval_floatlit(ASTNode *node, Enviroinment *env) {
  VladValue result = VladValue{VLAD_VALUE_TYPE_FLOAT};
  result.value.floating = node->value.floating;

  return result;
}
static VladValue eval_charlit(ASTNode *node, Enviroinment *env) {
  VladValue result = VladValue{VLAD_VALUE_TYPE_CHAR};
  result.value.character = node->value.character;

  return result;
}
static VladValue eval_strlit(ASTNode *node, Enviroinment *env) {
  VladValue result = VladValue{VLAD_VALUE_TYPE_STRING};
  result.value.string = node->value.string;

  return result;
}
static VladValue eval_structlit(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static VladValue eval_array(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_arr_access(ASTNode *node, Enviroinment *env) {
  ASTNode *ident_node = &node->children[0];
  ASTNode *index_node = &node->children[1];

  std::string &name = ident_node->value.identifier;

  /* eval the index in case of an ident or an expression */
  VladValue index_value = eval(index_node, env);
  V_ASSERT_IS_NUMBER(index_value.type, "array access");
  int index = eval_retrieve_number(&index_value);

  VladValue value;
  if (!enviroinment_search(env, name, &value)) {
    V_FATAL("vlad: unbound symbol %s", name.c_str());
  }

  return value.value.array[index];
}

static VladValue eval_if(ASTNode *node, Enviroinment *env) {
  VladValue cond = eval(&node->children[0], env);
  V_ASSERT_TYPE(cond.type, VLAD_VALUE_TYPE_CHAR, "if condition", "char");
  u8 result = cond.value.character;

  if (result) {
    ASTNode *block = &node->children[1];

    VladValue result = eval(block, env);
    return result;
  } else if (node->children.size() == 3) { /* Have else/else if clause */
    ASTNode *clause = &node->children[2];

    return eval_else(clause, env);
  }

  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_else(ASTNode *node, Enviroinment *env) {
  ASTNode *next = &node->children[0];

  /* else if clause */
  if (next->type == AST_NODE_TYPE_IF) {
    return eval_if(next, env);
  } else if (next->type == AST_NODE_TYPE_BLOCK) { /* else clause */
    VladValue result = eval(next, env);

    return result;
  }

  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static VladValue eval_while(ASTNode *node, Enviroinment *env) {
  ASTNode *cond = &node->children[0];
  ASTNode *block = &node->children[1];

  for (;;) {
    VladValue cond_value = eval(cond, env);
    V_ASSERT_TYPE(cond_value.type, VLAD_VALUE_TYPE_CHAR, "for condition",
                  "char");

    u8 val = cond_value.value.character;

    /* condition is not accomplished */
    if (val == 0) {
      break;
    }

    VladValue result = eval(block, env);
    switch (result.value.payload) {
    case VLAD_PAYLOAD_TYPE_RETURN: {
      return result;
    } break;
    case VLAD_PAYLOAD_TYPE_BREAK: {
      return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
    } break;
    case VLAD_PAYLOAD_TYPE_CONTINUE: {
      /* do nothing, since result's evaluation is already stopped */
    } break;
    };
  }

  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_for(ASTNode *node, Enviroinment *env) {
  /* variable declaration */
  ASTNode *declare = &node->children[0];
  /* loop termination condition */
  ASTNode *cond = &node->children[1];
  ASTNode *post = &node->children[2];
  ASTNode *block = &node->children[3];

  Enviroinment *local_env = enviroinment_create(env);

  eval(declare, local_env);
  for (;;) {
    VladValue cond_value = eval(cond, local_env);
    V_ASSERT_TYPE(cond_value.type, VLAD_VALUE_TYPE_CHAR, "for condition",
                  "char");

    u8 val = cond_value.value.character;

    /* condition is not accomplished */
    if (val == 0) {
      break;
    }

    /* evaluated the loop body */
    VladValue result = eval(block, local_env);
    switch (result.value.payload) {
    case VLAD_PAYLOAD_TYPE_RETURN: {
      return result;
    } break;
    case VLAD_PAYLOAD_TYPE_BREAK: {
      return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
    } break;
    case VLAD_PAYLOAD_TYPE_CONTINUE: {
      /* do nothing, since result's evaluation is already stopped */
    } break;
    };

    eval(post, local_env);
  }

  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static VladValue eval_var(ASTNode *node, Enviroinment *env) {
  /* store the last evaluated value */
  VladValue fin;
  /* loop over multiple definitions (var a = 0, b = 0;) */
  for (int i = 0; i < node->children.size(); ++i) {
    ASTNode *child = &node->children[i];
    VladValue result;

    /* variable with a value */
    if (child->type == AST_NODE_TYPE_ASSIGN) {
      ASTNode *lhs = &child->children[0];
      ASTNode *rhs = &child->children[1];

      std::string &var_name = lhs->value.identifier;

      result = eval(rhs, env);

      /* type is specified */
      if (lhs->children.size() == 1) {
        ASTNode *type_node = &lhs->children[0];
        /* TODO: check if type is correct. If not, log error */
      }

      if (!enviroinment_emplace(env, var_name, result)) {
        V_FATAL("vlad: symbol %s already bound", var_name);
      }
    } else if (child->type ==
               AST_NODE_TYPE_IDENT) { /* Variable with a specified type, with
                                       no value */
      ASTNode *type = &child->children[0];

      std::string &var_name = child->value.identifier;
      result = VladValue{eval_anttovvt(type->type)};

      if (!enviroinment_emplace(env, var_name, result)) {
        V_FATAL("vlad: symbol %s already bound", var_name);
      }
    } else if (child->type == AST_NODE_TYPE_ARRAY) {
      ASTNode *num_node = &child->children[0];
      ASTNode *ident_node = &child->children[1];
      ASTNode *type_node = &ident_node->children[0];
      std::string &var_name = ident_node->value.identifier;

      /* evaluate the result in case size value is an ident or an expression
       */
      VladValue len_value = eval(num_node, env);
      V_ASSERT_IS_NUMBER(len_value.type, "array initialization");
      int num_elements = (int)eval_retrieve_number(&len_value);

      /* reserve elements */
      std::vector<VladValue> vec;
      vec.reserve(num_elements);

      for (int i = 0; i < num_elements; ++i) {
        VladValue element = VladValue{eval_anttovvt(type_node->type)};
        vec.push_back(element);
      }

      /* array with initialization */
      if (child->children.size() == 3) {
        ASTNode *init = &child->children[2];
        V_ASSERT_ARRAY_SIZE(init->children.size(), num_elements,
                            var_name.c_str());

        /* TODO: check if sizes are correct */
        for (int i = 0; i < init->children.size(); ++i) {
          ASTNode *lit = &init->children[i];
          // TODO: check that type is match (or can be converted)
          VladValue val = eval(lit, env);

          vec[i] = val;
        }
      }

      result = VladValue{VLAD_VALUE_TYPE_ARRAY};
      result.value.array = vec;

      if (!enviroinment_emplace(env, var_name, result)) {
        V_FATAL("vlad: symbol %s already bound", var_name.c_str());
      }
    }

    fin = result;
  }

  return fin;
}
static VladValue eval_fun(ASTNode *node, Enviroinment *env) {
  VladFunData data;

  ASTNode *name_node = &node->children[0];

  std::string &fn_name = name_node->value.identifier;
  /* foreach function argument */
  for (int i = 1; i < node->children.size() - 2; ++i) {
    ASTNode *arg_node = &node->children[i];
    ASTNode *type_node = &arg_node->children[0];

    std::string &arg_name = arg_node->value.identifier;
    u8 type = eval_anttovvt(type_node->type);

    VladValue argument = {type};
    argument.value.identifier = arg_name;

    data.arguments.push_back(argument);
  }

  ASTNode *block = &node->children[node->children.size() - 1];
  ASTNode *return_value = &node->children[node->children.size() - 2];

  data.return_value = eval_anttovvt(return_value->type);
  data.block = *block;

  VladValue fun = {VLAD_VALUE_TYPE_FUN};
  fun.value.function = data;
  fun.value.function.name = fn_name;
  enviroinment_emplace(env, fn_name, fun);

  return fun;
}
static VladValue eval_func_call(ASTNode *node, Enviroinment *env) {
  Enviroinment *function_env = enviroinment_create(env);

  ASTNode *name_node = &node->children[0];
  std::string &fn_name = name_node->value.identifier;

  VladValue function;
  if (!enviroinment_search(env, fn_name, &function)) {
    V_FATAL("vlad: unbound symbol %s", fn_name.c_str());
  }

  V_ASSERT_TYPE(function.type, VLAD_VALUE_TYPE_FUN, fn_name.c_str(),
                "function");

  VladFunData *data = &function.value.function;
  u8 return_value_type = data->return_value;
  ASTNode *block = &data->block;
  std::vector<VladValue> *arguments = &data->arguments;

  int argc = node->children.size() - 1;
  V_ASSERT_FUN_ARGC(argc, arguments->size(), fn_name.c_str());

  for (int i = 1; i < node->children.size(); ++i) {
    ASTNode *arg_node = &node->children[i];

    VladValue eval_arg = eval(arg_node, env);

    VladValue *argument = &data->arguments[i - 1];

    u8 got = eval_arg.type;
    u8 expected = argument->type;

    if (got != expected) {
      /* TODO: check if got type can be converted to the expected type. If
      not, log error */
    }

    if (!enviroinment_emplace(function_env, argument->value.identifier,
                              eval_arg)) {
      V_FATAL("vlad: symbol %s already bound", arg_node->value.identifier);
    }
  }

  VladValue eval_result = eval(block, function_env);
  if (eval_result.type != return_value_type) {
    /* TODO: check if got type can be converted to the expected type. If not,
    log error */
  }

  /* TODO: the problem with enviroinments is that the recursion in vlad
   * program may cause huge number of envs, which will be freed only when
   * recursion terminates. This can be fixed by storing an enviroinment inside
   * a vladfundata */
  enviroinment_destroy(function_env);

  return eval_result;
}

static VladValue eval_return(ASTNode *node, Enviroinment *env) {
  VladValue result;
  /* has return value */
  if (node->children.size() > 0) {
    ASTNode *return_value = &node->children[0];

    result = eval(return_value, env);
  }

  result.value.payload = VLAD_PAYLOAD_TYPE_RETURN;
  return result;
}
static VladValue eval_continue(ASTNode *node, Enviroinment *env) {
  VladValue result;
  result.value.payload = VLAD_PAYLOAD_TYPE_CONTINUE;

  return result;
}
static VladValue eval_break(ASTNode *node, Enviroinment *env) {
  VladValue result;
  result.value.payload = VLAD_PAYLOAD_TYPE_BREAK;

  return result;
}

/* TODO: add conversion support */
static VladValue eval_struct(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_int(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_char(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_float(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_void(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}
static VladValue eval_string(ASTNode *node, Enviroinment *env) {
  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static VladValue eval_print(ASTNode *node, Enviroinment *env) {
  ASTNode *child = &node->children[0];

  VladValue val = eval(child, env);

  switch (val.type) {
  case VLAD_VALUE_TYPE_INT: {
    printf("%ld\n", val.value.integer);
  } break;
  case VLAD_VALUE_TYPE_FLOAT: {
    printf("%f\n", val.value.floating);
  } break;
  case VLAD_VALUE_TYPE_CHAR: {
    printf("%d\n", val.value.character);
  } break;
  case VLAD_VALUE_TYPE_STRING: {
    printf("%s\n", val.value.string.c_str());
  } break;
  case VLAD_VALUE_TYPE_IDENT: {
    printf("%s\n", val.value.identifier.c_str());
  } break;
  case VLAD_VALUE_TYPE_FUN: {
    printf("function %s\n", val.value.function.name.c_str());
  };
  };

  return VladValue{VLAD_VALUE_TYPE_UNKNOWN};
}

static u8 eval_anttovvt(u8 type) {
  switch (type) {
  case AST_NODE_TYPE_INT: {
    return VLAD_VALUE_TYPE_INT;
  } break;
  case AST_NODE_TYPE_FLOAT: {
    return VLAD_VALUE_TYPE_FLOAT;
  } break;
  case AST_NODE_TYPE_CHAR: {
    return VLAD_VALUE_TYPE_CHAR;
  } break;
  case AST_NODE_TYPE_STRING: {
    return VLAD_VALUE_TYPE_STRING;
  } break;
  case AST_NODE_TYPE_ARRAY: {
    return VLAD_VALUE_TYPE_ARRAY;
  } break;
  case AST_NODE_TYPE_FUN: {
    return VLAD_VALUE_TYPE_FUN;
  } break;
  };

  return VLAD_VALUE_TYPE_UNKNOWN;
}

static f64 eval_retrieve_number(VladValue *value) {
  f64 val;
  switch (value->type) {
  case VLAD_VALUE_TYPE_INT: {
    val = value->value.integer;
  } break;
  case VLAD_VALUE_TYPE_FLOAT: {
    val = value->value.floating;
  } break;
  case VLAD_VALUE_TYPE_CHAR: {
    val = value->value.character;
  } break;
  };

  return val;
}

static void eval_set_number_by_type(VladValue *vlad_value, f64 value) {
  switch (vlad_value->type) {
  case VLAD_VALUE_TYPE_INT: {
    vlad_value->value.integer = (i64)value;
  } break;
  case VLAD_VALUE_TYPE_FLOAT: {
    vlad_value->value.floating = value;
  } break;
  case VLAD_VALUE_TYPE_CHAR: {
    vlad_value->value.character = (u8)value;
  } break;
  };
}

static u8 eval_dominant_type(u8 left, u8 right) {
  u8 result;

  if (left == VLAD_VALUE_TYPE_INT) {
    result = right == VLAD_VALUE_TYPE_FLOAT ? right : left;
  } else if (left == VLAD_VALUE_TYPE_FLOAT) {
    result = left;
  } else if (left == VLAD_VALUE_TYPE_CHAR) {
    result = right;
  }

  return result;
}