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

/* Exported types ------------------------------------------------------------*/

/**
 * @brief indicates the state of a data stream
 */
enum streamState
{
	ACTIVE,     /** (0) The stream is ready */
	INACTIVE,   /** (1) The stream isn't used currently (no data flow) */
	BUSY        /** (2) Data is being transfered with a peripheral or another API */
};

/**
 * @brief contains useful data to continuously send or receive data through UART
 * Basically, it's a uint8_t buffer with a lot of metadata
 */
struct bitStream_Info
{
	UART_HandleTypeDef * huart;         
	/** pointer to a USART_HandleTypeDef structure that contains
	the configuration information for the specified USART module. */
	
	enum streamState state;   /** Current state of the stream (active or not) */
	uint8_t * stream;         /** uint8_t buffer containing raw data flow */
	uint8_t byte;             /** The byte to send or receive. Acts as a uint8_t buffer when
	calling HAL drivers to receive or send one byte. For more explanations on why MicroW sends
	and receives one byte at a time, please refer to USART detailed explanations docs section. */
	
	uint8_t lastBitOut;       /** Position of the last bit successfully treated (in last byte out) */
	uint8_t synchronized;     /** Tells if a synchronization signal was received */
	uint16_t length;          /** Buffer's size */
	uint16_t lastByteIn;      /** Position of the last new byte in the buffer */
	uint16_t lastByteOut;     /** Position of the last byte successfully treated */
	uint16_t bytesSinceLastSyncSignal;  /** Number of bytes sent/received since the last sync. signal */
};

/**
 * @brief contains useful data to continuously receive data from ADC or send data to DAC
 * Basically, it's a uint32_t buffer with a lot of metadata
 */
struct sampleStream_Info
{
	union {
		DAC_HandleTypeDef * hdac; /** pointer to a DAC_HandleTypeDef structure
		that contains the configuration information for the specified DAC. */
		
		ADC_HandleTypeDef * hadc; /** pointer to a ADC_HandleTypeDef structure
		that contains the configuration information for the specified ADC. */
	};
	struct bitStream_Info * defaultBitStream;   /** associated bitStream_Info structure (useful for encoder/decoder APIs) */
	uint32_t * stream;          /** Buffer containing ADC samples to encode or decode */
	enum streamState state;     /** Current state of the stream */
	uint8_t bitsOut;            /** Number of bits successfully treated (between 0 and ADC's bit depth) */
	uint16_t length;            /** Buffer's size */
	uint16_t lastSampleIn;      /** Last new sample in the buffer (it was put here by the decoder or the ADC) */
	uint16_t lastSampleOut;     /** Last sample successfully treated (encoded or given to the DAC) */
	uint32_t DAC_Channel;       /** The selected HAL DAC channel. 
	This field can be one of the following values: DAC_CHANNEL_1 or DAC_CHANNEL_2 */
};

/* Exported functions prototypes ---------------------------------------------*/

#if (MODULE_TYPE == MICROW_EMITTER)
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, ADC_HandleTypeDef * hadc, UART_HandleTypeDef * huart);
#else
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, DAC_HandleTypeDef * hdac, uint32_t Channel, UART_HandleTypeDef * huart);
#endif

HAL_StatusTypeDef streamFree(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream);

#endif /* INC_TYPES_H_ */
