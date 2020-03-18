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

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct sampleStream_Info * DAC_stream;

/* Private function prototypes -----------------------------------------------*/

/*
 * sampleAvailable checks if a new sample is available
 */
static uint8_t sampleAvailable();

/* Exported functions --------------------------------------------------------*/

HAL_StatusTypeDef DAC_streamStart(struct sampleStream_Info * sampleStream) {
	DAC_stream = sampleStream;
	DAC_stream->state = ACTIVE;

	return HAL_DAC_Start(DAC_stream->hdac, DAC_stream->DAC_Channel);
}

HAL_StatusTypeDef DAC_streamRetart() {
	return HAL_DAC_Start(DAC_stream->hdac, DAC_stream->DAC_Channel);
}

HAL_StatusTypeDef DAC_streamUpdate() {
	uint64_t value;

	if (sampleAvailable()) {
		DAC_stream->lastSampleOut += 1;
		if (DAC_stream->lastSampleOut >= DAC_stream->length) {
			DAC_stream->lastSampleOut = 0;
		}
		value = (DAC_stream->stream)[DAC_stream->lastSampleOut];

		return HAL_DAC_SetValue(DAC_stream->hdac, DAC_stream->DAC_Channel, DAC_ALIGN_12B_R, (uint32_t)value);
	}
	else {
		return HAL_OK;
	}
}

static uint8_t sampleAvailable() {
	// Check if a new sample is available
	if (DAC_stream->lastSampleIn < DAC_stream->lastSampleOut) {
		if (DAC_stream->lastSampleIn + DAC_stream->length > DAC_stream->lastSampleOut) {
			return 1;
		}
	}
	else {
		if (DAC_stream->lastSampleIn > DAC_stream->lastSampleOut) {
			return 1;
		}
	}
	return 0;
}

HAL_StatusTypeDef DAC_streamStop() {
	DAC_stream->state = INACTIVE;

	return HAL_DAC_Stop(DAC_stream->hdac, DAC_stream->DAC_Channel);
}

