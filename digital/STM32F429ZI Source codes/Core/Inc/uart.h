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

#include "stm32f4xx_hal.h"
#include "types.h"

/*=============================================================================
                  ##### Transmit functions #####
=============================================================================*/

HAL_StatusTypeDef UARTTx_streamStart(struct bitStream_Info * bitStream);
HAL_StatusTypeDef UARTTx_streamRestart();
HAL_StatusTypeDef UARTTx_streamUpdate();
HAL_StatusTypeDef UARTTx_streamStop();

/*=============================================================================
                  ##### Receive functions #####
=============================================================================*/

HAL_StatusTypeDef UARTRx_streamStart(struct bitStream_Info * bitStream);
HAL_StatusTypeDef UARTRx_streamRestart();
HAL_StatusTypeDef UARTRx_streamUpdate();
HAL_StatusTypeDef UARTRx_streamStop();

#ifdef __cplusplus
}
#endif

#endif /* INC_UART_H_ */
