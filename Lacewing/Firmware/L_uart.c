// File:    L_uart.c
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// UART Drivers for Lacewing Project

/* Includes ------------------------------------------------------------------*/
#include "L_uart.h"

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

// UART Peripherals with Rx DMA for PC debug and Bluetooth Communication
UART_HandleTypeDef  huart4;
UART_HandleTypeDef  huart2;

DMA_HandleTypeDef   hdma_uart4_rx;
DMA_HandleTypeDef   hdma_usart2_rx;

uint8_t             rx2Buffer[6];
uint8_t             rx4Buffer[1];

/* Private function prototypes -----------------------------------------------*/
void                L_UART_DMA_Init     (void);
HAL_StatusTypeDef   MX_UART4_Init       (void);
HAL_StatusTypeDef   MX_USART2_UART_Init (void);

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_UART_Init()
{
    L_UART_DMA_Init();

    if (MX_UART4_Init() != HAL_OK)
        return HAL_ERROR;

    if (MX_USART2_UART_Init() != HAL_OK)
        return HAL_ERROR;

    HAL_UART_Receive_DMA(&huart2, rx2Buffer, 6);
    HAL_UART_Receive_DMA(&huart4, rx4Buffer, 1);

    return HAL_OK;
}

/* Callback functions --------------------------------------------------------*/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART2)
    {
        PC_RxCpltCallback(huart);
    }

    if(huart->Instance==UART4)
    {
        BL_RxCpltCallback(huart);
    }
}

__weak void PC_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
}

__weak void BL_RxCpltCallback(UART_HandleTypeDef *huart)
{
    /* Prevent unused argument(s) compilation warning */
    UNUSED(huart);
}

/* Private function implementations ------------------------------------------*/

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */

HAL_StatusTypeDef MX_UART4_Init(void)
{
    huart4.Instance                    = UART4;
    huart4.Init.BaudRate               = 115200;
    huart4.Init.WordLength             = UART_WORDLENGTH_8B;
    huart4.Init.StopBits               = UART_STOPBITS_1;
    huart4.Init.Parity                 = UART_PARITY_NONE;
    huart4.Init.Mode                   = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart4.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&huart4) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
HAL_StatusTypeDef MX_USART2_UART_Init(void)
{
    huart2.Instance                    = USART2;
    huart2.Init.BaudRate               = 115200;
    huart2.Init.WordLength             = UART_WORDLENGTH_8B;
    huart2.Init.StopBits               = UART_STOPBITS_1;
    huart2.Init.Parity                 = UART_PARITY_NONE;
    huart2.Init.Mode                   = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling           = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    if (HAL_UART_Init(&huart2) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

/**
  * Enable DMA controller clock
  */
void L_UART_DMA_Init(void)
{
    // DMA controller clock enable
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    // DMA interrupt init
    // DMA1_Channel6_IRQn interrupt configuration
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    // DMA2_Channel3_IRQn interrupt configuration
    HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);
}
