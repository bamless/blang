# -----------------------------------------------------------------------------
# Sources
# -----------------------------------------------------------------------------

set(JSTAR_SOURCES
    ${PROJECT_SOURCE_DIR}/include/jstar/jstar.h
    ${PROJECT_SOURCE_DIR}/include/jstar/buffer.h
    ${PROJECT_SOURCE_DIR}/include/jstar/conf.h
    ${PROJECT_SOURCE_DIR}/include/jstar/parse/ast.h
    ${PROJECT_SOURCE_DIR}/include/jstar/parse/lex.h
    ${PROJECT_SOURCE_DIR}/include/jstar/parse/parser.h
    ${PROJECT_SOURCE_DIR}/include/jstar/parse/vector.h

    parse/ast.c
    parse/lex.c
    parse/parser.c
    parse/vector.c

    std/core.c
    std/core.h
    std/modules.h
    std/modules.c

    buffer.c
    code.c
    code.h
    compiler.c
    compiler.h
    const.h
    disassemble.c
    disassemble.h
    dynload.h
    gc.c
    gc.h
    hashtable.c
    hashtable.h
    import.c
    import.h
    jstar.c
    object.c
    object.h
    opcode.h
    opcode.c
    serialize.c
    serialize.h
    util.h
    value.c
    value.h
    vm.c
    vm.h
)

# J* standard library files
set(JSTAR_STDLIB 
    std/core.jsc
)

# Add optional module source files
if(JSTAR_SYS)
    list(APPEND JSTAR_SOURCES std/sys.h std/sys.c)
    list(APPEND JSTAR_STDLIB  std/sys.jsc)
endif()
if(JSTAR_IO)
    list(APPEND JSTAR_SOURCES std/io.h std/io.c)
    list(APPEND JSTAR_STDLIB  std/io.jsc)
endif()
if(JSTAR_MATH)
    list(APPEND JSTAR_SOURCES std/math.h std/math.c)
    list(APPEND JSTAR_STDLIB  std/math.jsc)
endif()
if(JSTAR_DEBUG)
    list(APPEND JSTAR_SOURCES std/debug.h std/debug.c)
    list(APPEND JSTAR_STDLIB  std/debug.jsc)
endif()
if(JSTAR_RE)
    list(APPEND JSTAR_SOURCES std/re.h std/re.c)
    list(APPEND JSTAR_STDLIB  std/re.jsc)
endif()

# Generate J* sandard library source headers
set(JSTAR_STDLIB_HEADERS)
foreach(jsr ${JSTAR_STDLIB})
    list(APPEND JSTAR_STDLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/${jsr}.inc)
    add_custom_command(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${jsr}.inc
        COMMAND python ${PROJECT_SOURCE_DIR}/scripts/bin2incl.py ${CMAKE_CURRENT_SOURCE_DIR}/${jsr} ${CMAKE_CURRENT_SOURCE_DIR}/${jsr}.inc
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${jsr}
    )
endforeach()

# -----------------------------------------------------------------------------
# Prepare extra libraries
# -----------------------------------------------------------------------------

# set extra libraries that we need to link
set(EXTRA_LIBS)
if(UNIX)
    set(EXTRA_LIBS dl m)
endif()

if(JSTAR_COMPUTED_GOTOS)
    if(${CMAKE_C_COMPILER_ID} STREQUAL "GNU")
        # disable crossjumping optimization on vm.c for an extra ~15% dispatch performance
        set_property(SOURCE vm.c PROPERTY COMPILE_FLAGS -fno-crossjumping)
    endif()
endif()

# -----------------------------------------------------------------------------
# Library targets
# -----------------------------------------------------------------------------

# static library
add_library(jstar_static STATIC ${JSTAR_SOURCES} ${JSTAR_STDLIB_HEADERS} ${JSTAR_STDLIB})
target_compile_definitions(jstar_static PUBLIC JSTAR_STATIC)
target_link_libraries(jstar_static PUBLIC ${EXTRA_LIBS})
target_include_directories(jstar_static 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include/jstar
        ${PROJECT_SOURCE_DIR}/profile
        ${PROJECT_BINARY_DIR}
)
set_target_properties(jstar_static PROPERTIES 
    OUTPUT_NAME "jstars"
    VERSION     ${JSTAR_VERSION}
)

#shared library
add_library(jstar SHARED ${JSTAR_SOURCES} ${JSTAR_STDLIB_HEADERS} ${JSTAR_STDLIB})
target_link_libraries(jstar PUBLIC ${EXTRA_LIBS})
target_include_directories(jstar 
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${PROJECT_SOURCE_DIR}/include/jstar
        ${PROJECT_SOURCE_DIR}/profile
        ${PROJECT_BINARY_DIR}
)
set_target_properties(jstar PROPERTIES
    PDB_NAME "jstar.dll"
    VERSION ${JSTAR_VERSION}
    SOVERSION ${JSTAR_VERSION_MAJOR}
    C_VISIBILITY_PRESET hidden
)

if(JSTAR_INSTRUMENT)
    target_link_libraries(jstar PRIVATE profile)
endif()

# Enable link-time optimization if supported
if(LTO)
    set_target_properties(jstar PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    set_target_properties(jstar_static PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# -----------------------------------------------------------------------------
# Installation
# -----------------------------------------------------------------------------

# Install target
if(JSTAR_INSTALL)
    # Install J* library
    install(TARGETS jstar jstar_static
        EXPORT  jstar-export
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )

    # Install header files
    install(DIRECTORY
        ${PROJECT_SOURCE_DIR}/include/ 
        DESTINATION include
        PATTERN "*.h.in" EXCLUDE
    )
endif()
