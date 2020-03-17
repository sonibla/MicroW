/*
 * timer.h
 *
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

/*
 * Timer_Start enables the counter and interruptions of provided timer
 */
HAL_StatusTypeDef Timer_Start(TIM_HandleTypeDef * htim);

/*
 * Timer_Stop disables the counter and interruptions of provided timer
 */
HAL_StatusTypeDef Timer_Stop(TIM_HandleTypeDef * htim);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_TIMER_H_ */
