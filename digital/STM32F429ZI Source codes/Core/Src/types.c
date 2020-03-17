/*
 * types.c
 *
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

	bitStream->lastBitOut = sizeof(uint8_t);

	bitStream->lastByteIn = bitStream->length;
	bitStream->lastByteOut = bitStream->length;
	bitStream->bytesSinceLastSyncSignal = SYNC_PERIOD + 1;

    bitStream->stream = NULL;

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

    return HAL_OK;

}
