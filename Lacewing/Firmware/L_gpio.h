// File:    L_gpio.h
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// GPIO Configuration

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void            L_GPIO_Init (void);
void            Set_LED     (GPIO_PinState);
void            Toggle_LED  (void);
GPIO_PinState   Get_Button  (void);
GPIO_PinState   Chip_Ready  (void);

/* Exported variables --------------------------------------------------------*/
