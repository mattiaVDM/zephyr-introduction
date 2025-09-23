# Minimum CMake version
cmake_minimum_required(VERSION 3.22.0)

# Locate the Zephyr RTOS source
find_package(Zephyr REQUIRED HINTS ${ZEPHYR_BASE})

# Name the project
project(blink)

# Add the print_lib subdirectory
add_subdirectory(lib/print_lib)

# Locate the source code for the application
target_sources(app PRIVATE src/main.c)

# Link the print_lib library to the app target
target_link_libraries(app PRIVATE print_lib)