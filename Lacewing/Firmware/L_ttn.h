// File:    L_ttn.h
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for TTN

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define ROW_WIDTH               78
#define COL_WIDTH               56
#define VS_REG                  730
#define VS_REG_TOL              20

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef   TTN_Reset                   (void);
HAL_StatusTypeDef   TTN_Init                    (uint8_t, uint8_t);

HAL_StatusTypeDef   TTN_Set_Pixel_Row           (uint16_t);
HAL_StatusTypeDef   TTN_Set_Pixel_Col           (uint16_t);
HAL_StatusTypeDef   TTN_Set_Pixel_RC            (uint16_t, uint16_t);

HAL_StatusTypeDef   TTN_Set_Vref                (uint16_t);
HAL_StatusTypeDef   TTN_Set_Peltier             (uint16_t);

uint32_t            TTN_Write_Pixel_RAM         (uint16_t);
uint32_t            TTN_Read_Pixel_RAM          (void);
HAL_StatusTypeDef   TTN_Clear_Pixel_RAM         (void);
uint32_t            TTN_Read_Pixel_RAM_with_Vs  (uint16_t);

/* Exported advanced function implementations --------------------------------*/
uint32_t            TTN_SweepSearch_Vs          (uint32_t);
uint32_t            TTN_BinarySearch_Vs         (uint32_t);

uint32_t            TTN_Count_Active_Pixels     (void);
uint32_t            TTN_BinarySearch_Vref       (void);

void                TTN_Temp_Reg_Init           (void);
void                TTN_Temp_Reg_Run            (uint32_t, float);

/* Exported variables --------------------------------------------------------*/
