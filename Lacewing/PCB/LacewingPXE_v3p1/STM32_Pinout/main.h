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
#define TTN_CLK_FREQ        500000
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// SPI clock frequency [Hz] (fixed, currently cannot be tunned)
#define SPI_CLK_FREQ        1000000
                                        // - Can only be the following [Hz]:
                                        // - 16000000,  8000000,    4000000,
                                        // - 2000000,   1000000,    500000,
                                        // - 250000,    125000
// ----------------------------------------------------------------

// ----------------------------------------------------------------
// - GPIO
#define BUTTON_PIN          GPIO_PIN_13                                         // Not Used for LacewingPXE
#define BUTTON_PORT         GPIOC                                               // Not Used for LacewingPXE
#define GREEN_LED_PIN       GPIO_PIN_0                                          // PCB Check OK
#define GREEN_LED_PORT      GPIOB                                               // PCB Check OK
#define RED_LED_PIN         GPIO_PIN_1                                          // PCB Check OK
#define RED_LED_PORT        GPIOB                                               // PCB Check OK
#define TTN_READY_PIN       GPIO_PIN_8                                          // PCB Check OK
#define TTN_READY_PORT      GPIOA                                               // PCB Check OK

// - PWM (TIM3 for OC & TIM4 for IC)
#define PWM_OC_PIN          GPIO_PIN_6      // For TTN Clock Generation         // PCB Check OK
#define PWM_OC_PORT         GPIOC                                               // PCB Check OK
#define PWM_IC_PIN          GPIO_PIN_7      // For TTN PWM Capture              // PCB Check OK
#define PWM_IC_PORT         GPIOB                                               // PCB Check OK

// - UART
#define PC_UART2_TX_PIN     GPIO_PIN_2      // USART2                           // PCB Check OK
#define PC_UART2_RX_PIN     GPIO_PIN_3                                          // PCB Check OK
#define PC_UART2_PORT       GPIOA                                               // PCB Check OK
#define BL_UART4_TX_PIN     GPIO_PIN_10     // UART4                            // PCB Check OK
#define BL_UART4_RX_PIN     GPIO_PIN_11                                         // PCB Check OK
#define BL_UART4_PORT       GPIOC                                               // PCB Check OK

// - SPI
#define SPI2_CS_PIN         GPIO_PIN_12     // Also known as NSS, SSEL          // PCB Check OK
#define SPI2_SCK_PIN        GPIO_PIN_13                                         // PCB Check OK
#define SPI2_MISO_PIN       GPIO_PIN_14                                         // PCB Check OK
#define SPI2_MOSI_PIN       GPIO_PIN_15                                         // PCB Check OK
#define SPI2_PORT           GPIOB                                               // PCB Check OK

// - I2C
#define I2C1_SCL_PIN        GPIO_PIN_8      // For Thermometer DS7505U+         // PCB Check OK
#define I2C1_SDA_PIN        GPIO_PIN_9                                          // PCB Check OK
#define I2C1_PORT           GPIOB                                               // PCB Check OK

// - DAC
#define DAC1_CHN1_PIN       GPIO_PIN_4      // For TTN Vref                     // PCB Check OK
#define DAC1_CHN2_PIN       GPIO_PIN_5      // For Peltier Module               // PCB Check OK
#define DAC1_PORT           GPIOA                                               // PCB Check OK

// - ADC
#define ADC1_IN1_PIN        GPIO_PIN_0                                          // PCB Check OK
#define ADC1_PORT           GPIOA                                               // PCB Check OK
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

