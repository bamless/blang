if(APPLE)
    set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
else()
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

# Cli app
add_executable(jstarc jstarc.c)
target_link_libraries(jstarc PRIVATE jstar argparse)

# Enable link-time optimization if supported
if(LTO)
    set_target_properties(cli PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Install target
if(JSTAR_INSTALL)
    install(TARGETS jstarc
        EXPORT jstar-export
        RUNTIME DESTINATION bin
    )
endif()