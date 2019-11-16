// File:    L_gpio.h
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// GPIO Configuration

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define B1_Pin              GPIO_PIN_13
#define B1_GPIO_Port        GPIOC
#define LD2_Pin             GPIO_PIN_7  //GPIO_PIN_5
#define LD2_GPIO_Port       GPIOC       //GPIOA

/* Exported functions prototypes ---------------------------------------------*/
void            L_GPIO_Init (void);
void            set_led     (GPIO_PinState);
void            toggle_led  (void);
GPIO_PinState   get_button  (void);

/* Exported variables --------------------------------------------------------*/
