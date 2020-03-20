/**
  ******************************************************************************
  * @file           : dac.h
  * @brief          : Header for dac.c file.
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

#ifndef INC_DAC_H_
#define INC_DAC_H_

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
 * DAC_streamStart initializes a stream to continuously decode data
 */
HAL_StatusTypeDef DAC_streamStart(struct sampleStream_Info * sampleStream);

/*
 * DAC_streamRestart starts a stream without overwriting existing parameters.
 */
HAL_StatusTypeDef DAC_streamRestart();

/*
 * DAC_streamUpdate should be called at the end of new data saving
 */
HAL_StatusTypeDef DAC_streamUpdate();

/*
 * DAC_streamStop stops a running stream.
 */
HAL_StatusTypeDef DAC_streamStop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_DAC_H_ */
