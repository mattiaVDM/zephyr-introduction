#include <zephyr/random/random.h>

#ifdef CONFIG_PRINT_LIB
#include "print_lib.h"
#endif

// Settings
static const int32_t sleep_time_ms = 1000;

int main(void)
{
    uint32_t rnd;
    double rnd_float;

    while (1) {

        rnd = sys_rand32_get();
		rnd_float = (double)rnd / (UINT32_MAX + 1.0);
		printk("Random number: %.3f\r\n", rnd_float);

#ifdef CONFIG_PRINT_LIB
        say_hello();
#endif

        // Sleep
        k_msleep(sleep_time_ms);
    }

    return 0;
}
