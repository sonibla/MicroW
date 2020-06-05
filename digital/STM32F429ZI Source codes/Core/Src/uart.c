/**
  ******************************************************************************
  * @file           : uart.c
  * @brief          : USART API
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
#include "links.h"
#include "config.h"

/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

struct bitStream_Info * UART_stream = NULL;

/* Private function prototypes -----------------------------------------------*/

static uint8_t dataAvailable();
static void saveByte(uint8_t byte);

/* Exported functions --------------------------------------------------------*/

/*=============================================================================
                  ##### Transmit functions #####
=============================================================================*/

/**
 * @brief initializes a stream to continuously send data over UART interface
 * 
 * @param bitStream[in] pointer to an initialized bitStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 * @note This function uses HAL, the UART peripheral must be initialized with HAL
 */
HAL_StatusTypeDef UARTTx_streamStart(struct bitStream_Info * bitStream)
{
	// Save the pointer to the struct in a global variable:
	UART_stream = bitStream;

	UART_stream->state = ACTIVE;
	return UARTTx_streamUpdate();
}

/**
 * @brief starts a stream without overwriting existing parameters.
 * Existing parameters are saved in the UART_stream global variable
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @note this function should be called after data has been sent
 * @warning UARTTx_streamStart() must be called at least once before
 * calling UARTTx_streamRestart()
 */
HAL_StatusTypeDef UARTTx_streamRestart()
{
	/* Check that UART parameters already exists 
	 * (ie UARTTx_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	UART_stream->state = ACTIVE;

	return UARTTx_streamUpdate();
}

/**
 * @brief sends data if necessary and updates the stream structure fields
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @note This function should be called when the UART buffer has been 
 * successfully updated and data is ready to be sent
 */
HAL_StatusTypeDef UARTTx_streamUpdate()
{
	/* Check that UART parameters already exists 
	 * (ie UARTTx_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	if (UART_stream->state != ACTIVE)
	{
		return HAL_BUSY;
	}

	if (dataAvailable())
	{
		UART_stream->state = BUSY;
		
		// Send one byte :
		UART_stream->lastByteOut += 1;
		if (UART_stream->lastByteOut >= UART_stream->length)
		{
			UART_stream->lastByteOut = 0;
		}

		UART_stream->byte = (UART_stream->stream)[UART_stream->lastByteOut];

		return HAL_UART_Transmit_DMA(UART_stream->huart, &(UART_stream->byte), 1);
	}

	return HAL_OK;
}

/**
 * @brief stops a running stream.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef UARTTx_streamStop()
{
	/* Check that UART parameters already exists 
	 * (ie UARTTx_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	// Mark the stream as inative:
	UART_stream->state = INACTIVE;
	return HAL_OK;
}

/*=============================================================================
                  ##### Receive functions #####
=============================================================================*/

/**
 * @brief initializes a stream to continuously receive data with UART interface
 * 
 * @param bitStream[in] pointer to an initialized bitStream_Info structure
 * @return HAL status (HAL_OK if no errors occured).
 * @note This function uses HAL, the UART peripheral must be initialized with HAL
 */
HAL_StatusTypeDef UARTRx_streamStart(struct bitStream_Info * bitStream)
{
	UART_HandleTypeDef * huart;
	UART_stream = bitStream;

	huart = UART_stream->huart;
	if ((*huart).Init.Mode != UART_MODE_TX_RX && (*huart).Init.Mode != UART_MODE_RX)
	{
		return HAL_ERROR;
	}

	UART_stream->state = BUSY;
	return HAL_UART_Receive_DMA(UART_stream->huart, &(UART_stream->byte), 1);
}

/**
 * @brief starts a stream without overwriting existing parameters.
 * Existing parameters are saved in the UART_stream global variable
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @warning UARTTx_streamStart() must be called at least once before to
 * calling UARTTx_streamRestart()
 */
HAL_StatusTypeDef UARTRx_streamRestart()
{
	/* Check that UART parameters already exists 
	 * (ie UARTRx_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	UART_stream->state = BUSY;
	return HAL_UART_Receive_DMA(UART_stream->huart, &(UART_stream->byte), 1);
}

/**
 * @brief updates the stream structure fields and restarts data reception if necessary
 * 
 * @return HAL status (HAL_OK if no errors occured).
 * @note This function should be called at the end of data reception
 */
HAL_StatusTypeDef UARTRx_streamUpdate()
{
	uint8_t value;
	HAL_StatusTypeDef status;
	
	/* Check that UART parameters already exists 
	 * (ie UARTRx_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	// Immediately restart the UART so that we don't miss any bit
	status = HAL_OK;
	value = UART_stream->byte;

	if (UART_stream->state != INACTIVE)
	{
		status = UARTRx_streamRestart();
	}
	else
	{
		return HAL_ERROR;
	}

	saveByte(value);

	// Tell the main API that data has beed saved in the buffer
	UARTRx_FinishedHandle();
	return status;
}

/**
 * @brief stops a running stream.
 * 
 * @return HAL status (HAL_OK if no errors occured).
 */
HAL_StatusTypeDef UARTRx_streamStop()
{
	/* Check that UART parameters already exists 
	 * (ie UARTRx_streamStart() was called before)
	 */
	if (UART_stream == NULL)
	{
		return HAL_ERROR;
	}
	
	UART_stream->state = INACTIVE;

	return HAL_UART_Abort(UART_stream->huart);
}

/**
 * @brief saves a byte into the buffer
 * 
 * @param byte[in] byte to save
 * @return None
 */
static void saveByte(uint8_t byte)
{
	/* Check that UART parameters already exists */
	if (UART_stream != NULL)
	{
		UART_stream->lastByteIn += 1;
		if (UART_stream->lastByteIn >= UART_stream->length)
		{
			UART_stream->lastByteIn = 0;
		}
		(UART_stream->stream)[UART_stream->lastByteIn] = byte;
	}
}

/*=============================================================================
              ##### Receive and transmit functions #####
=============================================================================*/

/**
 * @brief checks if unsaved data is in the buffer
 * 
 * @return 1 if new data is in the buffer
 */
static uint8_t dataAvailable()
{
	/* Check that UART parameters already exists */
	if (UART_stream == NULL)
	{
		return 0;
	}
	// Check if there is new data in the buffer
	if (UART_stream->lastByteIn != UART_stream->lastByteOut)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
