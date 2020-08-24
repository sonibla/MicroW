/**
  ******************************************************************************
  * @file           : links.h
  * @brief          : Header for links.c file.
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

#ifndef INC_LINKS_H_
#define INC_LINKS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"
#include "types.h"

/* Exported functions prototypes ---------------------------------------------*/

/*=============================================================================
                  ##### HAL Callback functions #####
=============================================================================*/

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef * hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef * hadc);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef * huart);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef * huart);
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart);

/*=============================================================================
                      ##### Event functions #####
=============================================================================*/

void ADC_FinishedHandle();
void Timer_RisingEdgeHandle();
void encode_FinishedHandle();
void UARTRx_FinishedHandle();

/*=============================================================================
                    ##### Main API functions #####
=============================================================================*/

HAL_StatusTypeDef emitter_start(UART_HandleTypeDef * huart, ADC_HandleTypeDef * hadc, TIM_HandleTypeDef * htim);
HAL_StatusTypeDef emitter_stop();

HAL_StatusTypeDef receiver_start(UART_HandleTypeDef * huart, DAC_HandleTypeDef * hdac, uint32_t DAC_Channel, TIM_HandleTypeDef * htim);
HAL_StatusTypeDef receiver_stop();

#ifdef __cplusplus
}
#endif

#endif /* INC_LINKS_H_ */
