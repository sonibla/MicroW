/**
  ******************************************************************************
  * @file           : dac.c
  * @brief          : Digital to Analog Converter API
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

/* Private typedef -----------------------------------------------------------*/

struct DAC_Config
{
	DAC_HandleTypeDef * hdac;
	uint32_t Channel;
};

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
struct DAC_Config config;

/* Private function prototypes -----------------------------------------------*/
/* Exported functions --------------------------------------------------------*/

HAL_StatusTypeDef DAC_streamInit(DAC_HandleTypeDef * hdac, uint32_t Channel) {
	config.hdac = hdac;
	config.Channel = Channel;
	return HAL_DAC_Start(hdac, Channel);
}

HAL_StatusTypeDef DAC_streamUpdate(uint64_t value) {
	return HAL_DAC_SetValue(config.hdac, config.Channel, DAC_ALIGN_12B_R, (uint32_t)value);
}

void DAC_streamStop() {
	HAL_DAC_Stop(config.hdac, config.Channel);
}
