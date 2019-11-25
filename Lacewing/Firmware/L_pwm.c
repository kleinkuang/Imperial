// File:    L_pwm.c
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// PWM (TIM) Drivers for TTN Clock Generation
// - Timer:         TIM3
// - Channel:       1
// - Prescaler:     0 (Value: 1)
// - SYSCLK:        128MHz
// - PWM Period:    SYSCLK / (period+1);
// - Pulse:         period>>1 -> Make duty cycle always 50%
// - More details see STM32 CCR & ARR registers

// & PWM (TIM) Input Capture
// - Timer:         TIM4
// - Channel:       1(Indirect) & 2(Direct)
// - Prescaler:     0 (Value: 1)
// - SYSCLK:        128MHz
// - !!! Frequency measurement only works when PWM freq <=32MHz
// - !!! DutyCycle measurement only works when PWM freq <=16MHz

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "L_pwm.h"

#define TIM34CLK 128U //MHz

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef       tim3Handle;     // Timer handler declaration
TIM_OC_InitTypeDef      sConfigOC;      // Timer output compare configuration structure declaration

TIM_HandleTypeDef       tim4Handle;     // Timer handler declaration
TIM_IC_InitTypeDef      sConfigIC;      // Timer input capture configuration structure declaration
TIM_SlaveConfigTypeDef  sSlaveConfig;   // Slave configuration structure

uint32_t                pwmSampleCnt = 0;
uint32_t                pwmPeriodCnt = 0;
uint32_t                pwmPulseCnt  = 0;
float                   pwmDutyCycle = 0;
float                   pwmFrequency = 0;

// User Parameters
uint32_t period = TIM34CLK*1000000 / TTN_CLK_FREQ - 1;

/* Private function prototypes -----------------------------------------------*/
HAL_StatusTypeDef TIM3_Init         (void);
HAL_StatusTypeDef PWM_OC_Init       (void);
HAL_StatusTypeDef TIM3_PWM_OC_Init  (void);

HAL_StatusTypeDef TIM4_Init         (void);
HAL_StatusTypeDef PWM_IC_Init       (void);
HAL_StatusTypeDef TIM4_PWM_IC_Init  (void);

/* Callback function ---------------------------------------------------------*/

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
    {
        if(pwmSampleCnt==PWM_IC_SAMPLE_N-1) // Sample PWM IC more than once !
        {
            // Stop IT
            HAL_TIM_IC_Stop_IT(&tim4Handle, TIM_CHANNEL_2);
            HAL_TIM_IC_Stop_IT(&tim4Handle, TIM_CHANNEL_1);
            // Get counter values with compensation (+2,+3)
            pwmPeriodCnt = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2) + 2;
            pwmPulseCnt  = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) + 3;
            // Computate frequency and duty cycle
            if(pwmPeriodCnt==2)
            {
                pwmFrequency = (float)-1;
                pwmDutyCycle = (float)-1;
            }
            else
            {
                pwmFrequency = (float) TIM34CLK / pwmPeriodCnt;
                pwmDutyCycle = pwmPulseCnt!=3 ? pwmPulseCnt * 100 / (float) pwmPeriodCnt : (float)-1;
            }
        }
        pwmSampleCnt++;
    }
}

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_PWM_Init(void)
{
    // Init TIM3 for PWM output
    if (TIM3_PWM_OC_Init()!= HAL_OK)
        return HAL_ERROR;
    // Init TIM4 for PWM input
    if (TIM4_PWM_IC_Init()!= HAL_OK)
        return HAL_ERROR;
    
    // First attempt alaways gives wrong duty cycle...
    L_PWM_In_Get_DutyCycle();

    return HAL_OK;
}

uint32_t L_PWM_Out_Get_Period()
{
    return period;
}

float L_PWM_Out_Get_Frequency(void)
{
    return (float) TIM34CLK / (period+1);
}

HAL_StatusTypeDef L_PWM_Out_Set_Period(uint32_t new_period)
{
    // Stop PWM and Deinit TIM
    if (HAL_TIM_PWM_Stop(&tim3Handle, TIM_CHANNEL_1)!= HAL_OK)
        return HAL_ERROR;
    if (HAL_TIM_PWM_DeInit(&tim3Handle)!= HAL_OK)
        return HAL_ERROR;

    // Update period
    period = new_period;

    // Reinit
    return TIM3_PWM_OC_Init();
}

