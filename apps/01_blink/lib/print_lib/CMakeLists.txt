# Specify a minimum CMake version
cmake_minimum_required(VERSION 3.20.0)

# Create a static library target named "print_lib"
add_library(print_lib
    STATIC
    src/print_lib.c
)

# Set the include directories for the library. PUBLIC adds the directory
# to the search path for any targets that link to this library.
target_include_directories(
    print_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

