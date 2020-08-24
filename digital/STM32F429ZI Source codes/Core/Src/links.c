/**
  ******************************************************************************
  * @file           : links.c
  * @brief          : Main API, manages links between lower API of MicroW
  * 
  * The goal of this API is to create links between lower lever MicroW APIs :
  * calling the right function at the right time and managing events, for
  * example end of data transfers, errors...
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

static void Error_Handler(void);
static HAL_StatusTypeDef receiver_restart();
static HAL_StatusTypeDef emitter_restart();

/* Exported functions --------------------------------------------------------*/

/*=============================================================================
                    ##### Main API functions #####
=============================================================================*/

/**
 * @brief emitter_start does everything necessary to automatically receive analog values and send them via serial.
 * @param huart[in] pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.
 * @param hadc[in] pointer to a ADC_HandleTypeDef structure that contains the configuration information for the specified ADC.
 * @param htim[in] pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.
 * @return HAL status (HAL_OK if no errors occured).
 * @note Non blocking function
 */
HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim)
{
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)
	peripherals.huart = huart;
	peripherals.hadc = hadc;
	peripherals.htim = htim;

	status = streamInit(&sampleStream, &bitStream, hadc, huart);
	if (status != HAL_OK)
	{
		return status;
	}

	status = UARTTx_streamStart(&bitStream);
	if (status != HAL_OK)
	{
		return status;
	}

	status = encoder_streamStart(&sampleStream, &bitStream);
	if (status != HAL_OK)
	{
		return status;
	}

	status = ADC_streamStart(&sampleStream);
	if (status != HAL_OK)
	{
		return status;
	}

	status = Timer_Start(htim);
	if (status != HAL_OK)
	{
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	if (ERROR_LED && (status == HAL_OK))
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
	}

	return status;
}

/**
 * @brief emitter_stop does everything necessary to stop the emitter
 * @return HAL status (HAL_OK if no errors occured).
 * @note Non blocking function
 */
HAL_StatusTypeDef emitter_stop()
{
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)
	status = ADC_streamStop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = encoder_streamStop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = UARTTx_streamStop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = streamFree(&sampleStream, &bitStream);
	if (status != HAL_OK)
	{
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	return status;
}


/**
 * @brief receiver_start does everything necessary to automatically receive a serial stream and convert received values into an analog signal.
 * @param huart[in] pointer to a USART_HandleTypeDef structure that contains the configuration information for the specified USART module.
 * @param hdac[in] pointer to a DAC_HandleTypeDef structure that contains the configuration information for the specified DAC.
 * @param DAC_Channel[in] The selected DAC channel. This parameter can be one of the following values: DAC_CHANNEL_1 or DAC_CHANNEL_2
 * @param htim[in] pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.
 * @return HAL status (HAL_OK if no errors occured).
 * @note Non blocking function
 */
HAL_StatusTypeDef receiver_start(UART_HandleTypeDef * huart, DAC_HandleTypeDef * hdac, uint32_t DAC_Channel, TIM_HandleTypeDef * htim)
{
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_RECEIVER)
	peripherals.huart = huart;
	peripherals.hdac = hdac;
	peripherals.DAC_Channel = DAC_Channel;
	peripherals.htim = htim;

	status = streamInit(&sampleStream, &bitStream, hdac, DAC_Channel, huart);
	if (status != HAL_OK)
	{
		return status;
	}

	status = Timer_Start(htim);
	if (status != HAL_OK)
	{
		return status;
	}

	status = DAC_streamStart(&sampleStream);
	if (status != HAL_OK)
	{
		return status;
	}

	status = decoder_streamStart(&bitStream, &sampleStream);
	if (status != HAL_OK)
	{
		return status;
	}

	status = UARTRx_streamStart(&bitStream);
	if (status != HAL_OK)
	{
		return status;
	}
#else
	status = HAL_ERROR;
#endif
	if (ERROR_LED && (status == HAL_OK))
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_RESET);
	}

	return status;
}

/**
 * @brief receiver_stop does everything necessary to stop the receiver
 * @return HAL status (HAL_OK if no errors occured).
 * @note Non blocking function
 */
