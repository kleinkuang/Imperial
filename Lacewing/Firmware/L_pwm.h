// File:    L_pwm.h
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// PWM (TIM) Drivers for TTN Clock Generation

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define PWM_IC_SAMPLE_N 5 //Number of PWM IC samples

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef   L_PWM_Init                  (void);

uint32_t            L_PWM_Out_Get_Period        (void);
float               L_PWM_Out_Get_Frequency     (void);
HAL_StatusTypeDef   L_PWM_Out_Set_Period        (uint32_t);

HAL_StatusTypeDef   L_PWM_In_Start_Capture_IT   (void);
HAL_StatusTypeDef   L_PWM_In_Get_DutyCycle      (void);

/* Exported variables --------------------------------------------------------*/
extern uint32_t     pwmSampleCnt;
extern uint32_t     pwmPeriodCnt;
extern uint32_t     pwmPulseCnt;
extern float        pwmDutyCycle;
extern float        pwmFrequency;
