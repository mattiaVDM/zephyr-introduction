# Devicetree bindings

Bindings are the link between this abstract hardware description and the low-level driver code. They are typically defined in YAML files placed in the `zephyr/dts/bindings directory`.
Each binding file corresponds to one or more compatible strings. A compatible string might look like "espressif,esp32-adc". Inside the corresponding binding file, you’ll find documentation on what properties that node can have, their types, default values, and what’s required vs. optional.

When you run west build, Zephyr’s build system uses these binding files to:

- Validate your DTS and overlay files.
- Generate C header files that map Devicetree nodes, properties, and configurations into macros and structs that driver code and your application can reference.

As a result, changing something in your DTS configuration automatically propagates to your code through these auto-generated headers and macros. This keeps your code clean, maintainable, and easy to adapt when you switch boards or peripherals.

Note that bindings files are unique to Zephyr—Linux does not use them to map Devicetree nodes to driver code. Zephyr reads them at compile time to enforce node properties in the Devicetree (interface) and to determine which driver files to use.

## Finding the Binding Files for ADC

You’ll find the ADC binding files in zephyr/dts/bindings/adc/. There should be a generic ADC binding (such as adc-controller.yaml) and a SoC-specific binding (espressif,esp32-adc.yaml), among others.

- `adc-controller.yaml`: Defines the standard properties for an ADC controller in Zephyr. Includes properties like #io-channel-cells and standard properties for defining channels. This file creates a common interface for ADC configurations.

- `espressif,esp32-adc.yaml`: Extends the generic ADC binding with properties specific to the ESP32 series.
These YAML files use a schema that lists required and optional properties, their types, and how the driver will interpret them.

For example, espressif,esp32-adc.yaml might include:

- A **compatible**: "espressif,esp32-adc" line, ensuring that any node with this compatible string uses this binding. Note that this string is what Zephyr looks for when matching the compatible string in the Devicetree, not the filename! The filename matching is by convention but not enforced.
- Properties like unit and channel-count that are specific to the ESP32 ADC design.
- Inclusion of adc-controller.yaml to inherit common ADC properties and constraints.

## Important Properties

Some key properties defined by these bindings include:

- **reg**: Indicates the memory address or bus address of the ADC peripheral.
- **status**: Should be "okay" to enable the ADC, or "disabled" otherwise. Note that Linux has other status values, but Zephyr sticks to these two (for now).
- **zephyr,gain, zephyr,reference, zephyr,vref-mv, and zephyr,resolution**: Define how the ADC should interpret input signals and what reference voltage or resolution to use. These are per-channel properties when you create channel sub-nodes.

By reading these binding files, you learn how to properly configure each node and what the driver expects. For instance, zephyr,gain and zephyr,reference might be enums defined in the binding file, ensuring that your DTS picks from a valid set of options (like ADC_GAIN_1_4 or ADC_REF_INTERNAL).

As previously mentioned, these properties create an interface you must adhere to when defining a Devicetree node that uses this particular compatible. Zephyr’s build system will throw an error if you forget to add a required property defined in the bindings file or use the wrong type of property.

## ADC configuration for ESP32s3

```txt
/{
    aliases {
        my-adc = &adc0;
        my-adc-channel = &adc0_ch0;
    };
};

// ADC1 (adc0) channel 0 is connected to GPIO1
&adc0 {
    status = "okay";
    #address-cells = <1>;
    #size-cells = <0>;

    adc0_ch0: channel@0 {
        reg = <0>;
        zephyr,gain = "ADC_GAIN_1_4";
        zephyr,reference = "ADC_REF_INTERNAL";
        zephyr,vref-mv = <3894>;
        zephyr,acquisition-time = <ADC_ACQ_TIME_DEFAULT>;
        zephyr,resolution = <12>;
    };
};
```

- We add an alias my-adc that points to &adc0, and my-adc-channel that points to adc0_ch0. Aliases allow our application code to easily reference these nodes without remembering the full path.
- We ensure status = "okay" to enable the ADC hardware.
- We set #address-cells = <1>; and #size-cells = <0>; to match what the ADC binding expects for channel sub-nodes.
- We create a sub-node channel@0 (with reg = <0>) representing channel 0 of this ADC. The zephyr,gain, zephyr,reference, and zephyr,vref-mv properties match what we learned from the binding files. They describe how the ADC driver should configure the channel:

1. ADC_GAIN_1_4 might mean we use an attenuation that allows for a larger input voltage range.
2. ADC_REF_INTERNAL means we’re using the internal reference voltage.
3. zephyr,vref-mv = <3894> sets the effective reference voltage in millivolts, factoring in attenuation. In this example, we’ve chosen ~3.9 V to match the internal configuration of ESP32’s ADC with an 11 dB attenuation (about gain of 1/4 or more precisely 1/3.54).
4. zephyr,resolution = <12> sets a 12-bit resolution, meaning the ADC returns values between 0 and 4095.

### src/main.c

```c
// Get Devicetree configurations
#define MY_ADC_CH DT_ALIAS(my_adc_channel)
static const struct device *adc = DEVICE_DT_GET(DT_ALIAS(my_adc));
static const struct adc_channel_cfg adc_ch = ADC_CHANNEL_CFG_DT(MY_ADC_CH);
```

- `DEVICE_DT_GET(DT_ALIAS(my_adc))` uses the alias my-adc defined in the overlay. This returns a device struct representing our ADC hardware.
- `ADC_CHANNEL_CFG_DT(MY_ADC_CH)` macro uses the properties defined in the my-adc-channel node (like gain, reference, resolution) to create an adc_channel_cfg structure automatically.
- `DT_PROP(MY_ADC_CH, zephyr_vref_mv)` fetches the zephyr,vref-mv property right from Devicetree, giving the code a clean and dynamic way to access hardware configuration values.

No hardcoded addresses, no platform-specific #defines. If we change the attenuation or the reference voltage in the DTS overlay later, the code adapts automatically after rebuilding.

All of this dynamic configuration works because of the binding files. The `espressif,esp32-adc.yaml` and generic `adc-controller.yaml` files told Zephyr’s build system how to interpret the **zephyr,gain**, **zephyr,vref-mv**, and other properties. It also enforced that reg = <0> for our channel node is valid, and guided the macros used by `DT_ALIAS()` and `DT_PROP()`.

Without bindings, you’d have to hardcode many platform-specific details in your code. You’d lose the flexibility and consistency that Devicetree brings.
Bindings are like a contract between your DTS and the driver code: they define what’s allowed what each property means, and how to translate those properties into driver configuration. In other words, they define Devicetree node interfaces.

The approach we used here isn’t unique to ADCs. Any peripheral—UART, I2C, SPI, PWM—follows a similar pattern:

- Find the compatible string for the device (e.g. "espressif,esp32-uart").
- Locate the corresponding binding file in zephyr/dts/bindings.
- Review required and optional properties in the binding file to understand what you can set in your DTS or overlay.
- Write or update your DTS/overlay to include these properties.
- Use Devicetree macros (like DT_PROP(), DT_NODELABEL(), DT_ALIAS()) in your C code to access these properties.
- Configure and use the driver code as per Zephyr’s APIs, confident that your properties match what the driver expects.

As you become more familiar with the process, you’ll rely less on guesswork. The binding files become a source of truth, guiding you in how to set up each device. Instead of searching datasheets and hoping your code aligns with the driver’s assumptions, you let Devicetree and bindings ensure correctness. Be aware that **this assumes your driver code has been provided for you**!
