#include "compiler.h"
#include "ast.h"
#include "vm.h"
#include "value.h"
#include "memory.h"
#include "opcode.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static ObjFunction *function(Compiler *c, Stmt *s);
static void addLocal(Compiler *c, Identifier *id, int line);

void initCompiler(Compiler *c, Compiler *prev, int depth, VM *vm) {
	c->vm = vm;
	c->func = NULL;
	c->prev = prev;
	c->depth = depth;
	c->localsCount = 0;
	c->hadError = false;

	vm->currCompiler = c;

	Identifier id = {0, ""};
	addLocal(c, &id, 0);
}

void endCompiler(Compiler *c) {
	c->vm->currCompiler = c->prev;
}

static void error(Compiler *c, int line, const char *format, ...) {
	fprintf(stderr, "[line:%d] ", line);
	va_list args;
	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stderr, "\n");
	c->hadError = true;
}

static void enterScope(Compiler *c) {
	c->depth++;
}

static void exitScope(Compiler *c) {
	c->depth--;
	while(c->localsCount > 0 &&
			c->locals[c->localsCount - 1].depth > c->depth) {
		c->localsCount--;
	}
}

static size_t emitBytecode(Compiler *c, uint8_t b, int line) {
	if(line == 0 && c->func->chunk.linesCount > 0) {
		line = c->func->chunk.lines[c->func->chunk.linesCount - 1];
	}
	return writeByte(&c->func->chunk, b, line);
}

static size_t emitShort(Compiler *c, uint16_t s, int line) {
	size_t i = writeByte(&c->func->chunk, (uint8_t) (s >> 8), line);
	writeByte(&c->func->chunk, (uint8_t) s, line);
	return i;
}

static uint8_t createConst(Compiler *c, Value constant, int line) {
	int index = addConstant(&c->func->chunk, constant);
	if(index == -1) {
		error(c, line, "too many constants "
				"in function %s", c->func->name->data);
		return 0;
	}
	return (uint8_t) index;
}

static uint8_t identifierConst(Compiler *c, Identifier *id, int line) {
	ObjString *idStr = copyString(c->vm, id->name, id->length);
	return createConst(c, OBJ_VAL(idStr), line);
}

static int resolveVariable(Compiler *c, Identifier *id, int line) {
	for(int i = c->localsCount - 1; i >= 0; i--) {
		if(identifierEquals(&c->locals[i].id, id)) {
			if (c->locals[i].depth == -1) {
				error(c, line, "Cannot read local"
						" variable in its own initializer.");
				return 0;
			}
			return i;
		}
	}
	return -1;
}

static void addLocal(Compiler *c, Identifier *id, int line) {
	if(c->localsCount == MAX_LOCALS) {
		error(c, line, "Too many local variables"
				" in function %s.", c->func->name->data);
		return;
	}

	Local *local = &c->locals[c->localsCount];
	local->id = *id;
	local->depth = -1;
	c->localsCount++;
}

static void declareVar(Compiler *c, Identifier *id, int line) {
	if(c->depth == 0) return;

	for(int i = c->localsCount - 1; i >= 0; i--) {
		if(c->locals[i].depth != -1 && c->locals[i].depth < c->depth) break;
		if(identifierEquals(&c->locals[i].id, id)) {
			error(c, line, "Variable %.*s already"
					" declared.", id->length, id->name);
		}
	}

	addLocal(c, id, line);
}

static void defineVar(Compiler *c, Identifier *id, int line) {
	if(c->depth == 0) {
		uint8_t idConst = identifierConst(c, id, line);
		emitBytecode(c, OP_DEFINE_GLOBAL, line);
		emitBytecode(c, idConst, line);
	} else {
		c->locals[c->localsCount - 1].depth = c->depth;
	}
}

static size_t emitJumpTo(Compiler *c, int jmpOpcode, size_t target, int line) {
	int32_t offset = target - (c->func->chunk.count + 3);
	if(offset > INT16_MAX || offset < INT16_MIN) {
		error(c, line, "Too much code to jump over.");
	}

	emitBytecode(c, jmpOpcode, 0);
	emitShort(c, (uint16_t) offset, 0);
	return c->func->chunk.count - 2;
}

