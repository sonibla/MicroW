/**
  ******************************************************************************
  * @file           : config.h
  * @brief          : Basic configuration
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020, Alban Benmouffek, Matthieu Planas
  * All rights reserved.</center></h2>
  *
  * This software component is licensed under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_

#define MICROW_EMITTER 0
#define MICROW_RECEIVER 1

// Emitter / Receiver config
#define MODULE_TYPE MICROW_EMITTER

// UART config
#define RX_BUFFER_SIZE 128
#define TX_BUFFER_SIZE 128

// ADC/DAC config
#define SAMPLE_BUFFER_SIZE 64
#define SAMPLE_SIZE 12

// Encode/decode config
#define WORD_LENGTH SAMPLE_SIZE
#define SYNC_SIGNAL 0xFF
#define SYNC_PERIOD 64

#endif /* INC_CONFIG_H_ */
