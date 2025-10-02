# Custom Zephyr function that imports the button subdirectory
# if the Kconfig option is enabled
add_subdirectory_ifdef(CONFIG_CUSTOM_BUTTON button)
