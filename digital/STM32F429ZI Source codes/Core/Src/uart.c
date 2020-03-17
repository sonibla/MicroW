/*
 * uart.c
 *
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

static uint8_t dataAvailable();

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
		return HAL_UART_Transmit_DMA(UART_stream->huart, &((UART_stream->stream)[UART_stream->lastByteOut]), 1);
	}

	return HAL_OK;
}

HAL_StatusTypeDef UARTTx_streamStart(struct bitStream_Info * bitStream) {
	UART_stream = bitStream;

	UART_stream->state = ACTIVE;

	return UARTTx_streamUpdate();
}

HAL_StatusTypeDef UARTTx_streamRetart() {
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

HAL_StatusTypeDef UART_RxStreamInit(UART_HandleTypeDef * huart, uint8_t * buffer, uint16_t size) {

	if ((*huart).Init.Mode != UART_MODE_TX_RX && (*huart).Init.Mode != UART_MODE_RX) {
		return HAL_ERROR;
	}

	RxStream.huart = huart;
	RxStream.buffer = buffer;
	RxStream.cursor = 0;
	RxStream.byte = 0;
	RxStream.size = size;
	RxStream.active = 1;

	return HAL_UART_Receive_DMA(huart, &(RxStream.byte), 1);
}

void UART_RxStreamHandle(UART_HandleTypeDef * huart) {
	// Immediately restart the UART so that we don't miss any bit
	uint8_t value = RxStream.byte;
	HAL_UART_Receive_DMA(huart, &(RxStream.byte), 1);

	if (RxStream.active) {
		// Save the last value in the buffer
		RxStream.cursor += 1;
		if (RxStream.cursor == RxStream.size) {
			RxStream.cursor = 0;
		}
		(RxStream.buffer)[RxStream.cursor] = value;
	}
	RxFinishedHandle(RxStream.cursor);
}

void UART_RxStreamStop() {
	RxStream.active = 0;
}

/*=============================================================================
              ##### Receive and transmit functions #####
=============================================================================*/

static uint8_t dataAvailable() {
	// Check if there is new data in the buffer
	if (UART_stream->lastByteIn < UART_stream->lastByteOut) {
		if (UART_stream->lastByteIn + UART_stream->length > UART_stream->lastByteOut) {
			return 1;
		}
	}
	else {
		if (UART_stream->lastByteIn > UART_stream->lastByteOut) {
			return 1;
		}
	}

	return 0;
}
