/**
  ******************************************************************************
  * @file           : timer.c
  * @brief          : Timer API
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

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "config.h"

/* Exported functions --------------------------------------------------------*/

/**
 * @brief enables the counter and interruptions of provided timer
 * 
 * @param htim[IN] pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef Timer_Start(TIM_HandleTypeDef * htim) {
	return HAL_TIM_Base_Start_IT(htim);
}

/**
 * @brief disables the counter and interruptions of provided timer
 * 
 * @param htim[IN] pointer to a TIM_HandleTypeDef structure that contains the configuration information for TIM module.
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef Timer_Stop(TIM_HandleTypeDef * htim) {
	return HAL_TIM_Base_Stop_IT(htim);
}
