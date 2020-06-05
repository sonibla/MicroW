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

/* Exported functions prototypes ---------------------------------------------*/

/*
 * Known issue: functions in this file only handle one ADC.
 * This is not a real issue since MicroW doesn't need to record stereo sound.
 */

HAL_StatusTypeDef ADC_streamStart(struct sampleStream_Info * sampleStream);
HAL_StatusTypeDef ADC_streamRestart();
HAL_StatusTypeDef ADC_streamUpdate();
HAL_StatusTypeDef ADC_streamStop();

#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_H_ */
