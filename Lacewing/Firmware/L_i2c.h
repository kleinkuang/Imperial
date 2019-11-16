// File:    L_i2c.h
// Author:  Lei Kuang
// Date:    2019.11.13
// @ Imperial College London

// I2C Drivers for Thermometer Communication
// - Fast Mode: 400K

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"

/* Private defines -----------------------------------------------------------*/
#define I2C_DEVICE_ADDRESS 0xA0 // 0xA0: Two-wire Serial EEPROM 24C02 for test
                                // 0x90: Digital Thermometer and Thermostata DS7505+

/* Exported functions prototypes ---------------------------------------------*/

HAL_StatusTypeDef   L_I2C_Init                  (void);

uint32_t            L_I2C_Mem_Read_1            (uint16_t);
uint32_t            L_I2C_Mem_Read_2            (uint16_t);
HAL_StatusTypeDef   L_I2C_Mem_Write_1           (uint16_t, uint16_t);
HAL_StatusTypeDef   L_I2C_Mem_Write_2           (uint16_t, uint16_t);

/* Exported variables --------------------------------------------------------*/



