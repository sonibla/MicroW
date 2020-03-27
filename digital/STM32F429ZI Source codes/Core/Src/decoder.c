/**
  ******************************************************************************
  * @file           : decoder.c
  * @brief          : Decoder API
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

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct sampleStream_Info * DAC_stream;
struct bitStream_Info * UART_stream;

/* Private function prototypes -----------------------------------------------*/

/*
 * synchronize return HAL_ERROR if UART_stream isn't synchronized, else and HAL_OK
 * the function also tries to synchronize the stream.
 */
static HAL_StatusTypeDef synchronize();

/*
 * dataAvailable returns 1 if untreated data is available, 0 else.
 */
static uint8_t dataAvailable();

/*
 * saveValue saves provided value into sample stream to make it available to the DAC
 */
static HAL_StatusTypeDef saveSample(uint64_t value);

/*
 * posiLastIncomingBit gives the position of the last incoming bit
 */
static uint32_t posiLastIncomingBit();

/*
 * posiNextOutgoingBit gives the position of the next bit that will go out
 */
static uint32_t posiNextOutgoingBit();

/*
 * posiNextNeededBit gives the position of the last bit needed to complete a sample
 */
static uint32_t posiNextNeededBit();

/* Exported functions --------------------------------------------------------*/

HAL_StatusTypeDef decoder_streamStart(struct bitStream_Info * bitStream, struct sampleStream_Info * sampleStream) {
	DAC_stream = sampleStream;
	UART_stream = bitStream;

	if (UART_stream->length < 1 + WORD_LENGTH/8) {
		return HAL_ERROR;
	}

	if (UART_stream->lastBitOut != 0) {
		UART_stream->lastBitOut = 0;
	}

	DAC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;

	return HAL_OK;
}

HAL_StatusTypeDef decoder_streamRestart() {
	DAC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;
	UART_stream->synchronized = 0;

	if (UART_stream->lastBitOut != 0) {
		UART_stream->lastBitOut = 0;
	}

	return HAL_OK;
}

HAL_StatusTypeDef decoder_streamUpdate() {
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t bit = 0; // For for loop
	uint8_t bitCursor = UART_stream->lastBitOut;
	uint16_t byteCursor = UART_stream->lastByteOut;
	uint64_t value = 0;

	status = synchronize();
	if (status == HAL_BUSY) {
		// Waiting for sync signal (not an error)
		return HAL_OK;
	}
	if (status != HAL_OK) {
		return status;
	}

	if (dataAvailable() == 0) {
		// No new data
		return HAL_OK;
	}

	for (bit = 0; bit < WORD_LENGTH; bit++) {
		value += (((UART_stream->stream)[byteCursor] & (0x80 >> bitCursor)) >> (7-bitCursor)) << (WORD_LENGTH-1-bit);
		bitCursor += 1;
		if (bitCursor == 8) {
			bitCursor = 0;
			byteCursor += 1;
		}
		if (byteCursor >= UART_stream->length) {
			byteCursor = 0;
		}
	}

	UART_stream->lastBitOut = bitCursor;
	UART_stream->lastByteOut = byteCursor;

	return saveSample(value);
}

static HAL_StatusTypeDef synchronize() {
	// Try to synchronize...
	if ((UART_stream->stream)[UART_stream->lastByteIn] == SYNC_SIGNAL) {

		UART_stream->synchronized = 1;
		UART_stream->lastBitOut = 0;
		UART_stream->lastByteOut = UART_stream->lastByteIn + 1;
		if (UART_stream->lastByteOut >= UART_stream->length) {
			UART_stream->lastByteOut = 0;
		}
	}

	if (UART_stream->synchronized == 0) {
		return HAL_BUSY;
	}
	else {
		return HAL_OK;
	}
}

static uint32_t posiLastIncomingBit() {
	return (UART_stream->lastByteIn + 1) * 8 - 1;
}

static uint32_t posiNextOutgoingBit() {
	return (UART_stream->lastByteOut * 8) + UART_stream->lastBitOut;
}

static uint32_t posiNextNeededBit() {
	if (posiNextOutgoingBit() + WORD_LENGTH >= UART_stream->length * 8) {
		return posiNextOutgoingBit() + WORD_LENGTH - UART_stream->length * 8;
	}
	else {
		return posiNextOutgoingBit() + WORD_LENGTH;
	}
}

static uint8_t dataAvailable() {
	if (posiNextNeededBit()/8 == posiLastIncomingBit()/8 && posiLastIncomingBit() >= posiNextNeededBit()) {
		return 1;
	}
	else {
		return 0;
	}
}

static HAL_StatusTypeDef saveSample(uint64_t value) {
	DAC_stream->lastSampleIn += 1;
	if (DAC_stream->lastSampleIn >= DAC_stream->length) {
		DAC_stream->lastSampleIn = 0;
	}

	if (DAC_stream->lastSampleIn == DAC_stream->lastSampleOut) {
		// Overrun error (DAC too slow, or buffer too short)
		return HAL_ERROR;
	}

	(DAC_stream->stream)[DAC_stream->lastSampleIn] = value;
	return HAL_OK;
}

HAL_StatusTypeDef decoder_streamStop() {
	DAC_stream->state = INACTIVE;
	UART_stream->state = INACTIVE;
	UART_stream->synchronized = 0;

	return HAL_OK;
}