static void setJumpTo(Compiler *c, size_t jumpAddr, size_t target, int line) {
	int32_t offset = target - (jumpAddr + 3);
	if(offset > INT16_MAX || offset < INT16_MIN) {
		error(c, line, "Too much code to jump over.");
	}

	Chunk *chunk = &c->func->chunk;
	chunk->code[jumpAddr + 1] = (uint8_t) (uint16_t) offset >> 8;
	chunk->code[jumpAddr + 2] = (uint8_t) (uint16_t) offset;
}

static void compileFunction(Compiler *c, Stmt *s) {
	Compiler funComp;
	initCompiler(&funComp, c, c->depth + 1, c->vm);
	ObjFunction *func = function(&funComp, s);
	endCompiler(&funComp);

	uint8_t fnConst = createConst(c, OBJ_VAL(func), s->line);
	uint8_t idConst = identifierConst(c, &s->funcDecl.id, s->line);

	emitBytecode(c, OP_GET_CONST, s->line);
	emitBytecode(c, fnConst, s->line);
	emitBytecode(c, OP_DEFINE_GLOBAL, s->line);
	emitBytecode(c, idConst, s->line);

	c->hadError |= funComp.hadError;
}

static void compileExpr(Compiler *c, Expr *e);

static void compileBinaryExpr(Compiler *c, Expr *e) {
	compileExpr(c, e->bin.left);
	compileExpr(c, e->bin.right);
	switch(e->bin.op) {
	case PLUS:  emitBytecode(c, OP_ADD, e->line);  break;
	case MINUS: emitBytecode(c, OP_SUB, e->line);  break;
	case MULT:  emitBytecode(c, OP_MUL, e->line);  break;
	case DIV:   emitBytecode(c, OP_DIV, e->line);  break;
	case MOD:   emitBytecode(c, OP_MOD, e->line);  break;
	case EQ:    emitBytecode(c, OP_EQ, e->line);   break;
	case NEQ:   emitBytecode(c, OP_NEQ, e->line);  break;
	case GT:    emitBytecode(c, OP_GT, e->line);   break;
	case GE:    emitBytecode(c, OP_GE, e->line);   break;
	case LT:    emitBytecode(c, OP_LT, e->line);   break;
	case LE:    emitBytecode(c, OP_LE, e->line);   break;
	default:
		error(c, e->line, "Wrong operator for binary expression.");
		break;
	}
}

static void compileLogicExpr(Compiler *c, Expr *e) {
	compileExpr(c, e->bin.left);
	emitBytecode(c, OP_DUP, e->line);

	uint8_t jmp = e->bin.op == AND ? OP_JUMPF : OP_JUMPT;
	size_t scJmp = emitBytecode(c, jmp, 0);
	emitShort(c, 0, 0);

	emitBytecode(c, OP_POP, e->line);
	compileExpr(c, e->bin.right);

	setJumpTo(c, scJmp, c->func->chunk.count, e->line);
}

static void compileUnaryExpr(Compiler *c, Expr *e) {
	compileExpr(c, e->unary.operand);
	switch(e->unary.op) {
	case MINUS: emitBytecode(c, OP_NEG, e->line); break;
	case NOT:   emitBytecode(c, OP_NOT, e->line); break;
	default:
		error(c, e->line, "Wrong operator for unary expression.");
		break;
	}
}

static void compileAssignExpr(Compiler *c, Expr *e) {
	compileExpr(c, e->assign.rval);
	int i = resolveVariable(c, &e->assign.lval->var.id, e->line);
	if(i != -1) {
		emitBytecode(c, OP_SET_LOCAL, e->line);
		emitBytecode(c, (uint8_t) i, e->line);
	} else {
		emitBytecode(c, OP_SET_GLOBAL, e->line);
		uint8_t id =identifierConst(c, &e->assign.lval->var.id, e->line);
		emitBytecode(c, id, e->line);
	}
}

