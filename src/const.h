#ifndef CONST_H
#define CONST_H

#include <stdint.h>

// -----------------------------------------------------------------------------
// RUNTIME CONSTANTS
// -----------------------------------------------------------------------------

#define RECURSION_LIMIT 5000                           // Max recursion depth
#define FRAME_SZ        100                            // Default starting frame size
#define STACK_SZ        (FRAME_SZ) * (MAX_LOCALS + 1)  // Deafult starting stack size
#define INIT_GC         (1024 * 1024 * 20)             // 10MiB - First GC collection point
#define HEAP_GROW_RATE  2                              // The heap growing rate
#define HANDLER_MAX     10                             // Max number of try-excepts for a frame

// -----------------------------------------------------------------------------
// COMPILER CONSTANTS
// -----------------------------------------------------------------------------

#define MAX_TRY_DEPTH HANDLER_MAX  // Max depth of nested trys
#define MAX_LOCALS    UINT8_MAX    // At most 255 local vars per frame

// -----------------------------------------------------------------------------
// STRING CONSTANTS
// -----------------------------------------------------------------------------

#define CTOR_STR    "new"
#define THIS_STR    "this"
#define ARGV_STR    "argv"
#define ANON_PREFIX "anon:"

#define JSR_EXT ".jsr"
#define JSC_EXT ".jsc"

#define NEXT_METH "__next__"
#define ITER_METH "__iter__"

#define EXC_ERR   "_err"
#define EXC_CAUSE "_cause"
#define EXC_TRACE "_stacktrace"

#define PACKAGE_FILE "__package__"

#if defined(JSTAR_WINDOWS)
    #define DL_PREFIX ""
    #define DL_SUFFIX ".dll"
#elif defined(JSTAR_MACOS) || defined(JSTAR_IOS)
    #define DL_PREFIX ""
    #define DL_SUFFIX ".dylib"
#elif defined(JSTAR_POSIX)
    #define DL_PREFIX "lib"
    #define DL_SUFFIX ".so"
#else
    #define DL_PREFIX ""
    #define DL_SUFFIX ""
#endif

#endif
