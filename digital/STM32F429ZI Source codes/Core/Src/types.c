/**
  ******************************************************************************
  * @file           : types.c
  * @brief          : Initialization of structures used by low lever APIs
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
#include <stdlib.h>

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

#if (MODULE_TYPE == MICROW_EMITTER)
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, ADC_HandleTypeDef * hadc, UART_HandleTypeDef * huart) {
#else
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, DAC_HandleTypeDef * hdac, uint32_t Channel, UART_HandleTypeDef * huart) {
#endif

	// BitStream Initialization
	bitStream->huart = huart;
	bitStream->state = INACTIVE;

	if (MODULE_TYPE == MICROW_EMITTER) {
		bitStream->length = TX_BUFFER_SIZE;
	}
	else {
		bitStream->length = RX_BUFFER_SIZE;
	}

	bitStream->lastBitOut = 8;

	bitStream->lastByteIn = bitStream->length;
	bitStream->lastByteOut = bitStream->length;
	bitStream->bytesSinceLastSyncSignal = SYNC_PERIOD + 1;

    bitStream->stream = NULL;
	bitStream->byte = 0;
	bitStream->synchronized = 0;

    bitStream->stream = malloc(bitStream->length * sizeof(uint8_t));
    if (bitStream->stream == NULL) {
        return HAL_ERROR;
    }

    // SampleStream Initialization

#if (MODULE_TYPE == MICROW_EMITTER)
	sampleStream->hadc = hadc;
#else
	sampleStream->hdac = hdac;
#endif

    sampleStream->length = SAMPLE_BUFFER_SIZE;
	sampleStream->defaultBitStream = bitStream;
	sampleStream->bitsOut = 0;
	sampleStream->lastSampleIn = sampleStream->length;
	sampleStream->lastSampleOut = sampleStream->length;
	sampleStream->state = INACTIVE;

	sampleStream->stream = NULL;
    sampleStream->stream = malloc(sampleStream->length * sizeof(uint32_t));
    if (sampleStream->stream == NULL) {
        return HAL_ERROR;
    }

    uint16_t i = 0;
    for(i=0; i<sampleStream->length; i++) {
    	(sampleStream->stream)[i] = 0xFFFFFFFF;
    }

    return HAL_OK;

}
