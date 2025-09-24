#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/adc.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/sys/printk.h>

#define MY_ADC_CH DT_ALIAS(my_adc_channel)
// Settings
static const int32_t sleep_time_ms = 100;

// Get Devicetree configurations
static const struct device *adc = DEVICE_DT_GET(DT_ALIAS(my_adc));
static const struct adc_channel_cfg adc_ch = ADC_CHANNEL_CFG_DT(MY_ADC_CH);
static const struct pwm_dt_spec pwm_led0 = PWM_DT_SPEC_GET(DT_ALIAS(pwm_led0));

int main(void)
{
    int ret;
    // ADC
    uint16_t buf;
    int32_t vref_mv;

    // Get Vref (mV) from Devicetree property
    // zephyr_vref_mv is a symbol that the preprocessor handles when expanding the macro
    vref_mv = DT_PROP(MY_ADC_CH, zephyr_vref_mv);

    // Buffer and options for ADC (defined in adc.h)
    struct adc_sequence seq = {
        .channels = BIT(adc_ch.channel_id),
        .buffer = &buf,
        .buffer_size = sizeof(buf),
        .resolution = DT_PROP(MY_ADC_CH, zephyr_resolution)
    };

    // Make sure that the ADC was initialized
    if (!device_is_ready(adc)) {
        printk("ADC peripheral is not ready\r\n");
        return 0;
    }

    // Configure ADC channel
    ret = adc_channel_setup(adc, &adc_ch);
    if (ret < 0) {
        printk("Could not set up ADC\r\n");
        return 0;
    }

    // Make sure that the PWM was initialized
    if (!pwm_is_ready_dt(&pwm_led0)) {
        printk("Error: PWM device %s is not ready\n",
                pwm_led0.dev->name);
        return 0;
	}

    while (1) {

        // Sample ADC
        ret = adc_read(adc, &seq);
        if (ret < 0) {
            printk("Could not read ADC: %d\r\n", ret);
            continue;
        }
       // This is mapping the ADC measurement to the total period of the LED PWM
       // (ADCread / ADCmax) * PWMperiod
       // PWMperiod (pwm_led0.period) corresponds to the max value for pulse width
       // ADCmax (1 << resolution) is the resolution of the ADC (2^resolution)
       // ADCread (buf) is the actual ADC measurement
		uint32_t pulse_ns = pwm_led0.period / ((1 << seq.resolution) / (float)buf);

		// Set LED PWM pulse width
		ret = pwm_set_dt(&pwm_led0, pwm_led0.period, pulse_ns);
		if (ret) {
			printk("Error %d: failed to set pulse width\n", ret);
			return 0;
		}
        double pulse_percentage = ((double)pulse_ns / (double)pwm_led0.period) * 100.0;
        printk("Using pulse %d, %.2f%%\n", pulse_ns, pulse_percentage);

        // Sleep
        k_msleep(sleep_time_ms);
    }

    return 0;
}
