#ifndef VM_H
#define VM_H

#include "compiler.h"
#include "const.h"
#include "hashtable.h"
#include "object.h"
#include "util.h"
#include "value.h"

#include <stdint.h>
#include <stdlib.h>

// This stores the info needed to jump
// to handler code and to restore the
// VM state when handling exceptions
typedef struct Handler {
#define HANDLER_ENSURE OP_SETUP_ENSURE
#define HANDLER_EXCEPT OP_SETUP_EXCEPT
    uint8_t type;     // The type of the handler block (HANDLER_ENSURE/HANDLER_EXCEPT)
    uint8_t *handler; // The address of the handler code
    Value *savesp;    // Stack pointer to restore state when handling exceptions
} Handler;

typedef struct Frame {
    uint8_t *ip;                   // Instruction pointer
    Value *stack;                  // Base of stack for current frame
    Value fn;                      // The function associated with the frame (native or closure)
    Handler handlers[HANDLER_MAX]; // Exception handlers
    uint8_t handlerc;              // Exception handlers count
} Frame;

// The J* VM. This struct stores all the
// state needed to execute J* code.
typedef struct JStarVM {
    // Paths searched for import
    ObjList *importpaths;

    // Built in classes
    ObjClass *clsClass;
    ObjClass *objClass;
    ObjClass *strClass;
    ObjClass *boolClass;
    ObjClass *lstClass;
    ObjClass *numClass;
    ObjClass *funClass;
    ObjClass *modClass;
    ObjClass *nullClass;
    ObjClass *stClass;
    ObjClass *tupClass;
    ObjClass *excClass;
    ObjClass *tableClass;

    // Current VM compiler
    Compiler *currCompiler;

    // Constant strings needed by compiler and runtime
    ObjString *ctor, *stacktrace, *next, *iter;

    // Names of overloadable operator's methods
    ObjString *add, *sub, *mul, *div, *mod, *get, *set;
    ObjString *radd, *rsub, *rmul, *rdiv, *rmod;
    ObjString *lt, *le, *gt, *ge, *eq, *neg;

    // Script arguments
    const char **argv;
    int argc;

    // The empty tuple (singleton)
    ObjTuple *emptyTup;

    // loaded modules
    HashTable modules;
    // current module and core module
    ObjModule *module, *core;

    // VM program stack and stack pointer
    size_t stackSz;
    Value *stack, *sp;

    // Frames stack
    int frameSz;
    Frame *frames;
    int frameCount;

    // Stack used during native function calls
    Value *apiStack;

    // Constant string pool, for interned strings
    HashTable strings;

    // Linked list of all open upvalues
    ObjUpvalue *upvalues;

    // ---- Memory management ----

    // Linked list of all allocated objects (used in
    // the sweep phase of GC to free unreached objects)
    Obj *objects;

    bool disableGC;   // Whether the garbage collector is enabled or disabled
    size_t allocated; // Bytes currently allocated
    size_t nextGC;    // Bytes to which the next GC will be triggered

    // Stack used to recursevely reach all the fields of reached objects
    Obj **reachedStack;
    size_t reachedCapacity, reachedCount;
} JStarVM;

bool runEval(JStarVM *vm, int depth);

bool getFieldFromValue(JStarVM *vm, Value val, ObjString *name);
bool setFieldOfValue(JStarVM *vm, Value val, ObjString *name, Value s);

bool callValue(JStarVM *vm, Value callee, uint8_t argc);
bool invokeFromValue(JStarVM *vm, ObjString *name, uint8_t argc);

bool unwindStack(JStarVM *vm, int depth);

static inline void push(JStarVM *vm, Value v) {
    *vm->sp++ = v;
}

static inline Value pop(JStarVM *vm) {
    return *--vm->sp;
}

static inline bool isValTrue(Value val) {
    if(IS_BOOL(val)) return AS_BOOL(val);
    return !IS_NULL(val);
}

static inline ObjClass *getClass(JStarVM *vm, Value v) {
    if(IS_NUM(v)) {
        return vm->numClass;
    } else if(IS_BOOL(v)) {
        return vm->boolClass;
    } else if(IS_OBJ(v)) {
        return AS_OBJ(v)->cls;
    } else {
        return vm->nullClass;
    }
}

static inline bool isInstance(JStarVM *vm, Value i, ObjClass *cls) {
    for(ObjClass *c = getClass(vm, i); c != NULL; c = c->superCls) {
        if(c == cls) {
            return true;
        }
    }
    return false;
}

static inline Value peek(JStarVM *vm) {
    return vm->sp[-1];
}

static inline Value peek2(JStarVM *vm) {
    return vm->sp[-2];
}

static inline Value peekn(JStarVM *vm, int n) {
    return vm->sp[-(n + 1)];
}

static inline int apiStackIndex(JStarVM *vm, int slot) {
    assert(vm->sp - slot > vm->apiStack, "API stack slot would be negative");
    assert(vm->apiStack + slot < vm->sp, "API stack overflow");
    if(slot < 0) return vm->sp + slot - vm->apiStack;
    return slot;
}

// Get the value at API stack slot "slot"
static inline Value apiStackSlot(JStarVM *vm, int slot) {
    assert(vm->sp - slot > vm->apiStack, "API stack slot would be negative");
    assert(vm->apiStack + slot < vm->sp, "API stack overflow");
    if(slot < 0) return vm->sp[slot];
    return vm->apiStack[slot];
}

#endif
