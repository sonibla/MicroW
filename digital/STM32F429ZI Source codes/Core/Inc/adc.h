/*
 * adc.h
 *
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
