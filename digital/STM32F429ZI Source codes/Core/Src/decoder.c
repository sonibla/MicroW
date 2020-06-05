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

/* Private variables ---------------------------------------------------------*/

struct sampleStream_Info * DAC_stream;
struct bitStream_Info * UART_stream;

/* Private function prototypes -----------------------------------------------*/

static HAL_StatusTypeDef synchronize();
static uint8_t dataAvailable();
static HAL_StatusTypeDef saveSample(uint64_t value);
static uint32_t posiLastIncomingBit();
static uint32_t posiNextOutgoingBit();
static uint32_t posiNextNeededBit();

/* Exported functions --------------------------------------------------------*/

/**
 * @brief initializes a stream to continuously decode data
 * 
 * @param bitStream[IN] pointer to an initialized bitStream_Info structure
 * @param sampleStream[IN] pointer to an initialized sampleStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef decoder_streamStart(struct bitStream_Info * bitStream, struct sampleStream_Info * sampleStream)
{
	DAC_stream = sampleStream;
	UART_stream = bitStream;

	if (UART_stream->length < 1 + WORD_LENGTH/8)
	{
		return HAL_ERROR;
	}

	if (UART_stream->lastBitOut != 0)
	{
		UART_stream->lastBitOut = 0;
	}

	DAC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;

	return HAL_OK;
}

/**
 * @brief starts a stream without overwriting existing parameters.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef decoder_streamRestart()
{
	if ((UART_stream == NULL) || (DAC_stream == NULL))
	{
		return HAL_ERROR;
	}
	
	DAC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;
	UART_stream->synchronized = 0;

	if (UART_stream->lastBitOut != 0)
	{
		UART_stream->lastBitOut = 0;
	}

	return HAL_OK;
}

/**
 * @brief updates the streams structures fields : take data from UART buffer to put it into the DAC buffer
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @note should be called at the end of new data saving (see in links.c for details)
 */
HAL_StatusTypeDef decoder_streamUpdate()
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t bit = 0; // For for loop
	uint8_t bitCursor;
	uint16_t byteCursor;
	uint64_t value = 0;
	
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	bitCursor = UART_stream->lastBitOut;
	byteCursor = UART_stream->lastByteOut;

	status = synchronize();
	if (status == HAL_BUSY)
	{
		// Waiting for sync signal (not an error)
		return HAL_OK;
	}
	if (status != HAL_OK)
	{
		return status;
	}

	if (dataAvailable() == 0)
	{
		// No new data
		return HAL_OK;
	}

	for (bit = 0; bit < WORD_LENGTH; bit++)
	{
		value += (((UART_stream->stream)[byteCursor] & (0x80 >> bitCursor)) >> (7-bitCursor)) << (WORD_LENGTH-1-bit);
		bitCursor += 1;
		if (bitCursor == 8)
		{
			bitCursor = 0;
			byteCursor += 1;
		}
		if (byteCursor >= UART_stream->length)
		{
			byteCursor = 0;
		}
	}

	UART_stream->lastBitOut = bitCursor;
	UART_stream->lastByteOut = byteCursor;

	return saveSample(value);
}

/**
 * @brief stops a running stream.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef decoder_streamStop()
{
	if ((UART_stream == NULL) || (DAC_stream == NULL))
	{
		return HAL_ERROR;
	}
	
	DAC_stream->state = INACTIVE;
	UART_stream->state = INACTIVE;
	UART_stream->synchronized = 0;

	return HAL_OK;
}

/**
 * @brief Checks if the UART stream is synchronized with the encoder.
 * This function also tries to synchronize the stream.
 * 
 * @return HAL_ERROR if UART_stream isn't synchronized, else and HAL_OK
 */
static HAL_StatusTypeDef synchronize()
{
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	// Try to synchronize...
	if ((UART_stream->stream)[UART_stream->lastByteIn] == SYNC_SIGNAL)
	{

		UART_stream->synchronized = 1;
		UART_stream->lastBitOut = 0;
		UART_stream->lastByteOut = UART_stream->lastByteIn + 1;
		if (UART_stream->lastByteOut >= UART_stream->length)
		{
			UART_stream->lastByteOut = 0;
		}
	}

	if (UART_stream->synchronized == 0)
	{
		return HAL_BUSY;
	}
	else
	{
		return HAL_OK;
	}
}

/**
 * @brief gives the position of the last incoming bit
 * 
 * @return 0xFFFFFFFF in case of error, else the position of the last incoming bit
 */
static uint32_t posiLastIncomingBit()
{
	if (UART_stream == NULL)
	{
		return 0xFFFFFFFF;
	}
	
	return (UART_stream->lastByteIn + 1) * 8 - 1;
}

/*
 * posiNextOutgoingBit gives the position of the next bit that will go out
 */
/**
 * @brief gives the position of the next bit that will go out
 * 
 * @return 0xFFFFFFFF in case of error, else position of the next bit that will go out
 */
static uint32_t posiNextOutgoingBit()
{
	if (UART_stream == NULL)
	{
		return 0xFFFFFFFF;
	}
	
	return (UART_stream->lastByteOut * 8) + UART_stream->lastBitOut;
}

/**
 * @brief gives the position of the last bit needed to complete a sample
 * 
 * @return 0xFFFFFFFF in case of error, else position of the last bit needed to complete a sample
 */
static uint32_t posiNextNeededBit()
{
	if (UART_stream == NULL)
	{
		return 0xFFFFFFFF;
	}
	
	if (posiNextOutgoingBit() + WORD_LENGTH >= UART_stream->length * 8)
	{
		return posiNextOutgoingBit() + WORD_LENGTH - UART_stream->length * 8;
	}
	else
	{
		return posiNextOutgoingBit() + WORD_LENGTH;
	}
}

/**
 * @brief check if there is nex data in incoming buffer (UART)
 * 
 * @return returns 1 if untreated data is available, 0 else.
 */
static uint8_t dataAvailable()
{
	if (posiNextNeededBit()/8 == posiLastIncomingBit()/8 && posiLastIncomingBit() >= posiNextNeededBit())
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief saves provided value into sample stream to make it available to the DAC
 * 
 * @return HAL status (HAL_ERROR or HAL_OK)
 */
static HAL_StatusTypeDef saveSample(uint64_t value)
{
	if (DAC_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	DAC_stream->lastSampleIn += 1;
	if (DAC_stream->lastSampleIn >= DAC_stream->length)
	{
		DAC_stream->lastSampleIn = 0;
	}

	if (DAC_stream->lastSampleIn == DAC_stream->lastSampleOut)
	{
		// Overrun error (DAC too slow, or buffer too short)
		return HAL_ERROR;
	}

	(DAC_stream->stream)[DAC_stream->lastSampleIn] = value;
	return HAL_OK;
}


