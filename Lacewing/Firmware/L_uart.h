// File:    L_uart.h
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// UART Drivers for Lacewing Project

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stdio.h"

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef L_UART_Init(void);

/* Exported variables --------------------------------------------------------*/
extern uint8_t  pc_cmd_avail;
extern uint8_t  bl_cmd_avail;
extern uint16_t func, par1, par2;
