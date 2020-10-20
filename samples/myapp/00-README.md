# Example of custom ADC binding

This example shows how to represent analog input signals and associated
hardware properties in a devicetree bindings.  The described problem
domain was for two analog inputs, one for an op-amp, one for an
RC-divider.

The file `dts/bindings/my,app.yaml` defines a binding for a compatible
`my,app` with the necessary properties.

The file `boards/nrf5340pdk_nrf5340_cpuapp.overlay` adds a devicetree
node with property values.

`src/main.c` extracts the information from the property values, and
configures the ADC to read the op-amp value, showing both the raw
reading and converted millivolts based on the internal reference
voltage.

Test and run:

    west build -b nrf5340pdk_nrf5340_cpuapp
    west flash

Output:

    *** Booting Zephyr OS build zephyr-v2.4.0-752-g62a5179b7b54  ***
    OP-AMP is ADC_0/0
    RC-OUT is ADC_0/1
    RC is 10000 Ohm / 100 uF
    Channel setup: 0
    ADC raw 304 => 66 mV
    ADC raw 261 => 57 mV
    ADC raw 238 => 52 mV
    ADC raw 231 => 50 mV
    ADC raw 220 => 48 mV
