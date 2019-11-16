// File:    L_rcc.c
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// GPIO Configuration
// - Button
// - LED

/* Includes ------------------------------------------------------------------*/
#include "L_gpio.h"

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
    
/* Exported function implementations -----------------------------------------*/
void L_GPIO_Init()
{
    MX_GPIO_Init();
}

GPIO_PinState get_button()
{
    return HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);
}

void set_led(GPIO_PinState pinState)
{
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, pinState);
}

void toggle_led()
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

/* Private function implementations ------------------------------------------*/
/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : B1_Pin */
    GPIO_InitStruct.Pin = B1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pin : LD2_Pin */
    GPIO_InitStruct.Pin = LD2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);
}
