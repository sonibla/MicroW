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

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct sampleStream_Info * ADC_stream;
struct bitStream_Info * UART_stream;
uint16_t maskSample;

/* Private function prototypes -----------------------------------------------*/

/*
 * mask creates a number in which n LSBs are ones
 */
static uint64_t mask(uint8_t bits);

/*
 * sendTrueByte saves a byte into the UART buffer without modifying data
 */
static void sendTrueByte(uint8_t byte);

/*
 * sendByte saves a byte into the UART buffer, but toggles the LSB if byte == SYNC_SIGNAL
 * LSB between 0 and 7.
 */
static void sendByte(uint8_t byte, uint8_t LSB);

/*
 * getSample returns the value of the sample to encode
 */
static uint64_t getSample();

/*
 * nextSample changes ADC_stream metadata to consider a new sample (the next one)
 */
static void nextSample();

/*
 * sampleAvailable checks if sample is (partially) available
 */
static uint8_t sampleAvailable();

/*
 * sendSyncSignal sends a synchronization byte instead of real data
 */
static void sendSyncSignal();

/* Exported functions --------------------------------------------------------*/

HAL_StatusTypeDef encoder_streamStart(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream){
	ADC_stream = sampleStream;
	UART_stream = bitStream;

	maskSample = mask(WORD_LENGTH);

	ADC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;

	sendSyncSignal();

	return HAL_OK;
}

HAL_StatusTypeDef encoder_streamRetart(){
	ADC_stream->state = ACTIVE;
	UART_stream->state = ACTIVE;

	sendSyncSignal();

	return HAL_OK;
}

HAL_StatusTypeDef encoder_streamUpdate(){
	uint8_t bit;
	uint8_t bit_value;
	uint8_t byte;
	uint8_t LSB;
	uint64_t sample;

	if (ADC_stream->state == INACTIVE || UART_stream->state == INACTIVE) {
		return HAL_BUSY;
	}


	while(sampleAvailable()) {

		sample = getSample();

		while (WORD_LENGTH - ADC_stream->bitsOut >= sizeof(uint8_t)) {
			// We can fill a full uint8_t with the last new sample
			byte = 0;

			for(bit = 0; bit < sizeof(uint8_t); bit++) {
				bit_value = (sample & (0x01 << (WORD_LENGTH - 1 - ADC_stream->bitsOut))) >> (WORD_LENGTH - 1 - ADC_stream->bitsOut);
				byte += bit_value << (sizeof(uint8_t) - bit);
				ADC_stream->bitsOut += 1;
			}

			// LSB = 7 because considered byte is fully inside a sample
			LSB = sizeof(uint8_t) - 1;
			sendByte(byte, LSB);
		}

		// Check if the above process encoded an entire sample
		if (WORD_LENGTH <= ADC_stream->bitsOut) {
			nextSample();
			if (UART_stream->bytesSinceLastSyncSignal > SYNC_PERIOD) {
				sendSyncSignal();
			}
		}
		// Check if we can complete a byte using next sample
		else if (sampleAvailable()) {
			// Start to fill the byte with the lasts bits of selected sample
			byte = 0;
			bit = 0;

			while(WORD_LENGTH > ADC_stream->bitsOut) {
				bit_value = (sample & (0x01 << (WORD_LENGTH - 1 - ADC_stream->bitsOut))) >> (WORD_LENGTH - 1 - ADC_stream->bitsOut);
				byte += bit_value << (sizeof(uint8_t) - bit);
				ADC_stream->bitsOut += 1;
				bit += 1;
			}

			// Now we continue the same process with next sample
			LSB = bit;
			nextSample();
			sample = getSample();

			while(bit < sizeof(uint8_t)) {
				bit_value = (sample & (0x01 << (WORD_LENGTH - 1 - ADC_stream->bitsOut))) >> (WORD_LENGTH - 1 - ADC_stream->bitsOut);
				byte += bit_value << (sizeof(uint8_t) - bit);
				ADC_stream->bitsOut += 1;
				bit += 1;
			}

			sendByte(byte, LSB);
		}
	}

	encode_FinishedHandle();
	return HAL_OK;
}

static uint8_t sampleAvailable() {
	// Check if an entire new sample is available
	if (ADC_stream->lastSampleIn < ADC_stream->lastSampleOut) {
		if (ADC_stream->lastSampleIn + ADC_stream->length > ADC_stream->lastSampleOut) {
			return 1;
		}
	}
	else {
		if (ADC_stream->lastSampleIn > ADC_stream->lastSampleOut) {
			return 1;
		}
	}
	if (WORD_LENGTH > ADC_stream->bitsOut) {
		return 1;
	}
	return 0;
}

static void nextSample() {
	ADC_stream->lastSampleOut += 1;
	if (ADC_stream->lastSampleOut >= ADC_stream->length) {
		ADC_stream->lastSampleOut = 0;
	}
	ADC_stream->bitsOut = 0;
}

static uint64_t getSample() {
	if (ADC_stream->lastSampleOut + 1 < ADC_stream->length) {
		return ((ADC_stream->stream)[1 + ADC_stream->lastSampleOut]) & maskSample;
	}
	else {
		return ((ADC_stream->stream)[1 + ADC_stream->lastSampleOut - ADC_stream->length]) & maskSample;
	}
}

static void sendTrueByte(uint8_t byte) {
	UART_stream->lastByteIn += 1;
	if (UART_stream->lastByteIn >= UART_stream->length) {
		UART_stream->lastByteIn = 0;
	}
	UART_stream->stream[UART_stream->lastByteIn] = byte;
	UART_stream->bytesSinceLastSyncSignal += 1;
}

static void sendByte(uint8_t byte, uint8_t LSB) {
	uint8_t mask;

	if (LSB >= sizeof(uint8_t)) {
		// Default value in case of error:
		LSB = sizeof(uint8_t) - 1;
	}

	mask = 0x80 >> LSB;

	if (byte == SYNC_SIGNAL) {
		if (byte & mask) {
			// byte's LSB is a 1
			byte &= ~mask;
		}
		else {
			// byte's LSB is a 0
			byte |= mask;
		}
	}

	sendTrueByte(byte);
}

static void sendSyncSignal() {
	sendTrueByte(SYNC_SIGNAL);
	if (ADC_stream->bitsOut != 0) {
		nextSample();
	}
	UART_stream->bytesSinceLastSyncSignal = 0;
}

HAL_StatusTypeDef encoder_streamStop(){
	ADC_stream->state = INACTIVE;
	UART_stream->state = INACTIVE;

	return HAL_OK;
}

static uint64_t mask(uint8_t bits) {
	uint8_t bit;
	uint64_t mask_var = 0;

	for (bit = 0; bit < bits; bit++) {
		mask_var <<= 1;
		mask_var += 1;
	}

	return mask_var;
}
