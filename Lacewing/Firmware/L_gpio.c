// File:    L_rcc.c
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// GPIO Configuration
// - Button
// - LED
// - TTN Signal Detection

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "L_gpio.h"

/* Private function prototypes -----------------------------------------------*/
void MX_GPIO_Init(void);

/* Exported function implementations -----------------------------------------*/

void L_GPIO_Init()
{
    MX_GPIO_Init();
}

GPIO_PinState Chip_Ready()
{
    return HAL_GPIO_ReadPin(TTN_READY_Port, TTN_READY_Pin);
}

GPIO_PinState Get_button()
{
    return HAL_GPIO_ReadPin(Button_Port, Button_Pin);
}

void Set_LED(GPIO_PinState pinState)
{
    HAL_GPIO_WritePin(LED0_Port, LED0_Pin, pinState);
}

void Toggle_LED()
{
    HAL_GPIO_TogglePin(LED0_Port, LED0_Pin);
}

/* Private function implementations ------------------------------------------*/

void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIO Ports Clock Enable
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // Configure GPIO pin Output Level
    HAL_GPIO_WritePin(LED0_Port, LED0_Pin, GPIO_PIN_RESET);

    // Configure GPIO pin : Button
    GPIO_InitStruct.Pin  = Button_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(Button_Port, &GPIO_InitStruct);

    // Configure GPIO pin : LED
    GPIO_InitStruct.Pin   = LED0_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LED0_Port, &GPIO_InitStruct);

    // Configure GPIO pin : TTN_Ready
    GPIO_InitStruct.Pin   = TTN_READY_Pin;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(TTN_READY_Port, &GPIO_InitStruct);
}
