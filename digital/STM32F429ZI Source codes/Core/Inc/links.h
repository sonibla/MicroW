/**
  ******************************************************************************
  * @file           : links.h
  * @brief          : Header for links.c file.
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

#ifndef INC_LINKS_H_
#define INC_LINKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "types.h"

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/*=============================================================================
                  ##### HAL Callback functions #####
=============================================================================*/

// ADC
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc);

// UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart);

/*=============================================================================
                      ##### Event functions #####
=============================================================================*/

/*
 * ADC_FinishedHandle will be called by ADC API when a new value has been successfully stored
 */
void ADC_FinishedHandle();

/*
 * Timer_RisingEdgeHandle will be called on every rising edge of the timer
 */
void Timer_RisingEdgeHandle();

/*
 * encode_FinishedHandle will be called by encoder API when the UART buffer out has been updated
 */
void encode_FinishedHandle();

/*
 * UARTRx_FinishedHandle will be called by UART API when the UART buffer in has been updated
 */
void UARTRx_FinishedHandle();

/*=============================================================================
                    ##### Main API functions #####
=============================================================================*/

/*
 * emitter_start does everything necessary to automatically receive analog values and send them via serial.
 * Non blocking function
 */
HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim);
void emitter_stop();

/*
 * receiver_start does everything necessary to automatically receive a serial stream and convert received values
 * into an analog signal.
 * Non blocking function
 */
HAL_StatusTypeDef receiver_start(UART_HandleTypeDef * huart, DAC_HandleTypeDef * hdac, uint32_t DAC_Channel, TIM_HandleTypeDef * htim);
void receiver_stop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_LINKS_H_ */
