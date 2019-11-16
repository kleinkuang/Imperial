// File:    L_rcc.c
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// System Clock Configuration
// - Input frequency:   8MHz (HSE)
// - PLLMul:            x8
// - SYSCLK:            64MHz

/* Includes ------------------------------------------------------------------*/
#include "L_rcc.h"

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef SystemClock_Config(void);

/* Exported function implementations -----------------------------------------*/
HAL_StatusTypeDef L_SystemClock_Init()
{
    // Reset of all peripherals, Initializes the Flash interface and the Systick
    HAL_Init();

    if (SystemClock_Config() != HAL_OK)
    {
        return HAL_ERROR;
    }
    return HAL_OK;
}

/* Private function implementations ------------------------------------------*/
/**
  * @brief System Clock Configuration
  * @retval None
  */
HAL_StatusTypeDef SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct   = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct   = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState       = RCC_HSE_BYPASS;
    RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL8;
    RCC_OscInitStruct.PLL.PREDIV     = RCC_PREDIV_DIV1;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        return HAL_ERROR;

    /** Initializes the CPU, AHB and APB busses clocks 
    */
    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK  | RCC_CLOCKTYPE_SYSCLK |
                                       RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
        return HAL_ERROR;

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2 | RCC_PERIPHCLK_UART4 | 
                                         RCC_PERIPHCLK_TIM34  | RCC_PERIPHCLK_I2C1; 
    PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
    PeriphClkInit.Uart4ClockSelection  = RCC_UART4CLKSOURCE_PCLK1;
    PeriphClkInit.Tim34ClockSelection  = RCC_TIM34CLK_PLLCLK; // RCC_TIM34CLK_HCLK
    PeriphClkInit.I2c1ClockSelection   = RCC_I2C1CLKSOURCE_SYSCLK;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}
