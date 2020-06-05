/**
  ******************************************************************************
  * @file           : encoder.c
  * @brief          : Encoder API
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
#include "config.h"
#include "links.h"

/* Private variables ---------------------------------------------------------*/

struct sampleStream_Info * ADC_stream;
struct bitStream_Info * UART_stream;
uint16_t maskSample;

/* Private function prototypes -----------------------------------------------*/

static uint64_t mask(uint8_t bits);
static HAL_StatusTypeDef sendTrueByte(uint8_t byte);
static HAL_StatusTypeDef sendByte(uint8_t byte, uint8_t LSB);
static uint64_t getSample();
static void nextSample();
static uint8_t sampleAvailable();
static HAL_StatusTypeDef sendSyncSignal();

/* Exported functions --------------------------------------------------------*/

/**
 * @brief initializes a stream (an auto-completing bitStream according to a sampleStream)
 * 
 * @param sampleStream[IN] pointer to the sampleStream_Info structure
 * @param bitStream[IN] pointer to the bitStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef encoder_streamStart(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream)
{
	ADC_stream = sampleStream;
	UART_stream = bitStream;

	maskSample = mask(WORD_LENGTH);

	ADC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;

	return sendSyncSignal();
}

/**
 * @brief starts a stream without overwriting existing parameters.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @warning encoder_streamStart() must be called at least once before to
 * calling encoder_streamRestart()
 */
HAL_StatusTypeDef encoder_streamRestart()
{
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if ((UART_stream == NULL) || (ADC_stream == NULL))
	{
		return HAL_ERROR;
	}
	
	ADC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;

	return sendSyncSignal();
}

/**
 * @brief updates the streams structures fields : take data from ADC buffer to put it into the UART buffer
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @note should be called at the end of a ADC buffer update to update the UART buffer
 */
HAL_StatusTypeDef encoder_streamUpdate()
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t bit;
	uint8_t bit_value;
	uint8_t byte;
	uint8_t LSB;
	uint64_t sample;
	
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if ((UART_stream == NULL) || (ADC_stream == NULL))
	{
		return HAL_ERROR;
	}

	if (ADC_stream->state == INACTIVE || UART_stream->state == INACTIVE)
	{
		return HAL_BUSY;
	}

	uint64_t debug = 0;
	while(sampleAvailable())
	{
		debug++;
		sample = getSample();

		while (WORD_LENGTH - ADC_stream->bitsOut >= 8)
		{
			// We can fill a full uint8_t with the last new sample
			byte = 0;

			for(bit = 0; bit < 8; bit++)
			{
				bit_value = (sample & (0x01 << (WORD_LENGTH - 1 - ADC_stream->bitsOut))) >> (WORD_LENGTH - 1 - ADC_stream->bitsOut);
				byte += bit_value << (8 - bit);
				ADC_stream->bitsOut += 1;
			}

			// LSB = 7 because considered byte is fully inside a sample
			LSB = 8 - 1;
			status = sendByte(byte, LSB);
			if (status != HAL_OK)
			{
				return status;
			}
		}

		// Check if the above process encoded an entire sample
		if (WORD_LENGTH <= ADC_stream->bitsOut)
		{
			nextSample();
			if (UART_stream->bytesSinceLastSyncSignal + 1 >= SYNC_PERIOD)
			{
				status = sendSyncSignal();
				if (status != HAL_OK)
				{
					return status;
				}
			}
		}
		// Check if we can complete a byte using next sample
		else if (sampleAvailable())
		{
			// Start to fill the byte with the lasts bits of selected sample
			byte = 0;
			bit = 0;

			while(WORD_LENGTH > ADC_stream->bitsOut)
			{
				bit_value = (sample & (0x01 << (WORD_LENGTH - 1 - ADC_stream->bitsOut))) >> (WORD_LENGTH - 1 - ADC_stream->bitsOut);
				byte += bit_value << (8 - bit);
				ADC_stream->bitsOut += 1;
				bit += 1;
			}

			// Now we continue the same process with next sample
			LSB = bit;
			nextSample();
			sample = getSample();

			while(bit < 8)
			{
				bit_value = (sample & (0x01 << (WORD_LENGTH - 1 - ADC_stream->bitsOut))) >> (WORD_LENGTH - 1 - ADC_stream->bitsOut);
				byte += bit_value << (8 - bit);
				ADC_stream->bitsOut += 1;
				bit += 1;
			}

			status = sendByte(byte, LSB);
			if (status != HAL_OK)
			{
				return status;
			}
		}
	}

	encode_FinishedHandle();
	return HAL_OK;
}

