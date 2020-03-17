/*
 * decoder.h
 *
 */

#ifndef INC_DECODER_H_
#define INC_DECODER_H_

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
HAL_StatusTypeDef decoder_streamUpdate(uint16_t lastByteReceived);
HAL_StatusTypeDef decoder_streamInit(uint8_t * stream, uint16_t length, uint64_t * value);

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_DECODER_H_ */
