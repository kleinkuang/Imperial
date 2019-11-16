// File:    L_spi.h
// Author:  Lei Kuang
// Date:    2019.11.12
// @ Imperial College London

// SPI Drivers for TTN Communication

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef   L_SPI_Init      (void);
uint32_t            L_SPI_Transfer  (uint16_t);

HAL_StatusTypeDef   TTN_Reset                   (void);
HAL_StatusTypeDef   TTN_Set_Pixel_RC            (uint16_t, uint16_t);
HAL_StatusTypeDef   TTN_Write_Pixel_RAM         (uint16_t);
uint32_t            TTN_Read_Pixel_RAM          (void);
uint32_t            TTN_Read_Pixel_RAM_with_Vs  (uint16_t);

/* Exported variables --------------------------------------------------------*/
