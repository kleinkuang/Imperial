// File:    L_spi.c
// Author:  Lei Kuang
// Date:    2019.11.12
// @ Imperial College London

// SPI Drivers for interfacing with TTN
// - MSB first

/* Includes ------------------------------------------------------------------*/
#include "L_spi.h"
#include "main.h"

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef spi2Handle;

uint8_t spi2TxBuffer[2];
uint8_t spi2RxBuffer[2];

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
    #if SPI_CLK_FREQ == 16000000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;    // 32MHz/2 = 16Mhz
    #elif SPI_CLK_FREQ == 8000000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;    // 32MHz/4 = 8Mhz
    #elif SPI_CLK_FREQ == 4000000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;    // 32MHz/8 = 4Mhz
    #elif SPI_CLK_FREQ == 2000000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;   // 32MHz/16 = 2Mhz
    #elif SPI_CLK_FREQ == 1000000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;   // 32MHz/32 = 1Mhz
    #elif SPI_CLK_FREQ == 500000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;   // 32MHz/64 = 500KHz
    #elif SPI_CLK_FREQ == 250000
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;  // 32MHz/128 = 250KHz
    #else
    spi2Handle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;  // 32MHz/256 = 125KHz
    #endif
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
