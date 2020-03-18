/**
  ******************************************************************************
  * @file           : links.c
  * @brief          : Main API, manages links between lower API of MicroW
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
#include "decoder.h"
#include "encoder.h"
#include "uart.h"
#include "config.h"
#include "adc.h"
#include "dac.h"
#include "types.h"
#include "timer.h"

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint64_t value;
uint8_t RxBuffer[RX_BUFFER_SIZE] = {0};
struct sampleStream_Info sampleStream;
struct bitStream_Info bitStream;

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/


/*=============================================================================
                  ##### HAL Callback functions #####
=============================================================================*/

// ADC

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc) {
	ADC_streamUpdate();
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc) {
	ADC_streamUpdate();
}

// UART

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {
	UART_RxStreamHandle(huart);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart) {
	UARTTx_streamRetart();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart) {
	UART_RxStreamHandle(huart);
}

/*=============================================================================
                      ##### "Handle" functions #####
=============================================================================*/

void Timer_RisingEdgeHandle() {
	if (sampleStream.state == ACTIVE) {
		if (MODULE_TYPE == MICROW_EMITTER) {
			ADC_streamRestart();
		}
		else {
			DAC_streamUpdate(value);
		}
	}
}

void ADC_FinishedHandle() {
	encoder_streamUpdate();
}

void encode_FinishedHandle() {
	UARTTx_streamUpdate();
}

void RxFinishedHandle(uint16_t lastByteReceived) {
	decoder_streamUpdate(lastByteReceived);
}

void DecodeFinishedHandle() {
	DAC_streamUpdate(value);
}

/*=============================================================================
                    ##### Main API functions #####
=============================================================================*/

HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim) {
#if (MODULE_TYPE == MICROW_EMITTER)
	streamInit(&sampleStream, &bitStream, hadc, huart);

	UARTTx_streamStart(&bitStream);

	encoder_streamStart(&sampleStream, &bitStream);

	ADC_streamStart(&sampleStream);

	Timer_Start(htim);

	return HAL_OK;

#else
	return HAL_ERROR;
#endif
}

void emitter_stop() {
	ADC_streamStop();

	encoder_streamStop();

	UARTTx_streamStop();
}

HAL_StatusTypeDef receiver_start(UART_HandleTypeDef * huart, DAC_HandleTypeDef * hdac, uint32_t DAC_Channel) {

	HAL_StatusTypeDef status;

	status = DAC_streamInit(hdac, DAC_Channel);
	if (status != HAL_OK) {
		return status;
	}

	status = decoder_streamInit(RxBuffer, RX_BUFFER_SIZE, &value);
	if (status != HAL_OK) {
		return status;
	}

	status = UART_RxStreamInit(huart, RxBuffer, RX_BUFFER_SIZE);
	return status;
}

void receiver_stop() {
	UART_RxStreamStop();

	DAC_streamStop();
}
