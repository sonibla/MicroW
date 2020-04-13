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

struct peripherals_Info
{
	UART_HandleTypeDef * huart;
	DAC_HandleTypeDef * hdac;
	ADC_HandleTypeDef * hadc;
	TIM_HandleTypeDef * htim;
	uint32_t DAC_Channel;
};

struct peripherals_Info peripherals;

struct sampleStream_Info sampleStream;
struct bitStream_Info bitStream;

/* Private function prototypes -----------------------------------------------*/

/*
 * Error_Handler is called whenever an unexpected error happen
 */
static void Error_Handler(void);

/*
 * receiver_restart starts the receiver without changing the parameters
 */
static HAL_StatusTypeDef receiver_restart();

/*
 * emitter_restart starts the receiver without changing the parameters
 */
static HAL_StatusTypeDef emitter_restart();

/* Exported functions --------------------------------------------------------*/

/*=============================================================================
                    ##### Main API functions #####
=============================================================================*/

HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim) {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)
	peripherals.huart = huart;
	peripherals.hadc = hadc;
	peripherals.htim = htim;

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
	if (ERROR_LED && status == HAL_OK) {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
	}

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

	status = streamFree(&sampleStream, &bitStream);
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
	peripherals.huart = huart;
	peripherals.hdac = hdac;
	peripherals.DAC_Channel = DAC_Channel;
	peripherals.htim = htim;

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
	if (ERROR_LED && status == HAL_OK) {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
	}

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

	status = streamFree(&sampleStream, &bitStream);
	if (status != HAL_OK) {
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	return status;
}


/*=============================================================================
                  ##### Restart functions #####
=============================================================================*/

static HAL_StatusTypeDef emitter_restart() {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)

	status = emitter_stop();
	if (status != HAL_OK) {
		return status;
	}

	status = emitter_start(peripherals.huart, peripherals.hadc, peripherals.htim);
#else
	status = HAL_ERROR;
#endif
	return status;
}

static HAL_StatusTypeDef receiver_restart() {
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_RECEIVER)

	status = receiver_stop();
	if (status != HAL_OK) {
		return status;
	}

	status = receiver_start(peripherals.huart, peripherals.hdac, peripherals.DAC_Channel, peripherals.htim);
#else
	status = HAL_ERROR;
#endif
	return status;
}

/*=============================================================================
                  ##### HAL Callback functions #####
=============================================================================*/

// ADC

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc) {
	HAL_StatusTypeDef status = HAL_OK;

	status = ADC_streamUpdate();

	if (status != HAL_OK) {
		Error_Handler();
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc) {
	Error_Handler();
}

// UART

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart) {
	HAL_StatusTypeDef status = HAL_OK;

	status = UARTRx_streamUpdate();

	if (status != HAL_OK) {
		Error_Handler();
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart) {
	HAL_StatusTypeDef status = HAL_OK;

	status = UARTTx_streamRestart();

	if (status != HAL_OK) {
		Error_Handler();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart) {
	Error_Handler();
}

/*=============================================================================
                      ##### "Handle" functions #####
=============================================================================*/

static void Error_Handler() {
	HAL_StatusTypeDef status = HAL_OK;

	if (ERROR_LED) {
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
	}

	if (ERROR_DELAY) {
		HAL_Delay(ERROR_DELAY);
	}

	if (ERROR_HANDLING == STOP || ERROR_HANDLING == RESTART) {
		receiver_stop();
		emitter_stop();
	}

	if (ERROR_HANDLING == RESTART) {
		if (MODULE_TYPE == MICROW_RECEIVER) {
			status = receiver_restart();
		}
		else {
			status = emitter_restart();
		}
		if (status != HAL_OK) {
			/*
			 * Oh no ! Error_Handler has encountered an error !
			 * No problem, let's call Error_Handler for help
			 */
			Error_Handler();
		}
	}

	if (ERROR_HANDLING == INFINITE_LOOP) {
		while (1) {

		}
	}
}

void Timer_RisingEdgeHandle() {
	HAL_StatusTypeDef status = HAL_OK;

	if (sampleStream.state == ACTIVE) {
		if (MODULE_TYPE == MICROW_EMITTER) {
			status = ADC_streamRestart();
		}
		else {
			status = DAC_streamUpdate();
		}
	}

	if (status != HAL_OK) {
		Error_Handler();
	}
}

void ADC_FinishedHandle() {
	HAL_StatusTypeDef status = HAL_OK;

	status = encoder_streamUpdate();

	if (status != HAL_OK) {
		Error_Handler();
	}
}

void encode_FinishedHandle() {
	HAL_StatusTypeDef status = HAL_OK;

	status = UARTTx_streamUpdate();

	if (status != HAL_OK && status != HAL_BUSY) {
		Error_Handler();
	}
}

void UARTRx_FinishedHandle() {
	HAL_StatusTypeDef status = HAL_OK;

	status = decoder_streamUpdate();

	if (status != HAL_OK) {
		Error_Handler();
	}
}
