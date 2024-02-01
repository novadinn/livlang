#pragma once

#include "ast_node.h"
#include "environment.h"

EvalValue eval(ASTNode *node, Environment *env);