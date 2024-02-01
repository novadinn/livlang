#include "environment.h"

#include "vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void environmentCreate(Environment *parent, Environment *out_env) {
  out_env->variables = vectorCreate(EvalVariable);
  out_env->parent = parent;
}

void environmentDestroy(Environment *env) {
  vectorDestroy(env->variables);
  env->variables = 0;
  env->parent = 0;
}

b8 environmentSearch(Environment *env, const char *name, EvalValue *out_value) {
  for (u32 i = 0; i < vectorLength(env->variables); ++i) {
    if (!strcmp(env->variables[i].identifier, name)) {
      *out_value = env->variables[i].value;

      return true;
    }
  }

  if (!env->parent) {
    EvalValue v;
    v.type = EVAL_VALUE_TYPE_UNKNOWN;
    *out_value = v;

    return false;
  }

  return environmentSearch(env->parent, name, out_value);
}

b8 environmentEmplace(Environment *env, char *name, EvalValue value) {
  for (u32 i = 0; i < vectorLength(env->variables); ++i) {
    if (!strcmp(env->variables[i].identifier, name)) {
      return false;
    }
  }

  EvalVariable var;
  var.identifier = name;
  var.value = value;
  vectorPush(env->variables, var);

  return true;
}

b8 environmentSet(Environment *env, const char *name, EvalValue value) {
  b8 contains = false;
  EvalVariable *var = 0;
  for (u32 i = 0; i < vectorLength(env->variables); ++i) {
    if (!strcmp(env->variables[i].identifier, name)) {
      contains = true;
      var = &env->variables[i];
    }
  }

  if (!contains) {
    if (env->parent) {
      return environmentSet(env->parent, name, value);
    }

    return false;
  }

  var->value = value;

  return true;
}