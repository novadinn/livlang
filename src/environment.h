#pragma once

#include "defines.h"
#include "eval_value.h"

typedef struct Environment {
  EvalVariable *variables;
  struct Environment *parent;
} Environment;

void environmentCreate(Environment *parent, Environment *out_env);
void environmentDestroy(Environment *env);

b8 environmentSearch(Environment *env, const char *name, EvalValue *out_value);
b8 environmentEmplace(Environment *env, char *name, EvalValue value);
b8 environmentSet(Environment *env, const char *name, EvalValue value);