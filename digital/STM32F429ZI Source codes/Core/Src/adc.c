/**
  ******************************************************************************
  * @file           : adc.c
  * @brief          : Analog to Digital Converter API
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

/* Exported functions --------------------------------------------------------*/

/**
 * @brief creates an ADC stream according to provided sampleStream structure.
 * 
 * @param sampleStream[IN] pointer to the sampleStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 * @note The ADC will automatically start 
 */
HAL_StatusTypeDef ADC_streamStart(struct sampleStream_Info * sampleStream)
{
	HAL_StatusTypeDef status;
	ADC_stream = sampleStream;

	status = HAL_ADC_Start_IT(ADC_stream->hadc);
	if (status != HAL_OK)
	{
		return status;
	}

	ADC_stream->state = ACTIVE;
	return HAL_OK;
}

/**
 * @brief starts the ADC without overwriting existing parameters.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef ADC_streamRestart()
{
	HAL_StatusTypeDef status;
	
	if (ADC_stream == NULL)
	{
		return HAL_ERROR;
	}

	status = HAL_ADC_Start_IT(ADC_stream->hadc);
	if (status != HAL_OK)
	{
		return status;
	}

	ADC_stream->state = ACTIVE;
	return HAL_OK;
}

/**
 * @brief should be called at the end of a conversion to update the buffer
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef ADC_streamUpdate()
{
	uint32_t value;
	
	if (ADC_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	value = HAL_ADC_GetValue(ADC_stream->hadc);

	if (ADC_stream->state == INACTIVE)
	{
		return HAL_BUSY;
	}


	ADC_stream->lastSampleIn += 1;
	if (ADC_stream->lastSampleIn >= ADC_stream->length)
	{
		ADC_stream->lastSampleIn = 0;
	}

	if (ADC_stream->lastSampleIn == ADC_stream->lastSampleOut)
	{
		// Overrun error (encoder too slow)
		return HAL_ERROR;
	}

	(ADC_stream->stream)[ADC_stream->lastSampleIn] = value;

	ADC_FinishedHandle();

	return HAL_OK;
}

/**
 * @brief stops a running stream.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef ADC_streamStop()
{
	if (ADC_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	ADC_stream->state = INACTIVE;
	return HAL_ADC_Stop_IT(ADC_stream->hadc);
}
