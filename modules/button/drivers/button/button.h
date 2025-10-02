#ifndef  ZEPHYR_DRIVERS_BUTTON_H_
#define ZEPHYR_DRIVERS_BUTTON_H_

#include <zephyr/drivers/gpio.h>

// We are not using predefined api, so define our own
struct button_api {
    int (*get)(const struct device *dev, uint8_t *state);
};

// Configuration
struct button_config {
    struct gpio_dt_spec btn;
    uint32_t id;
};

#endif /* ZEPHYR_DRIVERS_BUTTON_H_ */
