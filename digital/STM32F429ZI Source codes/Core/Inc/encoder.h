/**
  ******************************************************************************
  * @file           : encoder.h
  * @brief          : Header for encoder.c file.
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

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

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

/*
 * encoder_streamStart initializes a stream (an auto-completing bitStream according to a sampleStream)
 */
HAL_StatusTypeDef encoder_streamStart(struct sampleStream_Info * sampleStream, struct bitStream_Info * bitStream);

/*
 * encoder_streamRestart starts a stream without overwriting existing parameters.
 */
HAL_StatusTypeDef encoder_streamRestart();

/*
 * encoder_streamUpdate should be called at the end of a ADC buffer update to update the UART buffer
 */
HAL_StatusTypeDef encoder_streamUpdate();

/*
 * encoder_streamStop stops a running stream.
 */
HAL_StatusTypeDef encoder_streamStop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_ENCODER_H_ */
