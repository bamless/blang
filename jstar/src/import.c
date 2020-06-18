#include "import.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "builtin/modules.h"
#include "common.h"
#include "compiler.h"
#include "dynload.h"
#include "hashtable.h"
#include "jsrparse/parser.h"
#include "jstar.h"
#include "value.h"
#include "vm.h"

ObjFunction* compileWithModule(JStarVM* vm, const char* filename, ObjString* name, Stmt* program) {
    ObjModule* module = getModule(vm, name);

    if(module == NULL) {
        push(vm, OBJ_VAL(name));
        module = newModule(vm, name);
        pop(vm);

        if(vm->core != NULL) {
            hashTableImportNames(&module->globals, &vm->core->globals);
        }

        setModule(vm, name, module);
    }

    if(program != NULL) {
        ObjFunction* fn = compile(vm, filename, module, program);
        return fn;
    }

    return NULL;
}

void setModule(JStarVM* vm, ObjString* name, ObjModule* module) {
    push(vm, OBJ_VAL(module));
    push(vm, OBJ_VAL(name));
    hashTablePut(&module->globals, copyString(vm, "__name__", 8, true), OBJ_VAL(name));
    pop(vm);
    pop(vm);
    hashTablePut(&vm->modules, name, OBJ_VAL(module));
}

ObjModule* getModule(JStarVM* vm, ObjString* name) {
    Value module;
    if(!hashTableGet(&vm->modules, name, &module)) {
        return NULL;
    }
    return AS_MODULE(module);
}

static void tryNativeLib(JStarVM* vm, JStarBuffer* modulePath, ObjString* moduleName) {
    const char* rootPath = strrchr(modulePath->data, '/');
    const char* simpleName = strrchr(moduleName->data, '.');

    if(simpleName == NULL) {
        simpleName = moduleName->data;
    } else {
        simpleName++;
    }

    jsrBufferTrunc(modulePath, (int)(rootPath - modulePath->data));
    jsrBufferAppendstr(modulePath, "/");
    jsrBufferAppendstr(modulePath, DL_PREFIX);
    jsrBufferAppendstr(modulePath, simpleName);
    jsrBufferAppendstr(modulePath, DL_SUFFIX);

    void* dynlib = dynload(modulePath->data);
    if(dynlib != NULL) {
        jsrBufferClear(modulePath);
        jsrBufferAppendstr(modulePath, "jsr_open_");
        jsrBufferAppendstr(modulePath, simpleName);

        typedef JStarNativeReg* (*RegFunc)();
        RegFunc open_lib = (RegFunc)dynsim(dynlib, modulePath->data);

        if(open_lib == NULL) {
            dynfree(dynlib);
            return;
        }

        ObjModule* m = getModule(vm, moduleName);
        m->natives.dynlib = dynlib;
        m->natives.registry = (*open_lib)();
    }
}

static bool importWithSource(JStarVM* vm, const char* path, ObjString* name, const char* source) {
    Stmt* program = parse(path, source, vm->errorFun);

    if(program == NULL) {
        return false;
    }

    ObjFunction* moduleFun = compileWithModule(vm, path, name, program);
    freeStmt(program);

    if(moduleFun == NULL) {
        return false;
    }

    push(vm, OBJ_VAL(moduleFun));
    return true;
}

typedef enum ImportResult {
    IMPORT_OK,
    IMPORT_ERR,
    IMPORT_NOT_FOUND,
} ImportResult;

static ImportResult importFromPath(JStarVM* vm, JStarBuffer* path, ObjString* name) {
    char* source = jsrReadFile(path->data);
    if(source == NULL) {
        return IMPORT_NOT_FOUND;
    }

    bool imported = importWithSource(vm, path->data, name, source);
    free(source);

    if(imported) {
        tryNativeLib(vm, path, name);
        return IMPORT_OK;
    } else {
        return IMPORT_ERR;
    }
}

static bool importModuleOrPackage(JStarVM* vm, ObjString* name) {
    ObjList* paths = vm->importpaths;

    JStarBuffer fullPath;
    jsrBufferInit(vm, &fullPath);

    for(size_t i = 0; i < paths->count + 1; i++) {
        if(i < paths->count) {
            if(!IS_STRING(paths->arr[i])) continue;
            jsrBufferAppendstr(&fullPath, AS_STRING(paths->arr[i])->data);
            if(fullPath.len > 0 && fullPath.data[fullPath.len - 1] != '/') {
                jsrBufferAppendChar(&fullPath, '/');
            }
        }

        size_t moduleStart = fullPath.len;
        size_t moduleEnd = moduleStart + name->length;
        jsrBufferAppendstr(&fullPath, name->data);
        jsrBufferReplaceChar(&fullPath, moduleStart, '.', '/');

        ImportResult res;

        // try to load a package (__package__.jsr file in a directory)
        jsrBufferAppendstr(&fullPath, PACKAGE_FILE);
        res = importFromPath(vm, &fullPath, name);

        if(res != IMPORT_NOT_FOUND) {
            jsrBufferFree(&fullPath);
            return res == IMPORT_OK;
        }

        // if there is no package try to load module (i.e. normal .jsr file)
        jsrBufferTrunc(&fullPath, moduleEnd);
        jsrBufferAppendstr(&fullPath, ".jsr");
        res = importFromPath(vm, &fullPath, name);

        if(res != IMPORT_NOT_FOUND) {
            jsrBufferFree(&fullPath);
            return res == IMPORT_OK;
        }

        jsrBufferClear(&fullPath);
    }

    jsrBufferFree(&fullPath);
    return false;
}

bool importModule(JStarVM* vm, ObjString* name) {
    if(hashTableContainsKey(&vm->modules, name)) {
        push(vm, NULL_VAL);
        return true;
    }

    const char* builtinSrc = readBuiltInModule(name->data);
    if(builtinSrc != NULL) {
        return importWithSource(vm, name->data, name, builtinSrc);
    }

    if(!importModuleOrPackage(vm, name)) {
        return false;
    }

    const char* nameStart = strrchr(name->data, '.');
    if(nameStart == NULL) {
        // not a nested module, nothing to do
        return true;
    }

    nameStart++;

    // set module as a global in its parent
    ObjModule* module = getModule(vm, name);
    ObjString* parentName = copyString(vm, name->data, nameStart - 1 - name->data, true);
    ObjModule* parent = getModule(vm, parentName);
    ObjString* simpleName = copyString(vm, nameStart, strlen(nameStart), true);
    hashTablePut(&parent->globals, simpleName, OBJ_VAL(module));

    return true;
}
