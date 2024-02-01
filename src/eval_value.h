#pragma once

#include "ast_node.h"
#include "defines.h"

typedef enum EvalValueType {
  EVAL_VALUE_TYPE_UNKNOWN,
  EVAL_VALUE_TYPE_INT,
  EVAL_VALUE_TYPE_FLOAT,
  EVAL_VALUE_TYPE_CHAR,
  EVAL_VALUE_TYPE_STRING,
  EVAL_VALUE_TYPE_ARRAY,
  EVAL_VALUE_TYPE_IDENT,
  EVAL_VALUE_TYPE_FUN,
} EvalValueType;

typedef enum EvalPayloadType {
  EVAL_PAYLOAD_TYPE_NONE,
  EVAL_PAYLOAD_TYPE_RETURN,
  EVAL_PAYLOAD_TYPE_BREAK,
  EVAL_PAYLOAD_TYPE_CONTINUE,
} EvalPayloadType;

struct EvalValue;
struct EvalVariable;

typedef struct EvalFunData {
  u8 return_value;
  ASTNode block;
  struct EvalVariable *arguments;
} EvalFunData;

typedef union EvalValueData {
  i64 integer;
  f64 floating;
  char character;
  char *string;
  EvalFunData function;
  struct EvalValue *array;
} EvalValueData;

typedef struct EvalValue {
  u8 type;
  EvalValueData value;
  u8 payload;
} EvalValue;

typedef struct EvalVariable {
  struct EvalValue value;
  char *identifier;
} EvalVariable;
