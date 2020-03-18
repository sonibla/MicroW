/**
  ******************************************************************************
  * @file           : types.h
  * @brief          : Header for types.c file.
  *                   Defines structures used by low level APIs
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

#ifndef INC_TYPES_H_
#define INC_TYPES_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "config.h"

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

enum streamState
{
	ACTIVE,
	INACTIVE,
	BUSY
};

/*
 * bitStream_Info contains useful data to continuously send or receive data through UART
 * Basically, it's a uint8_t buffer with a lot of metadata
 */
struct bitStream_Info
{
	UART_HandleTypeDef * huart;
	enum streamState state;
	uint8_t * stream; // Buffer containing raw data flow
	uint8_t lastBitOut; // Last bit successfully treated (in last byte out)
	uint16_t length; // Buffer's size
	uint16_t lastByteIn; // Last new byte in the buffer
	uint16_t lastByteOut; // Last byte successfully treated
	uint16_t bytesSinceLastSyncSignal;
};

/*
 * sampleStream_Info contains useful data to continuously receive data from ADC or send data to DAC
 * Basically, it's a uint32_t buffer with a lot of metadata
 */
struct sampleStream_Info
{
	union {
		DAC_HandleTypeDef * hdac;
		ADC_HandleTypeDef * hadc;
	};
	struct bitStream_Info * defaultBitStream;
	uint32_t * stream; // Buffer containing ADC samples to encode or decode
	enum streamState state;
	uint8_t bitsOut; // Number of bits successfully treated (in lastSampleOut+1 element)
	uint16_t length; // Buffer's size
	uint16_t lastSampleIn; // Last new sample in the buffer
	uint16_t lastSampleOut; // Last sample successfully treated (encoded or given to the DAC)
	uint32_t DAC_Channel;
};

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Private defines -----------------------------------------------------------*/

/*
 * streamInit initializes sampleStream_Info and bitStream_Info structures with consistent data allowing to
 * immediately start the receiver or emitter.
 * The function's parameters depends on module's type because only ADC or DAC is needed, never both.
 *
 */
#if (MODULE_TYPE == MICROW_EMITTER)
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, ADC_HandleTypeDef * hadc, UART_HandleTypeDef * huart);
#else
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, DAC_HandleTypeDef * hdac, uint32_t Channel, UART_HandleTypeDef * huart);
#endif

#endif /* INC_TYPES_H_ */
