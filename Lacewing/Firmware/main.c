// File:    main.c
// Author:  Lei Kuang
// Date:    2019.11.11
// @ Imperial College London

// Firmware for Lacewing Project v0.01

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
                        // STM32 Peripherial Drivers
#include "L_rcc.h"      // - Clock configuration for core and peripherials
#include "L_uart.h"     // - UART drivers for PC and RN41 communication
#include "L_gpio.h"     // - GPIO for Button, LED and TTN ready signal detection
#include "L_pwm.h"      // - Clock generation for TTN and PWM pulse capture
#include "L_spi.h"      // - SPI drivers for TTN communication
#include "L_i2c.h"      // - I2C drivers for thermometer communication
#include "L_dac.h"      // - DAC for TTN Vref and Peltier module
#include "L_adc.h"      // - ADC for current sensing
                        // High-level Application Drivers
#include "L_pc.h"       // - Decode and operate cmds from PC
#include "L_ttn.h"      // - TTN function and algorithms
#include "L_ffy.h"      // - Decode and operate cmds from Android Smart Phone

/* Guide ---------------------------------------------------------------------*/
// Development Board Pinout
// - Quick view: https://os.mbed.com/platforms/ST-Nucleo-F303RE/

// Install STM32CubeMAX
// - Detailed Pinout
// - Peripheral Examples
// - Customize Settings for Peripheral Drivers and Generate initial code

// Install ARM MDK Keil uVersion5 for firmware development

int main(void)
{
    /* MCU Configuration -----------------------------------------------------*/
    // Reset all peripherals and configure the system clock
    L_SystemClock_Init();

    /* Initialize peripherals ------------------------------------------------*/
    L_GPIO_Init();
    if(L_UART_Init()!=HAL_OK) Error_Handler();
    if(L_PWM_Init() !=HAL_OK) Error_Handler();
    if(L_SPI_Init() !=HAL_OK) Error_Handler();
    if(L_I2C_Init() !=HAL_OK) Error_Handler();
    if(L_DAC_Init() !=HAL_OK) Error_Handler();
    if(L_ADC_Init() !=HAL_OK) Error_Handler();

    /* Program entry ---------------------------------------------------------*/    
    Set_LED(GPIO_PIN_SET);

    while (1)
    {
        if(pc_cmd_avail == 0x01)
        {
            // Commands from PC for debug, see driver "L_pc.c"
            pc_cmd_avail = 0x00;
            Set_LED(GPIO_PIN_SET);
            // ----------------------------------------------------------------
            PC_Command_Service();
            // ----------------------------------------------------------------
            Set_LED(GPIO_PIN_RESET);
        }
        
        if(bl_cmd_avail == 0x01)
        {
            // Commands from bluetooth for operation, see driver "L_ffy.c"
            bl_cmd_avail = 0;
            Set_LED(GPIO_PIN_SET);
            // ----------------------------------------------------------------
            BL_Command_Service();
            // ----------------------------------------------------------------
            Set_LED(GPIO_PIN_RESET);
        }
    }
}

/* Function implementations --------------------------------------------------*/

void Error_Handler(void)
{
    // If HAL error occurs, blink the LED forever
    while(1)
    {
        Toggle_LED();
        HAL_Delay(100); 
    }
}
