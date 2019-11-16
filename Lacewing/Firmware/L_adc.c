// File:    L_adc.c
// Author:  Lei Kuang
// Date:    2019.11.14
// @ Imperial College London

// ADC Drivers

/* Includes ------------------------------------------------------------------*/
#include "L_adc.h"

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef adc1Handle;

/* Private function prototypes -----------------------------------------------*/

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_ADC_Init()
{    
    ADC_MultiModeTypeDef multimode = {0};
    ADC_ChannelConfTypeDef sConfig = {0};
    
    // Common config 
    adc1Handle.Instance                   = ADC1;
    adc1Handle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV1;
    adc1Handle.Init.Resolution            = ADC_RESOLUTION_12B;
    adc1Handle.Init.ScanConvMode          = ADC_SCAN_DISABLE;
    adc1Handle.Init.ContinuousConvMode    = DISABLE;
    adc1Handle.Init.DiscontinuousConvMode = DISABLE;
    adc1Handle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc1Handle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;
    adc1Handle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
    adc1Handle.Init.NbrOfConversion       = 1;
    adc1Handle.Init.DMAContinuousRequests = DISABLE;
    adc1Handle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;
    adc1Handle.Init.LowPowerAutoWait      = DISABLE;
    adc1Handle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;
    if(HAL_ADC_Init(&adc1Handle) != HAL_OK)
        return HAL_ERROR;
    
    // Configure the ADC multi-mode 
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if(HAL_ADCEx_MultiModeConfigChannel(&adc1Handle, &multimode) != HAL_OK)
        return HAL_ERROR;

    // Configure Regular Channel 
    sConfig.Channel      = ADC_CHANNEL_1;
    sConfig.Rank         = ADC_REGULAR_RANK_1;
    sConfig.SingleDiff   = ADC_SINGLE_ENDED;
    sConfig.SamplingTime = ADC_SAMPLETIME_19CYCLES_5;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset       = 0;
    if(HAL_ADC_ConfigChannel(&adc1Handle, &sConfig) != HAL_OK)
        return HAL_ERROR;
    
    if(HAL_ADCEx_Calibration_Start(&adc1Handle, ADC_SINGLE_ENDED))
        return HAL_ERROR;

    return HAL_OK;
}

uint32_t L_ADC_Get_Value()
{
    uint32_t sum = 0;
    
    // Withdraw first sample
    HAL_ADC_Start(&adc1Handle);
    HAL_ADC_PollForConversion(&adc1Handle, 1);
    
    for(int i=0; i<4; i++)
    {
        if(HAL_ADC_Start(&adc1Handle)!= HAL_OK)
            return 0xFFFFFFFF;
        if(HAL_ADC_PollForConversion(&adc1Handle, 1)!= HAL_OK)
            return 0xFFFFFFFF;
        else
            sum += HAL_ADC_GetValue(&adc1Handle);
    }

    return sum >> 2;
}

/* Private function implementations ------------------------------------------*/
