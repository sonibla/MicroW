/**
  ******************************************************************************
  * @file           : adc.h
  * @brief          : Header for adc.c file.
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

#ifndef INC_ADC_H_
#define INC_ADC_H_

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
 * Known issue: functions in this file only handle one ADC.
 * This is not a real issue since MicroW doesn't need to record stereo sound.
 */


/*
 * ADC_streamStart creates an ADC stream according to provided sampleStream structure.
 * The uint32_t buffer inside sampleStream will fill automatically
 */
HAL_StatusTypeDef ADC_streamStart(struct sampleStream_Info * sampleStream);

/*
 * ADC_streamRestart starts the ADC without overwriting existing parameters.
 */
HAL_StatusTypeDef ADC_streamRestart();

/*
 * ADC_streamUpdate should be called at the end of a conversion to update the buffer
 */
HAL_StatusTypeDef ADC_streamUpdate();

/*
 * ADC_streamStop stops a running stream.
 */
HAL_StatusTypeDef ADC_streamStop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_H_ */
