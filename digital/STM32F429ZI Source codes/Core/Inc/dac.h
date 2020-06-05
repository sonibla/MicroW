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

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef DAC_streamStart(struct sampleStream_Info * sampleStream);
HAL_StatusTypeDef DAC_streamRestart();
HAL_StatusTypeDef DAC_streamUpdate();
HAL_StatusTypeDef DAC_streamStop();

#ifdef __cplusplus
}
#endif

#endif /* INC_DAC_H_ */
