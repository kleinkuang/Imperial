// File:    L_dac.c
// Author:  Lei Kuang
// Date:    2019.11.13
// @ Imperial College London

// DAC Drivers

/* Includes ------------------------------------------------------------------*/
#include "L_dac.h"

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef dac1Handle;

DMA_HandleTypeDef hdma_dac1_ch1;    // DMA not used
DMA_HandleTypeDef hdma_dac1_ch2;

uint32_t dac1Buffer;

/* Private function prototypes -----------------------------------------------*/
void L_DAC_DMA_Init (void);


/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_DAC_Init()
{
    //L_DAC_DMA_Init();

    // DAC Initialization 
    dac1Handle.Instance = DAC1;
    if (HAL_DAC_Init(&dac1Handle)!= HAL_OK)
        return HAL_ERROR;
    
    // DAC channel configuration
    DAC_ChannelConfTypeDef sConfig;
    sConfig.DAC_Trigger      = DAC_TRIGGER_SOFTWARE;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    if (HAL_DAC_ConfigChannel(&dac1Handle, &sConfig, DAC_CHANNEL_1) != HAL_OK)
        return HAL_ERROR;
    if (HAL_DAC_ConfigChannel(&dac1Handle, &sConfig, DAC_CHANNEL_2) != HAL_OK)
        return HAL_ERROR;
    
    return HAL_OK;
}

HAL_StatusTypeDef L_DAC_SetValue(uint16_t channel, uint16_t value)
{
    if (HAL_DAC_SetValue(&dac1Handle, channel==0 ? DAC_CHANNEL_1 : DAC_CHANNEL_2, DAC_ALIGN_12B_R, value)!= HAL_OK)
        return HAL_ERROR;

    if (HAL_DAC_Start(&dac1Handle, channel==0 ? DAC_CHANNEL_1 : DAC_CHANNEL_2)!= HAL_OK)
        return HAL_ERROR;
    
    return HAL_OK;
}

HAL_StatusTypeDef L_DAC_Stop(uint16_t channel)
{
    if (HAL_DAC_Stop(&dac1Handle, channel==0 ? DAC_CHANNEL_1 : DAC_CHANNEL_2)!= HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

uint32_t L_DAC_GetValue (uint16_t channel)
{
    return HAL_DAC_GetValue(&dac1Handle, channel==0 ? DAC_CHANNEL_1 : DAC_CHANNEL_2);
}

/* Private function implementations ------------------------------------------*/
