/**
  ******************************************************************************
  * @file           : uart.c
  * @brief          : USART API
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

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "links.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/

struct UART_Stream
{
	UART_HandleTypeDef * huart;
	uint8_t * buffer;
	uint8_t byte;
	uint8_t active; // boolean, indicates if the stream is running
	uint16_t size;
	uint16_t cursor;
};

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct UART_Stream RxStream;
struct bitStream_Info * UART_stream;

/* Private function prototypes -----------------------------------------------*/

/*
 * dataAvailable returns 1 if new data is in the buffer
 */
static uint8_t dataAvailable();

/*
 * saveByte saves given byte into the buffer
 */
static void saveByte(uint8_t byte);

/* Exported functions --------------------------------------------------------*/

/*=============================================================================
                  ##### Transmit functions #####
=============================================================================*/

HAL_StatusTypeDef UARTTx_streamUpdate() {
	if (UART_stream->state != ACTIVE) {
		return HAL_BUSY;
	}

	if (dataAvailable()) {
		UART_stream->state = BUSY;
		UART_stream->lastByteOut += 1;
		if (UART_stream->lastByteOut >= UART_stream->length) {
			UART_stream->lastByteOut = 0;
		}

		UART_stream->byte = (UART_stream->stream)[UART_stream->lastByteOut];

		return HAL_UART_Transmit_DMA(UART_stream->huart, &(UART_stream->byte), 1);
	}

	return HAL_OK;
}

HAL_StatusTypeDef UARTTx_streamStart(struct bitStream_Info * bitStream) {

	UART_stream = bitStream;

	UART_stream->state = ACTIVE;

	return UARTTx_streamUpdate();
}

HAL_StatusTypeDef UARTTx_streamRestart() {
	UART_stream->state = ACTIVE;

	return UARTTx_streamUpdate();
}

HAL_StatusTypeDef UARTTx_streamStop() {
	UART_stream->state = INACTIVE;
	return HAL_OK;
}

/*=============================================================================
                  ##### Receive functions #####
=============================================================================*/

HAL_StatusTypeDef UARTRx_streamStart(struct bitStream_Info * bitStream) {
	UART_HandleTypeDef * huart;
	UART_stream = bitStream;

	huart = UART_stream->huart;
	if ((*huart).Init.Mode != UART_MODE_TX_RX && (*huart).Init.Mode != UART_MODE_RX) {
		return HAL_ERROR;
	}

	UART_stream->state = BUSY;
	return HAL_UART_Receive_DMA(UART_stream->huart, &(UART_stream->byte), 1);
}

HAL_StatusTypeDef UARTRx_streamRestart() {
	UART_stream->state = BUSY;
	return HAL_UART_Receive_DMA(UART_stream->huart, &(UART_stream->byte), 1);
}

HAL_StatusTypeDef UARTRx_streamUpdate() {
	// Immediately restart the UART so that we don't miss any bit
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t value = UART_stream->byte;

	if (UART_stream->state != INACTIVE) {
		status = UARTRx_streamRestart();
	}
	else {
		return HAL_ERROR;
	}

	saveByte(value);

	UARTRx_FinishedHandle();
	return status;
}

static void saveByte(uint8_t byte) {
	UART_stream->lastByteIn += 1;
	if (UART_stream->lastByteIn >= UART_stream->length) {
		UART_stream->lastByteIn = 0;
	}
	(UART_stream->stream)[UART_stream->lastByteIn] = byte;
}

HAL_StatusTypeDef UARTRx_streamStop() {
	UART_stream->state = INACTIVE;

	return HAL_UART_Abort(UART_stream->huart);
}

/*=============================================================================
              ##### Receive and transmit functions #####
=============================================================================*/

static uint8_t dataAvailable() {
	// Check if there is new data in the buffer
	if (UART_stream->lastByteIn != UART_stream->lastByteOut) {
		return 1;
	}
	else {
		return 0;
	}
}