static void compileCallExpr(Compiler *c, Expr *e) {
	compileExpr(c, e->callExpr.callee);

	LinkedList *n;
	uint8_t argsc = 0;
	foreach(n, e->callExpr.args->exprList.lst) {
		if(argsc == UINT8_MAX) {
			error(c, e->line,
				"Too many arguments for function %s.", c->func->name->data);
			return;
		}

		argsc++;
		compileExpr(c, (Expr*) n->elem);
	}

	if(argsc <= 10) {
		emitBytecode(c, OP_CALL_0 + argsc, e->line);
	} else {
		emitBytecode(c, OP_CALL, e->line);
		emitBytecode(c, argsc, e->line);
	}
}

static void compileExpr(Compiler *c, Expr *e) {
	switch(e->type) {
	case ASSIGN:
	 	compileAssignExpr(c, e);
		break;
	case BINARY:
		if(e->bin.op == AND || e->bin.op == OR) {
			compileLogicExpr(c, e);
		} else {
			compileBinaryExpr(c, e);
		}
		break;
	case UNARY:
		compileUnaryExpr(c, e);
		break;
	case CALL_EXPR:
		compileCallExpr(c, e);
		break;
	case EXPR_LST: {
		LinkedList *n;
		foreach(n, e->exprList.lst) {
			compileExpr(c, (Expr*) n->elem);
		}
		break;
	}
	case NUM_LIT:
		emitBytecode(c, OP_GET_CONST, e->line);
		emitBytecode(c, createConst(c, NUM_VAL(e->num), e->line), e->line);
		break;
	case BOOL_LIT:
		emitBytecode(c, OP_GET_CONST, e->line);
		emitBytecode(c, createConst(c, BOOL_VAL(e->boolean), e->line), e->line);
		break;
	case STR_LIT: {
		emitBytecode(c, OP_GET_CONST, e->line);
		ObjString *str = copyString(c->vm, e->str.str + 1, e->str.length - 2);
		emitBytecode(c, createConst(c, OBJ_VAL(str), e->line), e->line);
		break;
	}
	case VAR_LIT: {
		int i = resolveVariable(c, &e->var.id, e->line);
		if(i != -1) {
			emitBytecode(c, OP_GET_LOCAL, e->line);
			emitBytecode(c, (uint8_t) i, e->line);
		} else {
			emitBytecode(c, OP_GET_GLOBAL, e->line);
			emitBytecode(c, identifierConst(c, &e->var.id, e->line), e->line);
		}
		break;
	}
	case NULL_LIT:
		emitBytecode(c, OP_NULL, e->line);
		break;
	}
}

static void compileVarDecl(Compiler *c, Stmt *s) {
	declareVar(c, &s->varDecl.id, s->line);

	if(s->varDecl.init != NULL) {
		compileExpr(c, s->varDecl.init);
	} else {
		emitBytecode(c, OP_NULL, s->line);
	}

	defineVar(c, &s->varDecl.id, s->line);
}

static void compileStatement(Compiler *c, Stmt *s);
static void compileStatements(Compiler *c, LinkedList *stmts);

static void compileReturn(Compiler *c, Stmt *s) {
	if(c->prev == NULL) {
		error(c, s->line, "Cannot use return in global scope.");
	}

	if(s->returnStmt.e != NULL) {
		compileExpr(c, s->returnStmt.e);
	} else {
		emitBytecode(c, OP_NULL, s->line);
	}

	emitBytecode(c, OP_RETURN, s->line);
}

static void compileIfStatement(Compiler *c, Stmt *s) {
	// compile the condition
	compileExpr(c, s->ifStmt.cond);

	// emit the jump istr for false condtion with dummy address
	size_t falseJmp = emitBytecode(c, OP_JUMPF, 0);
	emitShort(c, 0, 0);

	// compile 'then' branch
	compileStatement(c, s->ifStmt.thenStmt);

	// if the 'if' has an 'else' emit istruction to jump over the 'else' branch
	size_t exitJmp = 0;
	if(s->ifStmt.elseStmt != NULL) {
		exitJmp = emitBytecode(c, OP_JUMP, 0);
		emitShort(c, 0, 0);
	}

	// set the false jump to the 'else' branch (or to exit if not present)
	setJumpTo(c, falseJmp, c->func->chunk.count, s->line);

	// If present compile 'else' branch and set the exit jump to 'else' end
	if(s->ifStmt.elseStmt != NULL) {
		compileStatement(c, s->ifStmt.elseStmt);
		setJumpTo(c, exitJmp, c->func->chunk.count, s->line);
	}
}

