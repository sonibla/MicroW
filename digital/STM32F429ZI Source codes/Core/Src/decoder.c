/*
 * decoder.c
 *
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "links.h"
#include "config.h"

/* Private defines -----------------------------------------------------------*/

/* Private typedef -----------------------------------------------------------*/

struct bitStreamInfo
{
	/* Bit stream : 12-bit words, 1 byte at SYNC_SIGNAL every X bytes for synchronization
	 (X value doesn't matter actually) */
	uint8_t * stream; // Array to be at least 1 + ceil(WORD_LENGTH/8) bytes long
	uint8_t synchronized;
	uint8_t bitCursor;
	uint16_t length;
	uint16_t lastByteReceived;
	uint16_t byteCursor;
	uint64_t * lastValue;
};

/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct bitStreamInfo bitStream;

/* Private function prototypes -----------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


HAL_StatusTypeDef decoder_streamInit(uint8_t * stream, uint16_t length, uint64_t * value) {
	if (length < 1 + WORD_LENGTH/8) {
		return HAL_ERROR;
	}

	bitStream.stream = stream;
	bitStream.length = length;
	bitStream.bitCursor = 0;
	bitStream.byteCursor = 0;
	bitStream.lastByteReceived = length-1;
	bitStream.lastValue = value;
	bitStream.synchronized = 0;

	return HAL_OK;
}

HAL_StatusTypeDef decoder_streamUpdate(uint16_t lastByteReceived) {

	uint8_t bit = 0; // For for loop
	uint8_t bitCursor = bitStream.bitCursor;
	uint16_t byteCursor = bitStream.byteCursor;
	uint64_t value = 0;

	bitStream.lastByteReceived = lastByteReceived;

	// Try to synchronize...
	if ((bitStream.stream)[bitStream.lastByteReceived] == SYNC_SIGNAL) {
		bitStream.synchronized = 1;
		bitStream.bitCursor = 0;
		bitStream.byteCursor = bitStream.lastByteReceived + 1;
		return HAL_OK;
	}

	if (bitStream.synchronized == 0) {
		return HAL_ERROR;
	}

	// Test if we have a new entire word (several conditions because of the array's limit)
	if (byteCursor * 8 + bitCursor + WORD_LENGTH > bitStream.length * 8) {
		if (byteCursor * 8 + bitCursor + WORD_LENGTH > (lastByteReceived + bitStream.length + 1) * 8) {
			return HAL_OK;
		}
	}
	else {
		if (byteCursor * 8 + bitCursor + WORD_LENGTH > (lastByteReceived + 1) * 8) {
			return HAL_OK;
		}
	}

	for (bit = 0; bit < WORD_LENGTH; bit++) {
		value += (((bitStream.stream)[byteCursor] & (0x80 >> bitCursor)) >> (7-bitCursor)) << (WORD_LENGTH-1-bit);
		bitCursor += 1;
		if (bitCursor == 8) {
			bitCursor = 0;
			byteCursor += 1;
		}
		if (byteCursor == bitStream.length) {
			byteCursor = 0;
		}
	}

	bitStream.bitCursor = bitCursor;
	bitStream.byteCursor = byteCursor;
	*(bitStream.lastValue) = value;
	DecodeFinishedHandle();

	return HAL_OK;
}


