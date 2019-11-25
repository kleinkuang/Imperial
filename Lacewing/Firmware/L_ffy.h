// File:    L_ffy.h
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for FFY

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define FFY_END_FLAG        0xFFFF

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef   FFY_Transmit        (uint8_t*, uint16_t);
HAL_StatusTypeDef   FFY_Send_Data       (uint16_t);
HAL_StatusTypeDef   FFY_Send_Char       (char);

void                BL_Command_Service  (void);

/* Exported variables --------------------------------------------------------*/
extern uint8_t  bl_cmd_avail;
extern uint32_t bl_cmd;
