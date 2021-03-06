// File:    L_uart.h
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// UART Drivers for Lacewing Project

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define FFY_END_FLAG        0xFFFF

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef   L_UART_Init         (void);

void                BL_RxCpltCallback   (UART_HandleTypeDef *);
void                PC_RxCpltCallback   (UART_HandleTypeDef *);

/* Exported variables --------------------------------------------------------*/
extern UART_HandleTypeDef   huart4;
extern UART_HandleTypeDef   huart2;

extern uint8_t  rx2Buffer[6];
extern uint8_t  rx4Buffer[1];
