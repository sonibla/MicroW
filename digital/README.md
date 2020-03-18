# MicroW - Digital components

## Overview

Every module (receiver and emitter) is equipped with a microcontroller that basically acts as a serial/analog converter (we use [STM32F429ZI](https://www.st.com/en/microcontrollers-microprocessors/stm32f429zi.html) microcontrollers). Radio transmission and encryption is performed by [Digi Xbee® S1 802.15.4 RF Modules](https://www.digi.com/pdf/ds_xbeemultipointmodules.pdf). Those Xbee modules act as a serial line that can reach 250kb/s.

![microcontrollers details](../images/Microcontrollers_details.png "Microcontrollers details")

We use a sampling rate of 16kHz (most modern VoIP and VVoIP communication products sample at this frequency). A timer controlls the sampling rate of the ADC (and the refresh rate of the DAC). With a bit depth of 12 bits per sample, we only need a 192kb/s UART interface if we send samples without any data compression. We actually configured the UART interface at 230400 baud because it's the first standard value greater than 192000 baud.

Source codes of the microcontrollers and corresponding documentation are in [STM32F429ZI Source codes](https://github.com/sonibla/MicroW/tree/master/digital/STM32F429ZI%20Source%20codes) folder.

## Third party documentation

 * [STM32F429ZI Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031020.pdf)
 * [Description of STM32F4 HAL and LL drivers](https://www.st.com/resource/en/user_manual/dm00105879.pdf)
 * [XBee/XBee-PRO S1 802.15.4 Reference Manual](https://www.digi.com/resources/documentation/digidocs/pdfs/90000982.pdf)

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.
