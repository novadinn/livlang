#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ast_node.h"
#include "enviroinment.h"

VladValue eval(ASTNode *node, Enviroinment *env);

#endif // EVALUATOR_H