HAL_StatusTypeDef L_PWM_In_Start_Capture_IT()
{
    pwmSampleCnt = 0;
    /*
    if(HAL_TIM_IC_DeInit(&tim4Handle)!= HAL_OK)
        return HAL_ERROR;
    if(TIM4_PWM_IC_Init()!= HAL_OK)
        return HAL_ERROR;
    */
    
    if (HAL_TIM_IC_Start_IT(&tim4Handle, TIM_CHANNEL_2)!= HAL_OK)
        return HAL_ERROR;
    if (HAL_TIM_IC_Start(&tim4Handle, TIM_CHANNEL_1)!= HAL_OK)
        return HAL_ERROR;
    
    return HAL_OK;
}

HAL_StatusTypeDef L_PWM_In_Get_DutyCycle()
{
    if(L_PWM_In_Start_Capture_IT()!=HAL_OK)
        return HAL_ERROR;
    
    HAL_Delay(10);
    if(pwmSampleCnt!=PWM_IC_SAMPLE_N)
        return HAL_ERROR;
    
    return HAL_OK;
}

/* Private function implementations ------------------------------------------*/

HAL_StatusTypeDef TIM4_PWM_IC_Init()
{
    if (TIM4_Init() != HAL_OK)
        return HAL_ERROR;
    if (PWM_IC_Init() != HAL_OK)
        return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef TIM4_Init()
{
    // Initialize TIMx peripheral
    tim4Handle.Instance               = TIM4;
    tim4Handle.Init.Prescaler         = 0;
    tim4Handle.Init.Period            = 0xFFFF;
    tim4Handle.Init.ClockDivision     = 0;
    tim4Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    tim4Handle.Init.RepetitionCounter = 0;
    tim4Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_IC_Init(&tim4Handle)!= HAL_OK)
        return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef PWM_IC_Init()
{
    // Common configuration for all channels
    sConfigIC.ICPrescaler         = TIM_ICPSC_DIV1;
    sConfigIC.ICFilter            = 0;
    // Configure the Input Capture of channel 1
    sConfigIC.ICPolarity          = TIM_ICPOLARITY_FALLING;
    sConfigIC.ICSelection         = TIM_ICSELECTION_INDIRECTTI;
    if (HAL_TIM_IC_ConfigChannel(&tim4Handle, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
        return HAL_ERROR;
    // Configure the Input Capture of channel 2
    sConfigIC.ICPolarity          = TIM_ICPOLARITY_RISING;
    sConfigIC.ICSelection         = TIM_ICSELECTION_DIRECTTI;
    if (HAL_TIM_IC_ConfigChannel(&tim4Handle, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
        return HAL_ERROR;
    // Configure the slave mode - Select the slave Mode: Reset Mode
    sSlaveConfig.SlaveMode        = TIM_SLAVEMODE_RESET;
    sSlaveConfig.InputTrigger     = TIM_TS_TI2FP2;
    sSlaveConfig.TriggerPolarity  = TIM_TRIGGERPOLARITY_NONINVERTED;
    sSlaveConfig.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
    sSlaveConfig.TriggerFilter    = 0;
    if (HAL_TIM_SlaveConfigSynchro(&tim4Handle, &sSlaveConfig) != HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef TIM3_PWM_OC_Init()
{
    if (TIM3_Init()!= HAL_OK)
        return HAL_ERROR;
    if (PWM_OC_Init()!= HAL_OK)
        return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef TIM3_Init()
{
    // Initialize TIMx peripheral
    tim3Handle.Instance               = TIM3;
    tim3Handle.Init.Prescaler         = 0;
    tim3Handle.Init.Period            = period;
    tim3Handle.Init.ClockDivision     = 0;
    tim3Handle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    tim3Handle.Init.RepetitionCounter = 0;
    tim3Handle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_PWM_Init(&tim3Handle)!= HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef PWM_OC_Init()
{
    // Common configuration for all channels
    sConfigOC.OCMode       = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity   = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode   = TIM_OCFAST_DISABLE;
    sConfigOC.OCNPolarity  = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfigOC.OCIdleState  = TIM_OCIDLESTATE_RESET;
    // Set the pulse value for channel 1
    sConfigOC.Pulse        = period >> 1;
    if (HAL_TIM_PWM_ConfigChannel(&tim3Handle, &sConfigOC, TIM_CHANNEL_1)!= HAL_OK)
        return HAL_ERROR;
    // Start channel 1
    if (HAL_TIM_PWM_Start(&tim3Handle, TIM_CHANNEL_1)!= HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}
