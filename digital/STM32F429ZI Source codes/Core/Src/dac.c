/**
  ******************************************************************************
  * @file           : dac.c
  * @brief          : Digital to Analog Converter API
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
#include "types.h"

/* Private variables ---------------------------------------------------------*/

struct sampleStream_Info * DAC_stream;
uint16_t maskSample;

/* Private function prototypes -----------------------------------------------*/

static uint64_t mask(uint8_t bits);
static uint8_t sampleAvailable();

/* Exported functions --------------------------------------------------------*/

/**
 * @brief initializes a stream to continuously output analog data
 * 
 * @param sampleStream[IN] pointer to the sampleStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 * @note The DAC will automatically start
 */
HAL_StatusTypeDef DAC_streamStart(struct sampleStream_Info * sampleStream)
{
	DAC_stream = sampleStream;
	DAC_stream->state = ACTIVE;

	maskSample = mask(SAMPLE_SIZE);

	return HAL_DAC_Start(DAC_stream->hdac, DAC_stream->DAC_Channel);
}

/**
 * @brief starts a stream without overwriting existing parameters.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef DAC_streamRestart()
{
	if (DAC_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	return HAL_DAC_Start(DAC_stream->hdac, DAC_stream->DAC_Channel);
}

/**
 * @brief should be called at the end of new data saving
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef DAC_streamUpdate()
{
	uint64_t value;
	if (DAC_stream == NULL)
	{
		return HAL_ERROR;
	}

	if (sampleAvailable())
	{
		DAC_stream->lastSampleOut += 1;
		if (DAC_stream->lastSampleOut >= DAC_stream->length)
		{
			DAC_stream->lastSampleOut = 0;
		}
		value = (DAC_stream->stream)[DAC_stream->lastSampleOut];

		if ((value & maskSample) != value)
		{
			return HAL_ERROR;
		}

		return HAL_DAC_SetValue(DAC_stream->hdac, DAC_stream->DAC_Channel, DAC_ALIGN_12B_R, (uint32_t)value);
	}
	else
	{
		return HAL_OK;
	}
}

/**
 * @brief stops a running stream.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef DAC_streamStop()
{
	if (DAC_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	DAC_stream->state = INACTIVE;

	return HAL_DAC_Stop(DAC_stream->hdac, DAC_stream->DAC_Channel);
}

/**
 * @brief checks if sample is available
 * 
 * @return 1 if a sample is available 0 else
 */
static uint8_t sampleAvailable()
{
	if (DAC_stream == NULL)
	{
		return 0;
	}
	
	if (DAC_stream->lastSampleIn != DAC_stream->lastSampleOut)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**
 * @brief creates a number in which n LSBs are ones
 * 
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

