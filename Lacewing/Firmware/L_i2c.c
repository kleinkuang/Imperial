// File:    L_i2c.c
// Author:  Lei Kuang
// Date:    2019.11.13
// @ Imperial College London

// I2C Drivers for Thermometer Communication
// - Fast Mode: 400K
// - For DS7505, see Figure 9.2 Wire Interface Timing
// - A) Read 2 bytes from the temperature, tos, or thyst register (current pointer location)
// - B) Write to the configuration register
// - C) Write to the tos or thyst register
// - D) Read single byte (new pointer location)
// - E) Read from the configuration register (current pointer location)

// Pointer Definition (I2C Memory Address 0b0_0_0_0_0_0_P1_P2)
// - Register       P1      P0
// - Temperature    0       0
// - Configuration  0       1
// - T_HYST         1       0
// - T_OS           1       1

// Configuration Register
// - MSB    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    LSB
// - NVB    R1      R0      F1      F0      POL     TM      SD

// - NVB    NV Memory Status:   
//                              0 (Power-up)
//                              1: Write to an NV memory cell is in progress
//                              0: NV memory is not busy
// - R1:R0  Resolution Configuration: 
//                              0b00    (Power-up)
//                              0b00:   9-bit,  25ms (Max ConversionTime)
//                              0b01:   10-bit, 50ms
//                              0b10:   11-bit, 100ms
//                              0b11:   12-bit, 200ms
// - SD     Shutdown:
//                              0 (Power-up)
//                              0: Active conversion and thermostat operation
//                              1: Shutdown mode
//                              - When SD is programmed to 1, the conversion in progress is completed 
//                                and the result stored in the temperature register, after which the 
//                                DS7505 goes into a low-power standby state.
//                              - Writing a 0 to the SD bit returns the DS7505 to normal operation.
//

/* Includes ------------------------------------------------------------------*/
#include "L_i2c.h"

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef i2c1Handle;

uint8_t i2c1TxBuffer[2];
uint8_t i2c1RxBuffer[2];

/* Private function prototypes -----------------------------------------------*/

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_I2C_Init()
{
    i2c1Handle.Instance              = I2C1;
    i2c1Handle.Init.Timing           = 0x00602173;
    i2c1Handle.Init.OwnAddress1      = 0;
    i2c1Handle.Init.AddressingMode   = I2C_ADDRESSINGMODE_7BIT;
    i2c1Handle.Init.DualAddressMode  = I2C_DUALADDRESS_DISABLE;
    i2c1Handle.Init.OwnAddress2      = 0;
    i2c1Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    i2c1Handle.Init.GeneralCallMode  = I2C_GENERALCALL_DISABLE;
    i2c1Handle.Init.NoStretchMode    = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&i2c1Handle) != HAL_OK)
        return HAL_ERROR;

    /** Configure Analogue filter 
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&i2c1Handle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
        return HAL_ERROR;
    
    return HAL_OK;
}

uint32_t L_I2C_Mem_Read_1(uint16_t address)
{
    if(HAL_I2C_Mem_Read(&i2c1Handle, I2C_DEVICE_ADDRESS, address, 1, i2c1RxBuffer, 1, 1)!= HAL_OK)
        return 0xFFFFFFFF;
    else
        return (uint32_t)i2c1RxBuffer[0];
}

uint32_t L_I2C_Mem_Read_2(uint16_t address)
{
    if(HAL_I2C_Mem_Read(&i2c1Handle, I2C_DEVICE_ADDRESS, address, 1, i2c1RxBuffer, 2, 1)!= HAL_OK)
        return 0xFFFFFFFF;
    else    
        return ((uint32_t)i2c1RxBuffer[0] << 8) | (uint32_t)i2c1RxBuffer[1];
}

HAL_StatusTypeDef L_I2C_Mem_Write_1 (uint16_t address, uint16_t value)
{
    uint8_t temp = value;
    return HAL_I2C_Mem_Write(&i2c1Handle, I2C_DEVICE_ADDRESS, address, 1, &temp, 1, 1);
}

HAL_StatusTypeDef L_I2C_Mem_Write_2 (uint16_t address, uint16_t value)
{
    i2c1TxBuffer[0] = value >> 8;
    i2c1TxBuffer[1] = value & 0xFF;
    // i2c1TxBuffer sends out first
    return HAL_I2C_Mem_Write(&i2c1Handle, I2C_DEVICE_ADDRESS, address, 1, i2c1TxBuffer, 2, 1);
}

/* Private function implementations ------------------------------------------*/
