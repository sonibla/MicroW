/**
  ******************************************************************************
  * @file           : uart.h
  * @brief          : Header for uart.c file.
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

#ifndef INC_UART_H_
#define INC_UART_H_

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
                  ##### Transmit functions #####
=============================================================================*/

/*
 * UARTTx_streamStart initializes a stream to continuously send data
 */
HAL_StatusTypeDef UARTTx_streamStart(struct bitStream_Info * bitStream);

/*
 * UARTTx_streamRestart starts a stream without overwriting existing parameters.
 */
HAL_StatusTypeDef UARTTx_streamRetart();

/*
 * UARTTx_streamUpdate should be called when the UART buffer has been successfully updated
 */
HAL_StatusTypeDef UARTTx_streamUpdate();

/*
 * UARTTx_streamStop stops a running stream.
 */
HAL_StatusTypeDef UARTTx_streamStop();

/*=============================================================================
                  ##### Receive functions #####
=============================================================================*/

/*
 * UARTRx_streamStart initializes a stream to continuously receive data
 */
HAL_StatusTypeDef UARTRx_streamStart(struct bitStream_Info * bitStream);

/*
 * UARTRx_streamRestart starts a stream without overwriting existing parameters.
 */
HAL_StatusTypeDef UARTRx_streamRetart();

/*
 * UARTRx_streamUpdate should be called at the end of data reception
 */
HAL_StatusTypeDef UARTRx_streamUpdate();

/*
 * UARTRx_streamStop stops a running stream.
 */
HAL_StatusTypeDef UARTRx_streamStop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_UART_H_ */