/**
 * @brief stops a running stream.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef encoder_streamStop()
{
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if ((UART_stream == NULL) || (ADC_stream == NULL))
	{
		return HAL_ERROR;
	}
	
	ADC_stream->state = INACTIVE;
	UART_stream->state = INACTIVE;

	return HAL_OK;
}

/**
 * @brief checks if sample is available
 * 
 * @return 1 if a sample is available 0 else
 */
static uint8_t sampleAvailable()
{
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if (ADC_stream == NULL)
	{
		return 0;
	}
	
	if (ADC_stream->lastSampleIn != ADC_stream->lastSampleOut)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief changes ADC_stream metadata to consider a new sample (the next one)
 */
static void nextSample()
{
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if (ADC_stream != NULL)
	{
		ADC_stream->lastSampleOut += 1;
		if (ADC_stream->lastSampleOut >= ADC_stream->length)
		{
			ADC_stream->lastSampleOut = 0;
		}
		ADC_stream->bitsOut = 0;
	}
}

/**
 * @brief returns the value of the next sample to encode
 * 
 * @return the sample, as a uint64 number. 0xFFFFFFFFFFFFFFFF in case of error
 */
static uint64_t getSample()
{
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if (ADC_stream == NULL)
	{
		return 0xFFFFFFFFFFFFFFFF;
	}
	
	if (ADC_stream->lastSampleOut + 1 < ADC_stream->length)
	{
		return ((ADC_stream->stream)[1 + ADC_stream->lastSampleOut]) & maskSample;
	}
	else
	{
		return ((ADC_stream->stream)[1 + ADC_stream->lastSampleOut - ADC_stream->length]) & maskSample;
	}
}

/**
 * @brief saves a byte into the UART buffer without modifying data
 * 
 * @param byte[IN] the data to save into the buffer
 * @return HAL status (HAL_OK if no errors occured).
 */
static HAL_StatusTypeDef sendTrueByte(uint8_t byte)
{
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}

	UART_stream->lastByteIn += 1;
	if (UART_stream->lastByteIn >= UART_stream->length)
	{
		UART_stream->lastByteIn = 0;
	}

	if (UART_stream->lastByteIn == UART_stream->lastByteOut)
	{
		// Overrun error (UART too slow)
		return HAL_ERROR;
	}

	UART_stream->stream[UART_stream->lastByteIn] = byte;
	UART_stream->bytesSinceLastSyncSignal += 1;
	return HAL_OK;
}

/**
 * @brief saves a byte into the UART buffer, but toggles the LSB if byte == SYNC_SIGNAL
 * 
 * @param byte[IN] the data to save into the buffer
 * @param LSB[IN] the position of the least significant bit (between 0 and 7)
 * @return HAL status (HAL_OK if no errors occured).
 */
static HAL_StatusTypeDef sendByte(uint8_t byte, uint8_t LSB)
{
	uint8_t mask;

	if (LSB >= 8)
	{
		// Default value in case of error:
		LSB = 8 - 1;
	}

	mask = 0x80 >> LSB;

	if (byte == SYNC_SIGNAL)
	{
		if (byte & mask)
		{
			// byte's LSB is a 1
			byte &= ~mask;
		}
		else
		{
			// byte's LSB is a 0
			byte |= mask;
		}
	}

	return sendTrueByte(byte);
}

/**
 * @brief sends a synchronization byte instead of real data
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
static HAL_StatusTypeDef sendSyncSignal() {
	/* Check that the parameters already exists 
	 * (ie encoder_streamStart() was called before)
	 */
	if ((UART_stream == NULL) || (ADC_stream == NULL))
	{
		return HAL_ERROR;
	}
	
	HAL_StatusTypeDef status = HAL_OK;

	status = sendTrueByte(SYNC_SIGNAL);
	if (status != HAL_OK)
	{
		return status;
	}

	if (ADC_stream->bitsOut != 0)
	{
		nextSample();
	}
	UART_stream->bytesSinceLastSyncSignal = 0;

	return HAL_OK;
}

/**
 * @brief creates a number in which n LSBs are ones
 * For example 00011111 in binary if n = 5
 * 
 * @param bits[IN] the number of ones
 * @return the mask
 */
static uint64_t mask(uint8_t bits)
{
	uint8_t bit;
	uint64_t mask_var = 0;

	for (bit = 0; bit < bits; bit++)
	{
		mask_var <<= 1;
		mask_var += 1;
	}

	return mask_var;
}
