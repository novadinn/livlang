#pragma once

#include "defines.h"

typedef union InterpreterValue {
  i64 integer;
  f64 floating;
  char character;
  char *string;
  char *identifier;
} InterpreterValue;