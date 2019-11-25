/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f3xx_it.h
  * @brief   This file contains the headers of the interrupt handlers.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
 ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F3xx_IT_H
#define __STM32F3xx_IT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void NMI_Handler                (void);
void HardFault_Handler          (void);
void MemManage_Handler          (void);
void BusFault_Handler           (void);
void UsageFault_Handler         (void);
void SVC_Handler                (void);
void DebugMon_Handler           (void);
void PendSV_Handler             (void);

void SysTick_Handler            (void); // For HAL_Delay()
void DMA1_Channel6_IRQHandler   (void); // For UART Rx
void DMA2_Channel3_IRQHandler   (void); // For UART Rx
void TIM4_IRQHandler            (void); // For PWM IC

//void DMA1_Channel3_IRQHandler(void);
//void DMA1_Channel4_IRQHandler(void);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /* __STM32F3xx_IT_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/