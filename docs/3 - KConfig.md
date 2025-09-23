# KConfig

KConfig is a configuration language used to enable, disable and customize various pieces of software.

<https://docs.zephyrproject.org/latest/build/kconfig/menuconfig.html>

Configurations can be edited using menuconfig:

```bash
west build -p always -b <board> -t menuconfig
```

Configuration are generated inside `/build/zephyr/.config`.

**Note:** every time that we run the build command, the .config file is overridden, so the configurations are lost. To keep the configurations on each run, they must be added to the `prj.conf` file.

To know what to add to the prj.conf file, we can use diff command to show changes between .config and .config.old:

`diff build/zephyr/.config.old build/zephyr/.config`

Then we can past the ouput of the diff inside the `prj.conf`, removing unnecessary lines and statements.

We can use a different .conf file by passing the symbol `DCONF_FILE`:

`west build -p always -b <board> -- -DCONF_FILE=custom.conf`

There may be configurations that are board-specific, so it is possible to create board specific configurations inside `boards` folder, giving for example the name of the board as filename, for example:

`esp32s3_devkitc.conf`

A good approach is also to put common configuration in `prj.conf`, and then use the `-DEXTRA_CONF_FILE` symbol for board specific configuration:

`west build -p always -b <board> -- -DEXTRA_CONF_FILE=board.conf`

## Creating Custom Kconfig Symbols in a Zephyr Module

<https://docs.zephyrproject.org/latest/build/kconfig/index.html>

So far, we’ve only toggled existing Kconfig symbols. Zephyr also lets you define your own symbols for custom modules. Let’s create a simple module that prints “Hello!” when enabled.

### Module Structure

In your workspace, create an out-of-tree Zephyr module with the following structure:

```txt
workspace/modules/say_hello/
├─ zephyr/
│  └─ module.yaml
├─ say_hello.h
├─ say_hello.c
├─ Kconfig
└─ CMakeLists.txt
```

### Create a new option in menuconfig

```txt
config SAY_HELLO
    bool "Basic print test to console"
    default n   # Set the library to be disabled by default
    depends on PRINTK   # Make it dependent on PRINTK
    help
        Adds say_hello() function to print a basic message to the console.
```

This defines a new Kconfig symbol SAY_HELLO which the user can enable or disable from menuconfig.

CMakeLists.txt (in modules/say_hello):

```cmake
# Check if SAY_HELLO is set in Kconfig
if(CONFIG_SAY_HELLO)

    # Add your source file directory
    zephyr_include_directories(.)

    # Add the source file you want to compile
    zephyr_library_sources(say_hello.c)

endif()
```

This CMake logic ensures that say_hello.c is only compiled and linked if CONFIG_SAY_HELLO is set.

We also need a module.yaml file so Zephyr knows this directory is a module:

```yaml
name: say_hello
build:
  cmake: .
  kconfig: Kconfig

```

### Integrating the Module into the Application

Back in your main application, update CMakeLists.txt to tell Zephyr about the extra module:

```cmake
cmake_minimum_required(VERSION 3.20.0)

set(ZEPHYR_EXTRA_MODULES "${CMAKE_SOURCE_DIR}/../../modules/say_hello")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(random)

target_sources(app PRIVATE src/main.c)
```

## Useful KConfig keyboard commands

`spacebar`: enable/disable a menuconfig item.
`/`: jump to symbol, it uses a regex to look for configurations matching the regex. E.g ^random -> will look for configurations starting with "random"
`a`: show hidden menu options.
