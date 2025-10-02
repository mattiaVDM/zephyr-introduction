# Look for the next CMakeLists.txt file in the "drivers" subdirectory
add_subdirectory(drivers)

# Add the "drivers" directory to the compiler include search path for all modules
zephyr_include_directories(drivers)
