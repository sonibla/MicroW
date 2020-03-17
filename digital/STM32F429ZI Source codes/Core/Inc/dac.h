/*
 * dac.h
 *
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
HAL_StatusTypeDef DAC_streamInit(DAC_HandleTypeDef * hdac, uint32_t Channel);
HAL_StatusTypeDef DAC_streamUpdate(uint64_t value);
void DAC_streamStop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_DAC_H_ */
