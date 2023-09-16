#ifndef VLAD_VALUE_H
#define VLAD_VALUE_H

#include "ast_node.h"
#include "defines.h"
#include "token.h"

#include <vector>

typedef enum VladValueType {
  VLAD_VALUE_TYPE_UNKNOWN,
  VLAD_VALUE_TYPE_INT,
  VLAD_VALUE_TYPE_FLOAT,
  VLAD_VALUE_TYPE_CHAR,
  VLAD_VALUE_TYPE_STRING,
  VLAD_VALUE_TYPE_ARRAY,
  VLAD_VALUE_TYPE_IDENT,
  VLAD_VALUE_TYPE_FUN,
} VladValueType;

typedef enum VladPayloadType {
  VLAD_PAYLOAD_TYPE_NONE,
  VLAD_PAYLOAD_TYPE_RETURN,
  VLAD_PAYLOAD_TYPE_BREAK,
  VLAD_PAYLOAD_TYPE_CONTINUE,
} VladPayloadType;

struct VladValue;

typedef struct VladFunData {
  u8 return_value;
  ASTNode block;
  std::vector<VladValue> arguments;
  std::string name;
} VladFunData;

/* TODO: either separate those to arrays, or make is union (in that case
 * VladFunData should have its own identifier), but both approaches requires a
 * GC */
typedef struct VladValueData {
  i64 integer;
  f64 floating;
  u8 character;
  std::string string;
  std::string identifier;
  VladFunData function;
  std::vector<VladValue> array;
  /* TODO: instead of this flag, we can just or type with the payload type */
  u8 payload;
} VladValueData;

typedef struct VladValue {
  u8 type = VLAD_VALUE_TYPE_UNKNOWN;
  VladValueData value;
} VladValue;

#endif // VLAD_VALUE_H