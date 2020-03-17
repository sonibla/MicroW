/*
 * timer.c
 *
 */

/* Includes ------------------------------------------------------------------*/

#include "stm32f4xx_hal.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

HAL_StatusTypeDef Timer_Start(TIM_HandleTypeDef * htim) {
	return HAL_TIM_Base_Start_IT(htim);
}

HAL_StatusTypeDef Timer_Stop(TIM_HandleTypeDef * htim) {
	return HAL_TIM_Base_Stop_IT(htim);
}
