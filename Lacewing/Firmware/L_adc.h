// File:    L_adc.h
// Author:  Lei Kuang
// Date:    2019.11.14
// @ Imperial College London

// ADC Drivers

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef   L_ADC_Init      (void);
uint32_t            L_ADC_Get_Value (void);

/* Exported variables --------------------------------------------------------*/
