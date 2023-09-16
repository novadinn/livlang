#include "enviroinment.h"

#include "logger.h"

Enviroinment *enviroinment_create(Enviroinment *parent) {
  Enviroinment *env = new Enviroinment();
  env->parent = parent;

  return env;
}

void enviroinment_destroy(Enviroinment *env) { delete env; }

b8 enviroinment_search(Enviroinment *env, std::string &name,
                       VladValue *out_value) {
  if (env->map.count(name)) {
    *out_value = env->map[name];

    return true;
  }

  if (!env->parent) {
    *out_value = VladValue{VLAD_VALUE_TYPE_UNKNOWN};
    return false;
  }

  return enviroinment_search(env->parent, name, out_value);
}

b8 enviroinment_emplace(Enviroinment *env, std::string &name, VladValue token) {
  if (env->map.count(name)) {
    return false;
  }

  env->map[name] = token;

  return true;
}

b8 enviroinment_set(Enviroinment *env, std::string &name, VladValue token) {
  if (!env->map.count(name)) {
    if (!env->parent) {
      return false;
    }

    return enviroinment_set(env->parent, name, token);
  }

  env->map[name] = token;

  return true;
}