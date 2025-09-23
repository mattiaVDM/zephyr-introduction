# About Zephyr

Zephyr is a collection of libraries and an SDK that contains the toolchaing needed for all different target chips.

## Getting started

### Building the application

To build the application, open the application folder via terminal.
*west* is used to build the application:

`west build -p always -b esp32s3_devkitc/esp32s3/procpu -- -DDTC_OVERLAY_FILE=boards/esp32s3_devkitc.overlay`

`-p always` : pristine, this will clean the build directory if there is one

`-b board`: the board to build the application for, note that `esp32s3` is the processor and `procpu` is the core where the app will be loaded.

`--`: variables after these dashes are passed to CMake

`DDTC_OVERLAY_FILE`: decide tree information file

### Flashing the application

Firmware is flashed using espotool:

`python -m esptool --port '<port>' --chip auto --baud 921600 --before default_reset --after hard_reset write_flash -u --flash_size detect 0x0 <binary>`

`--port`: the port where the device is connected.

`--chip auto`: esptool will automatically detect the chip to flash

`--baud 921600`: the baud rate of the flash operation

`--before default_reset`: reset the chip to bootloader mode before flashing

`--after hard_reset`: reset the board after flashing it

`write_flash`: command to write the flash

`-u`: unset the protection bit (useful is some board has the protection bit locked)

`--flash_size detect`: tell esptool to automatically detect the size of the flash in the chip, making sure the binary is compatible

`0x0`: starting address for flashing

`<binary>`: .bin file to be flashed

### Connect to serial port

To connect to serial port, run the following commnd passing the port and the baud rate:

`python -m serial.tools.miniterm "<port>" 115200`
