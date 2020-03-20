# MicroW - Source codes

## Overview

Main C source codes are in [Src](Core/Src) and [Inc](Core/Inc) folders.

Main API to manage the receiver or emitter is in [links.h](Core/Inc/links.h). There are lower lever API that focus on peripherals, for example [UART](Core/Inc/uart.h), [ADC](Core/Inc/adc.h), [DAC](Core/Inc/dac.h), [Timers](Core/Inc/timer.h). There are also APIs for [encoding](Core/Inc/encoder.h) and [decoding](Core/Inc/decoder.h) data.

## Usage

### Building instructions

Since the microcontroller we use has an ARM processor ([Cortex-M4](https://developer.arm.com/ip-products/processors/cortex-m/cortex-m4)), you'll need the ARM compiler. You can use this command line to install it on Debian-based operating systems:
```
sudo apt install gcc-arm-none-eabi
```

You'll have to build the source codes twice. Once for the *emitter* module, and once for the *receiver* module.
To configure wether you're building emitter or receiver binaries, you have to modify [config.h](https://github.com/sonibla/MicroW/blob/master/digital/STM32F429ZI%20Source%20codes/Core/Inc/config.h) file on line 26:

To build emitter binaries :
```
#define MODULE_TYPE MICROW_EMITTER
```

To build receiver binaries :
```
#define MODULE_TYPE MICROW_RECEIVER
```

Once you've set this setting, you can build the binaries:
```
cd Build
make all
```
You'll have to download ```MicroW.bin``` file into your STM32F429ZI microcontroller. 

### Wiring

On the *emitter* module, connect the analog input to ```PAO``` pin (ADC) and the Xbee to ```PA9``` pin (UART_TX).

On the *receiver* module, connect the analog output to ```PA4``` pin (DAC) and the Xbee to ```PA10``` pin (UART_RX).

### Porting to another microcontroller

The easyest way to port this project to another microcontroller (after making sure the peripherals fit the requirements) is to create a new project (for example in STM32CubeIDE), configure it according to your microcontroller, then import the codes.

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.

MicroW source code is licensed under the [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause).
