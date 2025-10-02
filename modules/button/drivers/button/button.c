// This macro ties the "compatible = "custom_button";" in the device tree to this driver
#define DT_DRV_COMPAT custom_button

#include <errno.h>
#include <zephyr/logging/log.h>

#include <button.h>

// Enable logging for this module at CONFIG_LOG_DEFAULT_LEVEL
// default value can be set in prj.conf
LOG_MODULE_REGISTER(button);

// Forward declarations

// This is reserved for driver initialization function, and let the Zephyr kernel
// initialize the driver during boot up
static int button_init(const struct device *dev);
static int button_state_get(const struct device *dev, uint8_t *state);

// Private functions

// Initialize the button
static int button_init(const struct device *dev)
{
    int ret;

    // Cast the config to button_config
    const struct button_config *cfg = (const struct button_config *)dev->config;

    // Get the button gpio_dt_spec
    const struct gpio_dt_spec *btn = &cfg->btn;

    // Print to console
    LOG_DBG("Initializing button (id: %d) on %s pin %d", cfg->id, btn->port->name, btn->pin);

    if (!gpio_is_ready_dt(btn))
    {
        LOG_ERR("GPIO device %s is not ready", btn->port->name);
        return -ENODEV;
    }

    // Set the button pin as input
    ret = gpio_pin_configure_dt(btn, GPIO_INPUT);
    if (ret < 0)
    {
        LOG_ERR("Failed to configure button");
        return -ENODEV;
    }

    return 0;
}

// Public functions

// Get the button state
static int button_state_get(const struct device *dev, uint8_t *state)
{
    int ret;

    const struct button_config *cfg = (const struct button_config *)dev->config;
    // Get the button struct from the config
    const struct gpio_dt_spec *btn = &cfg->btn;

    // Poll button state
    ret = gpio_pin_get_dt(btn);
    if (ret < 0)
    {
        LOG_ERR("Failed to read button state");
        return ret;
    }
    *state = (uint8_t)ret;
    return 0;
}

// Devicetree handling
// Macrobatics to instantiate the driver for each node
// There are a lot of macros that allows to get informations from
// devicetree. We will generate code based on the devicetree nodes.

// Define the public API functions for the driver

static const struct button_api button_api_funcs = {
    .get = button_state_get,
};

// Expansion macro to define driver instances
#define BUTTON_DEFINE(inst)                                               \
                                                                          \
    /*Create an instance of the config struct, populate with DT values */ \
    static const struct button_config button_cfg_##inst = {               \
        .btn = GPIO_DT_SPEC_GET(                                          \
            DT_PHANDLE(DT_INST(inst, custom_button), pin), gpios),        \
        .id = inst};                                                      \
                                                                          \
    /* Create a device instance from Devicetree node identifier and */    \
    /* registers the init function to run during boot*/                   \
    DEVICE_DT_INST_DEFINE(inst,                                           \
                          button_init,                                    \
                          NULL,                                           \
                          NULL,                                           \
                          &button_cfg_##inst,                             \
                          POST_KERNEL,                                    \
                          CONFIG_GPIO_INIT_PRIORITY,                      \
                          &button_api_funcs);

// For each instance in the devicetree that matches the compatible string
// invoke the BUTTON_DEFINE macro to instantiate the driver as long as the
// status is "okay" in the devicetree
DT_INST_FOREACH_STATUS_OKAY(BUTTON_DEFINE)
