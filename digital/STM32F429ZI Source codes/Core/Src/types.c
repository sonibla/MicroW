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

/**
 * @brief frees the memory space allocated to buffers
 * 
 * @param sampleStream[IN] pointer to the sampleStream_Info structure
 * @param bitStream[IN] pointer to the bitStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 * @note You need to call this function before calling a second time streamInit
 */
HAL_StatusTypeDef streamFree(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream)
{
	if (bitStream->stream != NULL)
	{
		free(bitStream->stream);
	}
	
	if (sampleStream->stream != NULL)
	{
		free(sampleStream->stream);
	}
	
	return HAL_OK;
}

/**
 * @brief Initializes data structures with consistent data to begin with.
 * 
 * Initializes sampleStream_Info and bitStream_Info structures with consistent
 * data allowing to immediately start the receiver or emitter.
 * 
 * @param sampleStream[IN] pointer to the sampleStream_Info structure that will be used by lower level APIs
 * @param bitStream[IN] pointer to the bitStream_Info structure that will be used by lower level APIs
 * @param hadc[IN] (optional) pointer to a ADC_HandleTypeDef structure that contains the configuration information for the specified ADC
 * @param hdac[IN] (optional) pointer to a DAC_HandleTypeDef structure that contains the configuration information for the specified DAC
 * @param DAC_Channel[IN] (optional): The selected DAC channel. This parameter can be one of the following values: DAC_CHANNEL_1 or DAC_CHANNEL_2
 * @param huart[IN] pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.
 * @return HAL status (HAL_OK if no errors occured).
 * @note The function definition depends on module's type because only ADC or DAC is needed, never both.
 */
#if (MODULE_TYPE == MICROW_EMITTER)
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, ADC_HandleTypeDef * hadc, UART_HandleTypeDef * huart) {
#else
HAL_StatusTypeDef streamInit(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream, DAC_HandleTypeDef * hdac, uint32_t Channel, UART_HandleTypeDef * huart) {
#endif

	// BitStream Initialization
	bitStream->huart = huart;
	bitStream->state = INACTIVE;

	if (MODULE_TYPE == MICROW_EMITTER)
	{
		bitStream->length = TX_BUFFER_SIZE;
	}
	else
	{
		bitStream->length = RX_BUFFER_SIZE;
	}

	bitStream->lastBitOut = 8;

	if (MODULE_TYPE == MICROW_EMITTER)
	{
		bitStream->lastByteIn = bitStream->length - 1;
		bitStream->lastByteOut = bitStream->length - 1;
	}
	else
	{
		bitStream->lastByteIn = bitStream->length - 1;
		bitStream->lastByteOut = 0;
	}
	bitStream->bytesSinceLastSyncSignal = SYNC_PERIOD + 1;

    bitStream->stream = NULL;
	bitStream->byte = 0;
	bitStream->synchronized = 0;

    bitStream->stream = malloc(bitStream->length * sizeof(uint8_t));
    if (bitStream->stream == NULL)
    {
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
    if (sampleStream->stream == NULL)
    {
        return HAL_ERROR;
    }

    uint16_t i = 0;
    for(i=0; i<sampleStream->length; i++)
    {
    	(sampleStream->stream)[i] = 0xFFFFFFFF;
    }

    return HAL_OK;

}
