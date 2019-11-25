// File:    main.h
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// Firmware for Lacewing Project

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private includes ----------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* Private defines -----------------------------------------------------------*/

// ----------------------------------------------------------------
// TTN clock frequency [Hz] (initial but tunable during the run time)
#define TTN_CLK_FREQ        4000000
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// SPI clock frequency [Hz] (fixed, currently cannot be tunned)
#define SPI_CLK_FREQ        8000000
                                        // - Can only be the following [Hz]:
                                        // - 16000000,  8000000,    4000000,
                                        // - 2000000,   1000000,    500000,
                                        // - 250000,    125000
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// - GPIO
#define Button_Pin          GPIO_PIN_13 
#define Button_Port         GPIOC
#define LED0_Pin            GPIO_PIN_7      // Cmd Operation Indicator
#define LED0_Port           GPIOC
#define TTN_READY_Pin       GPIO_PIN_8
#define TTN_READY_Port      GPIOC
// - PWM (TIM3 for OC & TIM4 for IC)
#define PWM_OC_Pin          GPIO_PIN_6      // For TTN Clock Generation
#define PWM_OC_Port         GPIOC
#define PWM_IC_Pin          GPIO_PIN_7      // For TTN PWM Capture
#define PWM_IC_Port         GPIOB
// - UART
#define PC_UART2_Tx_Pin     GPIO_PIN_2
#define PC_UART2_Rx_Pin     GPIO_PIN_3
#define PC_UART2_Port       GPIOA
#define BL_UART4_Tx_Pin     GPIO_PIN_10
#define BL_UART4_Rx_Pin     GPIO_PIN_11
#define BL_UART4_Port       GPIOC
// - SPI
#define SPI2_CS_Pin         GPIO_PIN_12     // Also known as NSS, SSEL
#define SPI2_SCK_Pin        GPIO_PIN_13
#define SPI2_MISO_Pin       GPIO_PIN_14
#define SPI2_MOSI_Pin       GPIO_PIN_15
#define SPI2_Port           GPIOB
// - I2C
#define I2C1_SCL_Pin        GPIO_PIN_8
#define I2C1_SDA_Pin        GPIO_PIN_9
#define I2C1_Port           GPIOB
// - DAC
#define DAC1_CHN1_Pin       GPIO_PIN_4      // For TTN Vref
#define DAC1_CHN2_Pin       GPIO_PIN_5      // For Peltier Module
#define DAC1_Port           GPIOA
// - ADC
#define ADC1_IN1_Pin        GPIO_PIN_0  
#define ADC1_Port           GPIOA
// ----------------------------------------------------------------

/*
#define TMS_Pin             GPIO_PIN_13
#define TMS_GPIO_Port       GPIOA
#define TCK_Pin             GPIO_PIN_14
#define TCK_GPIO_Port       GPIOA
#define SWO_Pin             GPIO_PIN_3
#define SWO_GPIO_Port       GPIOB
*/

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

