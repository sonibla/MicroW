# MicroW - Source codes

## Table of contents

- [Overview](#overview)
- [Usage](#usage)
  * [Building instructions](#building-instructions)
  * [Wiring](#wiring)
  * [Porting to another microcontroller](#porting-to-another-microcontroller)
- [API reference](#api-reference)
  * [Configuration (config.h)](#configuration-configh)
  * [Main API (links.h)](#main-api-linksh)
  * [Data structures (types.h)](#data-structures-typesh)
  * [ADC (adc.h)](#adc-adch)
  * [DAC (dac.h)](#dac-dach)
  * [Encoder (encoder.h)](#encoder-encoderh)
  * [Decoder (decoder.h)](#decoder-decoderh)
  * [Timer (timer.h)](#timer-timerh)
  * [USART (uart.h)](#usart-uarth)
- [Detailed explanations](#detailed-explanations)
  * [Clocks](#clocks)
  * [ADC](#adc)
  * [DAC](#dac)
  * [Timers](#timers)
  * [NVIC](#nvic)
  * [USART](#usart)
  * [DMA](#dma)
  * [Encoding and decoding data](#encoding-and-decoding-data)
- [License](#license)

## Overview

The aim of these source codes is to continuously sample analog data, transmit it via a serial line, then put it back to analog on another microcontroller.
For more details please read [detailed explanations](#detailed-explanations) section.

Main C source codes are in [Src](Core/Src) and [Inc](Core/Inc) folders.

Main API to manage the receiver or emitter is in [links.h](Core/Inc/links.h). There are lower lever API that focus on peripherals, for example [UART](Core/Inc/uart.h), [ADC](Core/Inc/adc.h), [DAC](Core/Inc/dac.h), [Timers](Core/Inc/timer.h). There are also APIs for [encoding](Core/Inc/encoder.h) and [decoding](Core/Inc/decoder.h) data. You may Read The Fine Manual about API in [API reference](#api-reference) section.

MicroW source code rely on [HAL drivers](https://www.st.com/resource/en/user_manual/dm00105879.pdf).

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

## API reference

### Configuration (config.h)
[config.h](Core/Inc/config.h) contains many defines that makes it easy to build a custom version of MicroW.

#### `MODULE_TYPE`

Set it to `MICROW_EMITTER` or `MICROW_RECEIVER` to determine which MicroW module you will build.

#### `RX_BUFFER_SIZE`

Determines the length of the uint8_t array that will contain raw serial data, on the receiver side.
Should not be below ADC's bit depth divided by 8, or existing data will be wiped out by incoming bytes before being decoded.

Default value : 128

#### `TX_BUFFER_SIZE`

Determines the length of the *uint8_t* array that will contain raw serial data, on the receiver side.
Should not be below ADC's bit depth divided by 8, or it won't be possible to save encoded data into this buffer.

Default value : 128

#### `SAMPLE_BUFFER_SIZE`

Determines the length of the *uint32_t* array that will contain ADC and DAC samples.

Default value : 64

#### `SAMPLE_SIZE`

The ADC and DAC bit depth. Note that if you change this value you'll also have to change it in [main.c](Core/Src/main.c).

Default value : 12

#### `WORD_LENGTH`

Tells the enocder and decoder how many bits contain information in *uint32_t* variables containing samples. Should be equal to `SAMPLE_SIZE`.

Default value : 12

#### `SYNC_SIGNAL`

To synchronize emitter's encoder and receiver's decoder, the encoder sometimes sends a one-byte synchronization signal. For more details, please read [encoding and decoding data](#encoding-and-decoding-data) section.

Default value : 0xFF

#### `SYNC_PERIOD`

Determines how many bytes the encoder waits to send a new synchronization signal. A value that is too low may cause errors due to a too low flow rate of actual data. A too high value may cause errors because of a de-synchronization of the decoder.

Default value : 64

#### `ERROR_HANDLING`

Determines what to do in case of error.

Possible values:
 - `NOTHING`
 - `RESTART`
 - `STOP`
 - `INFINITE_LOOP`

Default value : `RESTART`

### Main API (links.h)

The goal of this API is to create links between lower lever MicroW APIs : calling the right function at the right time and managing events, for example end of data transfers, errors...

#### `emitter_start`
```
HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, 
                                ADC_HandleTypeDef * hadc, 
                                TIM_HandleTypeDef * htim);
```
emitter_start is a non blocking function that does everything necessary to automatically receive analog values and send them via serial.

##### Parameters
- **huart**: pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.
- **hadc**: pointer to a ADC_HandleTypeDef structure that contains the configuration information for the specified ADC.
- **htim**: pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.

##### Return values
- **HAL**: status

#### `emitter_stop`
```
HAL_StatusTypeDef emitter_stop(void);
```
emitter_stop is a non blocking function that does everything necessary to stop the emitter

##### Return values
- **HAL**: status

#### `receiver_start`
```
HAL_StatusTypeDef receiver_start(UART_HandleTypeDef * huart, 
                                 DAC_HandleTypeDef * hdac, 
                                 uint32_t DAC_Channel, 
                                 TIM_HandleTypeDef * htim);
```
receiver_start is a non blocking function that does everything necessary to automatically receive a serial stream and convert received values into an analog signal.

##### Parameters
- **huart**: pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.
- **hdac**: pointer to a DAC_HandleTypeDef structure that contains the configuration information for the specified DAC.
- **DAC_Channel**: The selected DAC channel. This parameter can be one of the following values:
  * DAC_CHANNEL_1: DAC Channel1 selected
  * DAC_CHANNEL_2: DAC Channel2 selected
- **htim**: pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.
  
##### Return values
- **HAL**: status


#### `receiver_stop`
```
HAL_StatusTypeDef receiver_stop(void);
```
receiver_stop is a non blocking function that does everything necessary to stop the receiver

##### Return values
- **HAL**: status

### Data structures (types.h)

[types.h](Core/Inc/types.h) contains definition of most used data structures in MicroW API, and initialization functions.

#### `streamState`
```
enum streamState
{
    ACTIVE,
    INACTIVE,
    BUSY
};
```

Enumeration of possible states of a data stream. 

### `bitStream_Info`
```
struct bitStream_Info
{
UART_HandleTypeDef * huart;
    enum streamState state;
    uint8_t * stream;
    uint8_t byte;
    uint8_t lastBitOut;
    uint8_t synchronized;
    uint16_t length;
    uint16_t lastByteIn;
    uint16_t lastByteOut;
    uint16_t bytesSinceLastSyncSignal;
};
```
bitStream_Info structures contains useful data to continuously send or receive data through UART. Basically, it's a *uint8_t* buffer with a lot of metadata.

##### Fields
- **huart**: pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.
- **state**: streamState enumeration that tells if the stream is active or not
- **stream**: the actual *uint8_t* buffer containing raw serial data
- **byte**: acts as a *uint8_t* buffer when calling HAL drivers to receive or send one byte. For more explanations on why MicroW sends and receives one byte at a time, please refer to [USART detailed explanations](#usart) section.
- **lastBitOut**: index of last bit that went out of the buffer (between 0 and 7)
- **synchronized**: bool that tells if decoder is synchronized
- **length**: buffer's size
- **lastByteIn**: last incoming byte in the buffer (it was put here by the encoder or the UART receiver)
- **lastByteOut**: last byte successfully out of the buffer (was treated by the decoder or the UART transmitter)
- **bytesSinceLastSyncSignal**: counts bytes since the last synchronization signal


### `sampleStream_Info`
```
struct sampleStream_Info
{
    union {
        DAC_HandleTypeDef * hdac;
        ADC_HandleTypeDef * hadc;
    };
    struct bitStream_Info * defaultBitStream;
    uint32_t * stream;
    enum streamState state;
    uint8_t bitsOut;
    uint16_t length;
    uint16_t lastSampleIn;
    uint16_t lastSampleOut;
    uint32_t DAC_Channel;
};
```
sampleStream_Info structures contains useful data to continuously receive data from ADC or send data to DAC. Basically, it's a uint32_t buffer with a lot of metadata.

##### Fields
- **hadc**: pointer to a ADC_HandleTypeDef structure that contains the configuration information for the specified ADC.
- **hdac**: pointer to a DAC_HandleTypeDef structure that contains the configuration information for the specified DAC.
- **defaultBitStream**: pointer to the associated bitStream_Info structure
- **stream**: the actual *uint32_t* buffer containing samples
- **state**: streamState enumeration that tells if the stream is active or not
- **bitsOut**: number of bits successfully treated (between 0 and ADC's bit depth)
- **length**: buffer's size
- **lastSampleIn**: last new sample in the buffer (it was put here by the decoder or the ADC)
- **lastSampleOut**: last sample successfully treated (encoded or given to the DAC)
- **DAC_Channel**: The selected DAC channel. This parameter can be one of the following values:
  * DAC_CHANNEL_1: DAC Channel1 selected
  * DAC_CHANNEL_2: DAC Channel2 selected

#### `streamInit`
```
#if (MODULE_TYPE == MICROW_EMITTER)
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, 
                             struct bitStream_Info * bitStream, 
                             ADC_HandleTypeDef * hadc, 
                             UART_HandleTypeDef * huart);
#else
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, 
                             struct bitStream_Info * bitStream, 
                             DAC_HandleTypeDef * hdac, 
                             uint32_t Channel, 
                             UART_HandleTypeDef * huart);
#endif
```
Initializes data structures with consistent data to begin with.

##### Parameters
- **sampleStream**: pointer to the sampleStream_Info structure that will be used by lower level APIs
- **bitStream**: pointer to the bitStream_Info structure that will be used by lower level APIs
- **hadc** *(optional)*: pointer to a ADC_HandleTypeDef structure that contains the configuration information for the specified ADC.
- **hdac** *(optional)*: pointer to a DAC_HandleTypeDef structure that contains the configuration information for the specified DAC.
- **DAC_Channel** *(optional)*: The selected DAC channel. This parameter can be one of the following values:
  * DAC_CHANNEL_1: DAC Channel1 selected
  * DAC_CHANNEL_2: DAC Channel2 selected
- **huart**: pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.

##### Return values
- **HAL**: status

### ADC (adc.h)

ADC API helps managing a data flow generated by analog samples

#### `ADC_streamStart`
```
HAL_StatusTypeDef ADC_streamStart(struct sampleStream_Info * sampleStream);
```
ADC_streamStart creates an ADC stream according to provided sampleStream structure. 
The uint32_t buffer inside sampleStream will fill automatically.

##### Parameters
- **sampleStream**: pointer to an initialized sampleStream_Info structure

##### Return values
- **HAL**: status

#### `ADC_streamRestart`
```
HAL_StatusTypeDef ADC_streamRestart(void);
```
ADC_streamRestart starts the ADC without overwriting existing parameters.

##### Return values
- **HAL**: status

#### `ADC_streamUpdate`
```
HAL_StatusTypeDef ADC_streamUpdate(void);
```
ADC_streamUpdate should be called at the end of a conversion to update the buffer.

##### Return values
- **HAL**: status

#### `ADC_streamStop`
```
HAL_StatusTypeDef ADC_streamStop(void);
```
ADC_streamStop stops a running stream.

##### Return values
- **HAL**: status

### DAC (dac.h)

#### `DAC_streamStart`
```
HAL_StatusTypeDef DAC_streamStart(struct sampleStream_Info * sampleStream);
```
DAC_streamStart initializes a stream to continuously and automatically output analog data

##### Parameters

##### Return values
- **HAL**: status

#### `DAC_streamRestart`
```
HAL_StatusTypeDef DAC_streamRestart(void);
```
DAC_streamRestart starts a stream without overwriting existing parameters.

##### Return values
- **HAL**: status

#### `DAC_streamUpdate`
```
HAL_StatusTypeDef DAC_streamUpdate(void);
```
DAC_streamUpdate should be called at the end of new data saving

##### Return values
- **HAL**: status

#### `DAC_streamStop`
```
HAL_StatusTypeDef DAC_streamStop(void);
```
DAC_streamStop stops a running stream.

##### Return values
- **HAL**: status

### Encoder (encoder.h)

#### `encoder_streamStart`
```
HAL_StatusTypeDef encoder_streamStart(struct sampleStream_Info * sampleStream, 
                                      struct bitStream_Info * bitStream);
```
encoder_streamStart initializes a stream (an auto-completing bitStream based on a sampleStream)

##### Parameters
- **sampleStream**: pointer to an initialized sampleStream_Info structure
- **bitStream**: pointer to an initialized bitStream_Info structure

##### Return values
- **HAL**: status

#### `encoder_streamRestart`
```
HAL_StatusTypeDef encoder_streamRestart(void);
```
encoder_streamRestart starts a stream without overwriting existing parameters.

##### Return values
- **HAL**: status

#### `encoder_streamUpdate`
```
HAL_StatusTypeDef encoder_streamUpdate(void);
```
encoder_streamUpdate should be called at the end of a ADC buffer update to update the UART buffer

##### Return values
- **HAL**: status

#### `encoder_streamStop`
```
HAL_StatusTypeDef encoder_streamStop(void);
```
encoder_streamStop stops a running stream.

##### Return values
- **HAL**: status

### Decoder (decoder.h)

#### `decoder_streamStart`
```
HAL_StatusTypeDef decoder_streamStart(struct bitStream_Info * bitStream, 
                                      struct sampleStream_Info * sampleStream);
```
decoder_streamStart initializes a stream to continuously decode data

##### Parameters
- **bitStream**: pointer to an initialized bitStream_Info structure
- **sampleStream**: pointer to an initialized sampleStream_Info structure

##### Return values
- **HAL**: status

#### `decoder_streamRestart`
```
HAL_StatusTypeDef decoder_streamRestart(void);
```
decoder_streamRestart starts a stream without overwriting existing parameters.

##### Return values
- **HAL**: status

#### `decoder_streamUpdate`
```
HAL_StatusTypeDef decoder_streamUpdate(void);
```
decoder_streamUpdate should be called at the end of new data saving

##### Return values
- **HAL**: status

#### `decoder_streamStop`
```
HAL_StatusTypeDef decoder_streamStop(void);
```
decoder_streamStop stops a running stream.

##### Return values
- **HAL**: status

### Timer (timer.h)

#### `Timer_Start`
```
HAL_StatusTypeDef Timer_Start(TIM_HandleTypeDef * htim);
```
Timer_Start enables the counter and interruptions of provided timer

##### Parameters
- **htim**: pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.

##### Return values
- **HAL**: status

#### `Timer_Stop`
```
HAL_StatusTypeDef Timer_Stop(TIM_HandleTypeDef * htim);
```
Timer_Stop disables the counter and interruptions of provided timer

##### Parameters
- **htim**: pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.

##### Return values
- **HAL**: status

### USART (uart.h)

#### `UARTTx_streamStart`
```
HAL_StatusTypeDef UARTTx_streamStart(struct bitStream_Info * bitStream);
```
UARTTx_streamStart initializes a stream to continuously send data

##### Parameters
- **bitStream**: pointer to an initialized bitStream_Info structure

##### Return values
- **HAL**: status

#### `UARTTx_streamRestart`
```
HAL_StatusTypeDef UARTTx_streamRestart(void);
```
UARTTx_streamRestart starts a stream without overwriting existing parameters.

##### Return values
- **HAL**: status

#### `UARTTx_streamUpdate`
```
HAL_StatusTypeDef UARTTx_streamUpdate(void);
```
UARTTx_streamUpdate should be called when the UART buffer has been successfully updated

##### Return values
- **HAL**: status

#### `UARTTx_streamStop`
```
HAL_StatusTypeDef UARTTx_streamStop(void);
```
UARTTx_streamStop stops a running stream.

##### Return values
- **HAL**: status

#### `UARTRx_streamStart`
```
HAL_StatusTypeDef UARTRx_streamStart(struct bitStream_Info * bitStream);
```
UARTRx_streamStart initializes a stream to continuously receive data

##### Parameters
- **bitStream**: pointer to an initialized bitStream_Info structure

##### Return values
- **HAL**: status

#### `UARTRx_streamRestart`
```
HAL_StatusTypeDef UARTRx_streamRestart(void);
```
UARTRx_streamRestart starts a stream without overwriting existing parameters.

##### Return values
- **HAL**: status

#### `UARTRx_streamUpdate`
```
HAL_StatusTypeDef UARTRx_streamUpdate(void);
```
UARTRx_streamUpdate should be called at the end of data reception

##### Return values
- **HAL**: status

#### `UARTRx_streamStop`
```
HAL_StatusTypeDef UARTRx_streamStop(void);
```
UARTRx_streamStop stops a running stream.

##### Return values
- **HAL**: status

## Detailed explanations

In this section, I'll explain in detail how MicroW microcontrollers are configured. For details on how STM32F429ZI and its peripherals work, please refer to [STM32F429ZI Reference Manual](https://www.st.com/resource/en/reference_manual/dm00031020.pdf).

The way MicroW encode and decode data is explained in [Encoding and decoding data](#encoding-and-decoding-data) sub-section.

Code extracts for peripherals configuration come from [main.c](Core/Src/main.c).

### Clocks

### ADC

Like most modern VoIP communication products, MicroW samples at 16kHz. 
This frequency is controlled by a timer, the duration of an ADC conversion is is negligible compared to the sampling period. 

A sampling rate of 16MHz is enough to record voice because barely no one speaks above 8kHz. 
However, MicroW won't be able to record good quality music, because humans can hear sound up to 20kHz.
MicroW only need one ADC peripheral to sample voice because it uses mono sound.

To control the sampling frequency, at every rising edge of the timer ```HAL_ADC_Start_IT()``` is called, which will start a regular conversion and generate a callback when it's finished.

In order to ensure a good sound quality, the bit depth is set by default to **12 bit per sample** (the more is the best).
```
hadc1.Init.Resolution = ADC_RESOLUTION_12B;
```

As conversions are controlled one-by-one by a timer, we only need regular conversions.
```
hadc1.Init.ScanConvMode = DISABLE;
hadc1.Init.ContinuousConvMode = DISABLE;
hadc1.Init.DiscontinuousConvMode = DISABLE;
hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
hadc1.Init.NbrOfConversion = 1;
```

At this state of the project, the CPU isn't overloaded by many signal processing stuff, so we don't need DMA
```
hadc1.Init.DMAContinuousRequests = DISABLE;
```

Right alignment is easier to handle
```
hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
```

End Of Conversion flag (EOC) setting isn't important since we request conversions one-by-one.
```
hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
```

Changing the channel will only change which pin on the microcontroller is used. Rank setting isn't important because we only use one channel.
```
sConfig.Channel = ADC_CHANNEL_0;
sConfig.Rank = 1;
```

This clock prescaler allows the fastest conversion supported by the ADC (knowing clocks configuration). You may change sampling time to increase reliability, but it should stay negligible compared to the sampling period.
```
sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
```

### DAC

The refresh rate of the DAC is controlled by a timer, and should be equal to the sampling time of the ADC (16MHz by default).
The latency introduced by the conversion is negligible compared to the transmission of the radio signal.

To control the refresh frequency, at every rising edge of the timer, ```HAL_DAC_SetValue()``` is called.

DAC output buffer is a feature that reduces the output impedance, so that we don't need an external operational amplifier connected right after the DAC.
```
sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
```

STM32F4's DAC has a bit depth of 12 bit per sample.

### Timers

We use TIM2 timer to set the sampling frequency (16kHz)

Let's use internal clock (90MHz) :
```
sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
```

Here is the formula that explain how to contigure timer's parameters :

<a href="https://www.codecogs.com/eqnedit.php?latex=T_{Timer}&space;=&space;T_{Clock}&space;\times&space;(Prescaler&space;&plus;&space;1)&space;\times&space;(AutoreloadPeriod&space;&plus;&space;1)" target="_blank"><img src="https://latex.codecogs.com/gif.latex?T_{Timer}&space;=&space;T_{Clock}&space;\times&space;(Prescaler&space;&plus;&space;1)&space;\times&space;(AutoreloadPeriod&space;&plus;&space;1)" title="T_{Timer} = T_{Clock} \times (Prescaler + 1) \times (AutoreloadPeriod + 1)" /></a>

<a href="https://www.codecogs.com/eqnedit.php?latex=\frac{F_{Timer}}{F_{Clock}}&space;=&space;\frac{1}{(Prescaler&space;&plus;&space;1)&space;\times&space;(AutoreloadPeriod&space;&plus;&space;1)}" target="_blank"><img src="https://latex.codecogs.com/gif.latex?\frac{F_{Timer}}{F_{Clock}}&space;=&space;\frac{1}{(Prescaler&space;&plus;&space;1)&space;\times&space;(AutoreloadPeriod&space;&plus;&space;1)}" title="\frac{F_{Timer}}{F_{Clock}} = \frac{1}{(Prescaler + 1) \times (AutoreloadPeriod + 1)}" /></a>

Knowing timer and clock frequencies (16kHz and 90MHz), we can set the autoreload period to 5624 and the clock prescaler to 0. It's good to keep a small prescaler to reduce errors.
```
htim2.Init.Prescaler = 0;
htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
htim2.Init.Period = 5624;
```

We want an output trigger when the timer has finished counting from 0 to 5624 :
```
sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
```

No need to use slave mode.
```
sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
```

### NVIC

### USART

### DMA

### Encoding and decoding data

Knowing that we sample 12-bit analog values at 16kHz (192kb/s), and Xbee modules can reach 250kb/s, we don't need data compression. 
We have chosen to send the samples one directly after another in the serial line.

But this method can only work if the decoder knows exactly on which bit a sample starts (the decoder has to be synchronized).
To do that, the encoder periodically sends a *synchronization signal* of one byte (0xFF every 64 bytes by default).
When the decoder receives a synchronization byte, it knows that the next bit it receives will be a sample's most significant bit.

Once the decoder is synchronized, it just have to count incoming bits and comparing that value to ADC's bit depth (12 bit)

If a encoded byte is unintentionnaly the synchronization signal (0xFF by default), the encoder toggles its least significant bit. It may causes an error up to 0.7% on the analog value. 

## License

<a rel="license" href="http://creativecommons.org/licenses/by/4.0/"><img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" /></a><br />This work is licensed under a <a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.

MicroW source code is licensed under the [3-Clause BSD License](https://opensource.org/licenses/BSD-3-Clause).



