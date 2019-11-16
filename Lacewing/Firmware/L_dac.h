// File:    L_dac.h
// Author:  Lei Kuang
// Date:    2019.11.13
// @ Imperial College London

// SPI Drivers for TTN Communication

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef   L_DAC_Init      (void);
HAL_StatusTypeDef   L_DAC_SetValue  (uint16_t, uint16_t);
HAL_StatusTypeDef   L_DAC_Stop      (uint16_t);

uint32_t            L_DAC_GetValue  (uint16_t);

/* Exported variables --------------------------------------------------------*/
