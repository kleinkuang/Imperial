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

uint8_t rx2Buffer[6];
uint8_t rx4Buffer[6];

uint8_t pc_cmd_avail = 0x00;
uint8_t bl_cmd_avail = 0x00;
uint16_t func = 0x00;
uint16_t par1 = 0x00;
uint16_t par2 = 0x00;

/* Private function prototypes -----------------------------------------------*/

void                L_UART_DMA_Init     (void);
HAL_StatusTypeDef   MX_UART4_Init       (void);
HAL_StatusTypeDef   MX_USART2_UART_Init (void);
void                decode_cmd          (uint8_t *);

// Retargets the C library printf function to the USART
#ifdef __GNUC__
    /*  With GCC, small printf (option LD Linker->Libraries->Small printf
        set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_UART_Init()
{
    L_UART_DMA_Init();
    
    if (MX_UART4_Init() != HAL_OK)
        return HAL_ERROR;

    if (MX_USART2_UART_Init() != HAL_OK)
        return HAL_ERROR;

    HAL_UART_Receive_DMA(&huart2, rx2Buffer, 6);
    HAL_UART_Receive_DMA(&huart4, rx4Buffer, 6);

    return HAL_OK;
}

/* Callback functions --------------------------------------------------------*/

// Callback function for UART DMA
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance==USART2)
    {
        decode_cmd(rx2Buffer);
        pc_cmd_avail = 0x01;
    }

    if(huart->Instance==UART4)
    {
        bl_cmd_avail = 0x01;
        HAL_UART_Transmit(&huart4, rx4Buffer, 6, 0xFFFF);
    }
}

/* Private function implementations ------------------------------------------*/

void decode_cmd(uint8_t *rxBuffer)
{
    func   = rxBuffer[0] << 8 | rxBuffer[1];
    par1   = rxBuffer[2] << 8 | rxBuffer[3];
    par2   = rxBuffer[4] << 8 | rxBuffer[5];
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */

PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */

HAL_StatusTypeDef MX_UART4_Init(void)
{
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;
    huart4.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
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
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
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
    /* DMA controller clock enable */
    __HAL_RCC_DMA1_CLK_ENABLE();
    __HAL_RCC_DMA2_CLK_ENABLE();

    /* DMA interrupt init */
    // For UART
    /* DMA1_Channel6_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
    /* DMA2_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA2_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA2_Channel3_IRQn);
    // For DAC
    /* DMA1_Channel3_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel3_IRQn);
    /* DMA1_Channel4_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}
