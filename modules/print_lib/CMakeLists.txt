if(CONFIG_PRINT_LIB)

    # Tell zephyr where to find the header files
    zephyr_include_directories(.)
    # Add the source files to the build
    zephyr_library_sources(print_lib.c)

endif()