HAL_StatusTypeDef receiver_stop()
{
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_RECEIVER)
	status = UARTRx_streamStop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = decoder_streamStop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = DAC_streamStop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = streamFree(&sampleStream, &bitStream);
	if (status != HAL_OK)
	{
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

/**
 * @brief emitter_restart starts the emitter without changing existing configuration
 * @return HAL status (HAL_OK if no errors occured).
 * @note Non blocking function
 */
static HAL_StatusTypeDef emitter_restart()
{
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_EMITTER)

	status = emitter_stop();
	if (status != HAL_OK)
	{
		return status;
	}

	status = emitter_start(peripherals.huart, peripherals.hadc, peripherals.htim);
#else
	status = HAL_ERROR;
#endif
	return status;
}

/**
 * @brief receiver_restart starts the receiver without changing existing configuration
 * @return HAL status (HAL_OK if no errors occured).
 * @note Non blocking function
 */
static HAL_StatusTypeDef receiver_restart()
{
	HAL_StatusTypeDef status = HAL_OK;
#if (MODULE_TYPE == MICROW_RECEIVER)

	status = receiver_stop();
	if (status != HAL_OK)
	{
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

/*
 * Please refer to HAL documentation for the detailed manual of those functions.
 * Those functions are called by HAL when a specific event happens.
 */
 
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = ADC_streamUpdate();

	if (status != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc)
{
	Error_Handler();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = UARTRx_streamUpdate();

	if (status != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart)
{
	HAL_StatusTypeDef status = HAL_OK;

	status = UARTTx_streamRestart();

	if (status != HAL_OK)
	{
		Error_Handler();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart)
{
	Error_Handler();
}

/*=============================================================================
                      ##### "Handle" functions #####
=============================================================================*/

/**
 * @brief Error_Handler is called whenever an unexpected error happen
 * @note Its behaviour if configurable thants to defines in config.h
 */
static void Error_Handler()
{
	HAL_StatusTypeDef status = HAL_OK;

	if (ERROR_LED)
	{
		HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, GPIO_PIN_SET);
	}

	if (ERROR_DELAY)
	{
		HAL_Delay(ERROR_DELAY);
	}

	if ((ERROR_HANDLING == STOP) || (ERROR_HANDLING == RESTART))
	{
		receiver_stop();
		emitter_stop();
	}

	if (ERROR_HANDLING == RESTART)
	{
		if (MODULE_TYPE == MICROW_RECEIVER)
		{
			status = receiver_restart();
		}
		else
		{
			status = emitter_restart();
		}
		if (status != HAL_OK)
		{
			/*
			 * Oh no ! Error_Handler has encountered an error !
			 * No problem, let's call Error_Handler for help
			 */
			Error_Handler();
		}
	}

	if (ERROR_HANDLING == INFINITE_LOOP)
	{
		while (1)
		{

		}
	}
}

/**
 * @brief Timer_RisingEdgeHandle will be called by low-level MicroW APIs on every rising edge of the timer
 */
void Timer_RisingEdgeHandle()
{
	HAL_StatusTypeDef status = HAL_OK;

	if (sampleStream.state == ACTIVE)
	{
		if (MODULE_TYPE == MICROW_EMITTER)
		{
			status = ADC_streamRestart();
		}
		else
		{
			status = DAC_streamUpdate();
		}
	}

	if (status != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief ADC_FinishedHandle will be called by ADC API when a new value has been successfully stored
 */
void ADC_FinishedHandle()
{
	HAL_StatusTypeDef status = HAL_OK;

	status = encoder_streamUpdate();

	if (status != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief encode_FinishedHandle will be called by encoder API when the UART buffer out has been updated
 */
void encode_FinishedHandle()
{
	HAL_StatusTypeDef status = HAL_OK;

	status = UARTTx_streamUpdate();

	if ((status != HAL_OK) && (status != HAL_BUSY))
	{
		Error_Handler();
	}
}

/**
 * @brief UARTRx_FinishedHandle will be called by UART API when the UART buffer in has been updated
 */
void UARTRx_FinishedHandle()
{
	HAL_StatusTypeDef status = HAL_OK;

	status = decoder_streamUpdate();

	if (status != HAL_OK)
	{
		Error_Handler();
	}
}
