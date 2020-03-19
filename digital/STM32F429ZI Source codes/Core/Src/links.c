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

struct sampleStream_Info sampleStream;
struct bitStream_Info bitStream;

/* Private function prototypes -----------------------------------------------*/

/*
 * Error_Handler is called whenever an unexpected error happen
 */
static void Error_Handler(void);

/* Exported functions --------------------------------------------------------*/


/*=============================================================================
                  ##### HAL Callback functions #####
=============================================================================*/

// ADC

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc) {
	ADC_streamUpdate();
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc) {
	Error_Handler();
}

// UART

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {
	UARTRx_streamUpdate();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart) {
	UARTTx_streamRetart();
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart) {
	Error_Handler();
}

/*=============================================================================
                      ##### "Handle" functions #####
=============================================================================*/

static void Error_Handler(void) {

}

void Timer_RisingEdgeHandle() {
	if (sampleStream.state == ACTIVE) {
		if (MODULE_TYPE == MICROW_EMITTER) {
			ADC_streamRestart();
		}
		else {
			DAC_streamUpdate();
		}
	}
}

void ADC_FinishedHandle() {
	encoder_streamUpdate();
}

void encode_FinishedHandle() {
	UARTTx_streamUpdate();
}

void UARTRx_FinishedHandle() {
	decoder_streamUpdate();
}

/*=============================================================================
                    ##### Main API functions #####
=============================================================================*/

HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim) {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)
	status = streamInit(&sampleStream, &bitStream, hadc, huart);
	if (status != HAL_OK) {
		return status;
	}

	status = UARTTx_streamStart(&bitStream);
	if (status != HAL_OK) {
		return status;
	}

	status = encoder_streamStart(&sampleStream, &bitStream);
	if (status != HAL_OK) {
		return status;
	}

	status = ADC_streamStart(&sampleStream);
	if (status != HAL_OK) {
		return status;
	}

	status = Timer_Start(htim);
	if (status != HAL_OK) {
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	return status;
}

HAL_StatusTypeDef emitter_stop() {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)
	status = ADC_streamStop();
	if (status != HAL_OK) {
		return status;
	}

	status = encoder_streamStop();
	if (status != HAL_OK) {
		return status;
	}

	status = UARTTx_streamStop();
	if (status != HAL_OK) {
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	return status;
}

HAL_StatusTypeDef receiver_start(UART_HandleTypeDef * huart, DAC_HandleTypeDef * hdac, uint32_t DAC_Channel, TIM_HandleTypeDef * htim) {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_RECEIVER)
	status = streamInit(&sampleStream, &bitStream, hdac, DAC_Channel, huart);
	if (status != HAL_OK) {
		return status;
	}

	status = Timer_Start(htim);
	if (status != HAL_OK) {
		return status;
	}

	status = DAC_streamStart(&sampleStream);
	if (status != HAL_OK) {
		return status;
	}

	status = decoder_streamStart(&bitStream, &sampleStream);
	if (status != HAL_OK) {
		return status;
	}

	status = UARTRx_streamStart(&bitStream);
	if (status != HAL_OK) {
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	return status;
}

HAL_StatusTypeDef receiver_stop() {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_RECEIVER)
	status = UARTRx_streamStop();
	if (status != HAL_OK) {
		return status;
	}

	status = decoder_streamStop();
	if (status != HAL_OK) {
		return status;
	}

	status = DAC_streamStop();
	if (status != HAL_OK) {
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	return status;
}
