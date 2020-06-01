/**
  ******************************************************************************
  * @file           : timer.h
  * @brief          : Header for timer.c file.
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

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef Timer_Start(TIM_HandleTypeDef * htim);
HAL_StatusTypeDef Timer_Stop(TIM_HandleTypeDef * htim);

#ifdef __cplusplus
}
#endif

#endif /* INC_TIMER_H_ */
