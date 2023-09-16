#ifndef ENVIROINMENT_H
#define ENVIROINMENT_H

#include "vlad_value.h"

#include <string>
#include <unordered_map>

typedef struct Enviroinment {
  std::unordered_map<std::string, VladValue> map;
  Enviroinment *parent;
} Enviroinment;

Enviroinment *enviroinment_create(Enviroinment *parent);
void enviroinment_destroy(Enviroinment *env);

b8 enviroinment_search(Enviroinment *env, std::string &name,
                       VladValue *out_value);
b8 enviroinment_emplace(Enviroinment *env, std::string &name, VladValue token);
b8 enviroinment_set(Enviroinment *env, std::string &name, VladValue token);

#endif // ENVIROINMENT_H