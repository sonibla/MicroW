/**
  ******************************************************************************
  * @file           : decoder.h
  * @brief          : Header for decoder.c file.
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

/*
 * decoder_streamStart initializes a stream to continuously decode data
 */
HAL_StatusTypeDef decoder_streamStart(struct bitStream_Info * bitStream, struct sampleStream_Info * sampleStream);

/*
 * decoder_streamRestart starts a stream without overwriting existing parameters.
 */
HAL_StatusTypeDef decoder_streamRestart();

/*
 * decoder_streamUpdate should be called at the end of new data saving
 */
HAL_StatusTypeDef decoder_streamUpdate();

/*
 * decoder_streamStop stops a running stream.
 */
HAL_StatusTypeDef decoder_streamStop();

/* Private defines -----------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* INC_DECODER_H_ */
