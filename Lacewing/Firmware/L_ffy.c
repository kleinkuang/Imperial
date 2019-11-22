// File:    L_ffy.c
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for FFY

/* Includes ------------------------------------------------------------------*/
#include "L_ffy.h"
#include "L_uart.h"

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t     bl_cmd_avail = 0x00;
uint32_t    bl_cmd = 0;

/* Private function prototypes -----------------------------------------------*/

/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef FFY_Send_Char(char ch)
{
    uint8_t temp[1];
    temp[0] = ch;
    return HAL_UART_Transmit(&huart4, temp, 1, 0xFFFF);
}

HAL_StatusTypeDef FFY_Send_Data(uint16_t data)
{
    uint8_t temp[2];
    temp[0] = data >> 8;
    temp[1] = data & 0xFF;
    return HAL_UART_Transmit(&huart4, temp, 2, 0xFFFF);
}

/* Callback functions --------------------------------------------------------*/

void BL_RxCpltCallback(UART_HandleTypeDef *huart)
{
    bl_cmd_avail = 0x01;
    
    // Use UART DMA callback function to update the command (from Anselm's code)
    switch(bl_cmd)
    {
        case 0:     switch(rx4Buffer[0])
                    {
                        case 'Q':   bl_cmd = 1;
                                    break;
                        case 'R':   bl_cmd = 2;
                                    break;
                        case 'T':   bl_cmd = 3;
                                    break;
                        case 'U':   bl_cmd = 4;
                                    break;
                        case 'W':   bl_cmd = 7;
                                    break;
                    }
                    break;

        case 7:     if(rx4Buffer[0] == 'X')
                        bl_cmd = 0;
                    break;
    }
}

/* Private function implementations ------------------------------------------*/
