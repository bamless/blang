#ifndef COMPILER_H
#define COMPILER_H

#include "jsrparse/ast.h"
#include "jstar.h"
#include "object.h"

typedef struct Compiler Compiler;

ObjFunction* compile(JStarVM* vm, const char* filename, ObjModule* module, Stmt* s);
void reachCompilerRoots(JStarVM* vm, Compiler* c);

#endif
