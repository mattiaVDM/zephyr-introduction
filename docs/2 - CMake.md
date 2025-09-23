# CMake

It is a metatool that helps with creating build files, it generates Makefiles to build applications. Zephyr extends CMake, adding some Zephyr specific functions.

https://docs.zephyrproject.org/latest/build/zephyr_cmake_package.html

## Basic template

First thing to do is to set the version of CMake that we are going to use:
`cmake_minimum_required(VERSION 3.20.0)`

This will give a brief explanation of the project:

```CMake
project(
    hello_world
    VERSION 1.0
    DESCRIPTION "The classic"
    LANGUAGES C
)
```

Then a static library is created:

```CMake
add_library(my_lib
    STATIC
    src/my_lib.c
)

```

`STATIC`:  this means that the library is statically linked, when working with zephyr this will almost alwyas be the case.

 Set the include directories for the library. PUBLIC adds the directory to the search path for any targets that link to this library:

```CMake
target_include_directories(
    my_lib
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

```

Create an executable target with the same name as the project name:

```CMake
add_executable(
    ${PROJECT_NAME}
    src/main.c
)

```

Link the library to the executable. PRIVATE means that the library is not exposed to targets that depend on this target:

```CMake
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    my_lib
)
```

## Usage

In the application folder, create a build directory:

`mkdir build`

Open the build directory and run CMake, giving the context of the parent folder:

```bash
cd build
cmake ..
```

This will generate the Makefile, then we can run make to build the executable:

`make`

This will build the executable of our application.

### Using cmake directly

```cmake -S . -B build```

`-S` specifies the source folder.
`-B` specifies and creates the build folder if does not exits.

Then we can build using make like:
`cmake --build build`

### Using a different build system

```cmake -S . -B build -G "Ninja"```

### Zephyr and CMake

Zephyr extend CMake to add functionalities that help building Zephyr projects.

#### Example blink CMakeList.txt

```CMake
# Minimum CMake version
cmake_minimum_required(VERSION 3.22.0)

# Locate the Zephyr RTOS source
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Name the project
project(blink)

# Locate the source code for the application
target_sources(app PRIVATE src/main.c)
```

When Zephyr package is add, it automatically calls
add_executable(app ...), so the app target name is already created.

To build this project, we previously used west, that automatically calls CMake to generate the executable.
We can also build the project without using west:

```bash
cmake -G "Ninja" -DBOARD=esp32s_devkitc/esp32s/procpu -DDTC_OVERLAY_FILE=boards/esp32s3_devkit.overlay -B build -S .
```
