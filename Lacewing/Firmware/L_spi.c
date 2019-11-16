// File:    L_spi.c
// Author:  Lei Kuang
// Date:    2019.11.12
// @ Imperial College London

// SPI Drivers for interfacing with TTN
// - MSB first


/* Includes ------------------------------------------------------------------*/
#include "L_spi.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef spi2Handle;

uint8_t spi2TxBuffer[2];
uint8_t spi2RxBuffer[2];

/* Private function prototypes -----------------------------------------------*/

/* User function implementations ---------------------------------------------*/

HAL_StatusTypeDef TTN_Reset()
{
    if(L_SPI_Transfer(0x2000)==0xFFFFFFFF)
        return HAL_ERROR;
    return HAL_OK;
}

// Set pixel row and column address
// - row: 0:77
// - col: 0:55
HAL_StatusTypeDef TTN_Set_Pixel_RC(uint16_t row, uint16_t col)
{
    // Set pixel row
    if(L_SPI_Transfer(0xC000 | row << 4)==0xFFFFFFFF)
        return HAL_ERROR;
    // Set pixel column
    if(L_SPI_Transfer(0xC800 | col << 5)==0xFFFFFFFF)
        return HAL_ERROR;

    return HAL_OK;
}

// Write to pixel RAM
// - val: 0:1023
HAL_StatusTypeDef TTN_Write_Pixel_RAM(uint16_t value)
{
    if(L_SPI_Transfer(0xDC00 | value)==0xFFFFFFFF)
        return HAL_ERROR;
    return HAL_OK;
}

// Read from pixel RAM
// - 1st cmd -> FSM exectues cmd and generates response
// - 2nd cmd -> SPI accepts response from FSM
// - 3rd cmd -> SPI sends out response
uint32_t TTN_Read_Pixel_RAM()
{
    uint16_t spi_seq = 0xD800;
    uint32_t spi_out;
    // Send read ram cmd
    if(L_SPI_Transfer(spi_seq)==0xFFFFFFFF)
        return 0xFFFFFFFF;
    HAL_Delay(1);
    // Empty cmd to read response
    if(L_SPI_Transfer(0x00)==0xFFFFFFFF)
        return 0xFFFFFFFF;
    spi_out = L_SPI_Transfer(0x00);
    return spi_out==0xFFFFFFFF ? 0xFFFFFFFF : spi_out>>6;
}

uint32_t TTN_Read_Pixel_RAM_with_Vs(uint16_t value)
{
    uint16_t spi_seq = 0xD000 | value;
    if(L_SPI_Transfer(spi_seq)==0xFFFFFFFF)
        return 0xFFFFFFFF;
    HAL_Delay(1);
    if(L_SPI_Transfer(0x00)==0xFFFFFFFF)
        return 0xFFFFFFFF;
    return L_SPI_Transfer(0x00); 
}

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef L_SPI_Init(void)
{
    spi2Handle.Instance               = SPI2;
    spi2Handle.Init.Mode              = SPI_MODE_MASTER;
    spi2Handle.Init.Direction         = SPI_DIRECTION_2LINES;
    spi2Handle.Init.DataSize          = SPI_DATASIZE_16BIT;
    spi2Handle.Init.CLKPolarity       = SPI_POLARITY_HIGH;          // Failing Edge !!!
    spi2Handle.Init.CLKPhase          = SPI_PHASE_1EDGE;
    spi2Handle.Init.NSS               = SPI_NSS_HARD_OUTPUT;
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;   // 32MHz / Prescaler
    spi2Handle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    spi2Handle.Init.TIMode            = SPI_TIMODE_DISABLE;
    spi2Handle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    spi2Handle.Init.CRCPolynomial     = 7;
    spi2Handle.Init.CRCLength         = SPI_CRC_LENGTH_DATASIZE;
    spi2Handle.Init.NSSPMode          = SPI_NSS_PULSE_ENABLE;
    if (HAL_SPI_Init(&spi2Handle)!= HAL_OK)
        return HAL_ERROR;

    return HAL_OK;
}

uint32_t L_SPI_Transfer(uint16_t txData)
{
    // Mask out bytes
    spi2TxBuffer[1] = txData >> 8;
    spi2TxBuffer[0] = txData & 0xFF;
    
    if(HAL_SPI_TransmitReceive(&spi2Handle, spi2TxBuffer, spi2RxBuffer, 1, 10)!= HAL_OK)
        return 0xFFFFFFFF;
    else
        return ((uint32_t)spi2RxBuffer[1] << 8) | (uint32_t)spi2RxBuffer[0];
}

/* Private function implementations ------------------------------------------*/