static void compileForStatement(Compiler *c, Stmt *s) {
	// init
	if(s->forStmt.init != NULL) {
		compileExpr(c, s->forStmt.init);
		emitBytecode(c, OP_POP, 0);
	}

	// condition
	size_t forStart = c->func->chunk.count;
	size_t exitJmp = 0;
	if(s->forStmt.cond != NULL) {
		compileExpr(c, s->forStmt.cond);
		exitJmp = emitBytecode(c, OP_JUMPF, 0);
		emitShort(c, 0, 0);
	}

	// body
	compileStatement(c, s->forStmt.body);

	// act
	if(s->forStmt.act != NULL) {
		compileExpr(c, s->forStmt.act);
		emitBytecode(c, OP_POP, 0);
	}

	// jump back to for start
	emitJumpTo(c, OP_JUMP, forStart, 0);

	// set the exit jump
	if(s->forStmt.cond != NULL) {
		setJumpTo(c, exitJmp, c->func->chunk.count, s->line);
	}
}

static void compileWhileStatement(Compiler *c, Stmt *s) {
	size_t start = c->func->chunk.count;

	compileExpr(c, s->whileStmt.cond);
	size_t exitJmp = emitBytecode(c, OP_JUMPF, 0);
	emitShort(c, 0, 0);

	compileStatement(c, s->whileStmt.body);

	emitJumpTo(c, OP_JUMP, start, 0);

	setJumpTo(c, exitJmp, c->func->chunk.count, s->line);
}

static void compileStatement(Compiler *c, Stmt *s) {
	switch(s->type) {
	case IF:
		compileIfStatement(c, s);
		break;
	case FOR:
		compileForStatement(c, s);
		break;
	case WHILE:
		compileWhileStatement(c, s);
		break;
	case BLOCK:
		enterScope(c);
		compileStatements(c, s->blockStmt.stmts);
		exitScope(c);
		break;
	case RETURN_STMT:
		compileReturn(c, s);
		break;
	case EXPR:
		compileExpr(c, s->exprStmt);
		emitBytecode(c, OP_POP, 0);
		break;
	case VARDECL:
		compileVarDecl(c, s);
		break;
	case FUNCDECL:
		compileFunction(c, s);
		break;
	case PRINT:
		compileExpr(c, s->printStmt.e);
		emitBytecode(c, OP_PRINT, s->line);
		break;
	}
}

static void compileStatements(Compiler *c, LinkedList *stmts) {
	LinkedList *n;
	foreach(n, stmts) {
		compileStatement(c, (Stmt*) n->elem);
	}
}

ObjFunction *compile(VM *vm, Stmt *s) {
	Compiler c;
	initCompiler(&c, NULL, -1, vm);

	ObjFunction *func = function(&c, s);

	endCompiler(&c);

	if(c.hadError) {
		return NULL;
	} else {
		return func;
	}
}

static ObjFunction *function(Compiler *c, Stmt *s) {
	c->func = newFunction(c->vm, linkedListLength(s->funcDecl.formalArgs));
	if(s->funcDecl.id.length != 0) {
		c->func->name = copyString(c->vm, s->funcDecl.id.name,
		                                  s->funcDecl.id.length);
	}

	enterScope(c);

	LinkedList *n;
	foreach(n, s->funcDecl.formalArgs) {
		declareVar(c, (Identifier*) n->elem, s->line);
		c->locals[c->localsCount - 1].depth = c->depth;
	}

	compileStatements(c, s->funcDecl.body->blockStmt.stmts);

	exitScope(c);

	emitBytecode(c, OP_NULL, 0);
	emitBytecode(c, OP_RETURN, 0);

	return c->func;
}

void reachCompilerRoots(VM *vm, Compiler *c) {
	while(c != NULL) {
		reachObject(vm, (Obj*) c->func);
		c = c->prev;
	}
}
