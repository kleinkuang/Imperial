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
#include "L_gpio.h"     // - GPIO for LEDs and READY signal detection
#include "L_pwm.h"      // - Clock generation for TTN and PWM pulse capture
#include "L_spi.h"      // - SPI drivers for TTN communication
#include "L_i2c.h"      // - I2C drivers for thermometer communication
#include "L_dac.h"      // - DAC for Vref and Peltier control
#include "L_adc.h"      // - ADC for current sensing
                        // High-level Application Drivers
#include "L_pc.h"       // - Decode and operate commands from PC
#include "L_ttn.h"      // - TTN function and algorithms
#include "L_ffy.h"      // - FFY communication functions

/* Private defines -----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void        Save_Pixel_State_and_Send       (void);

/* Private variables ---------------------------------------------------------*/

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

    uint32_t    temp_u;
    
    uint32_t    ttn_cali_vref;
    
    TTN_Reset();
    
    while (1)
    {
        if(pc_cmd_avail == 0x01)
        {   
            // Commands from PC for debug, see driver "L_pc.c"
            pc_cmd_avail = 0x00;
            PC_Command_Service();
            
        }
        else if(bl_cmd_avail == 0x01)
        {
            // Commands from bluetooth for operation, see driver "L_ffy.c"
            bl_cmd_avail = 0;
            Set_LED(GPIO_PIN_SET);

            switch(bl_cmd)
            {
                // Initialize TTN
                case 1:
                {
                    //for(int i=0; i<1000; i++)
                    //{
                    //HAL_Delay(10);
                    TTN_Init(3, 50);

                    // Set Vref DAC to 0V
                    // L_DAC_SetValue(0, 0);

                    // Set Peltier DAC to 0V
                    // L_DAC_SetValue(1, 0);

                    // Check TTN is connected and operational using temperature pixel
                    TTN_Set_Pixel_RC(1,1);
                    TTN_Write_Pixel_RAM(0);
                    
                    // Sweep Vs from 0 to 1023 until vreg reached
                    // - Use binary search algorithm, approximately x25 times faster
                    //temp_u = TTN_SweepSearch_Vs(VS_REG);
                    //printf("%5d %d\n", i, temp_u);
                    //if(temp_u==0) break;
                    temp_u = TTN_BinarySearch_Vs(VS_REG);
                    //}

                    // Compare Vs, tau with optimal range
                    if(255<temp_u && temp_u<426)
                        FFY_Send_Char('a');
                    else
                        FFY_Send_Char('n'); // Bad result
    
                    // Operation complete
                    bl_cmd = 0;
                    FFY_Send_Data(FFY_END_FLAG);
                    break;
                }
                
                // Take temperature footprint
                case 2:
                {
                    // Traverse all temperature px
                    for(int col=1; col<COL_WIDTH; col=col+3)
                    {
                        TTN_Set_Pixel_Col(col);
                        for(int row=1; row<ROW_WIDTH; row=row+3)
                        {
                            TTN_Set_Pixel_Row(row);

                            TTN_Write_Pixel_RAM(0);
                            temp_u = TTN_BinarySearch_Vs(VS_REG);
                            TTN_Write_Pixel_RAM(temp_u);
                            printf("%3d%3d%5d\n", col, row, temp_u);

                            // Send to Firefly
                            //FFY_Send_Data(temp_u);
                        }
                    }

                    // Operation complete, deassert flag
                    bl_cmd = 0;
                    FFY_Send_Char('b');
                    FFY_Send_Data(FFY_END_FLAG);
                    break;
                }
                
                // Calibrate Vref
                case 3:
                {
                    // Get optimal Vref that maximizes the number of active ISFET pixels
                    ttn_cali_vref = TTN_BinarySearch_Vref();
                    //TTN_Set_Vref(ttn_cali_vref);
                    
                    // Store flags to indicate ON/OFF state of ISFET pixel and send to Firefly
                    Save_Pixel_State_and_Send();
                    
                    // Operation complete
                    bl_cmd = 0;
                    FFY_Send_Char('d');
                    FFY_Send_Data(FFY_END_FLAG);
                    break;
                }
                
                // Calibrate Vs
                case 4:
                {
                    // Traverse on every active ISFET pixel
                    for(int col=0; col<COL_WIDTH; col++)
                    {
                        TTN_Set_Pixel_Col(col);
                        for(int row=0; row<ROW_WIDTH; row++)
                        {
                            TTN_Set_Pixel_Row(row);
                            // Check if pixel is active
                            if(TTN_Read_Pixel_RAM()!=511)
                                continue;
                            
                            // Calibrate Vs
                            temp_u = TTN_BinarySearch_Vs(VS_REG-VS_REG_TOL);
                            // Write to RAM and send to FFY
                            TTN_Write_Pixel_RAM(temp_u);
                            FFY_Send_Data(temp_u);
                        }
                    }

                    // Operation complete
                    bl_cmd = 0;
                    FFY_Send_Char('e');
                    FFY_Send_Data(FFY_END_FLAG);
                    break;
                }
                
                // Readout with temperature regulation
                case 7:
                {
                    float       temp = 0;
                    uint32_t    vs_ram;
                    uint32_t    vs_read;
                    uint32_t    frame_n = 0;
                    
                    // Initialize temperature regulation
                    TTN_Temp_Reg_Init();
                    
                    while(bl_cmd == 7)
                    {
                        for(int col=0; col<COL_WIDTH; col++)
                        {
                            TTN_Set_Pixel_Col(col);
                            for(int row=0; row<ROW_WIDTH; row++)
                            {
                                TTN_Set_Pixel_Row(row);
                                
                                // Chemical px
                                // Change pixel
                                // Read and clear RAM Vs value

                                vs_ram = TTN_Read_Pixel_RAM();

                                // Intermediate calib Vs of active px
                                if(vs_ram!=0 && vs_ram!=1023)
                                {
                                    vs_read = TTN_Read_Pixel_RAM_with_Vs(vs_ram);

                                    if(vs_read<(VS_REG-VS_REG_TOL) && vs_ram<1020)
                                    {
                                        while(vs_read<(VS_REG-VS_REG_TOL) && vs_ram<1020)
                                        {
                                            vs_ram++;
                                            vs_read = TTN_Read_Pixel_RAM_with_Vs(vs_ram);
                                        }
                                        vs_ram--;
                                    }
                                    else if(vs_read>(VS_REG+VS_REG_TOL) && vs_ram>4)
                                    {
                                        while(vs_read>(VS_REG+VS_REG_TOL) && vs_ram>4)
                                        {
                                            vs_ram--;
                                            vs_read = TTN_Read_Pixel_RAM_with_Vs(vs_ram);
                                        }
                                        vs_ram++;
                                    }
                                }

                                // Store Vs in RAM
                                TTN_Write_Pixel_RAM(vs_ram);

                                // If temperature pixel
                                if((col%3==1) && (row%3==1) && (row>27) && (row<51) && (col>30))
                                {
                                    temp = temp + vs_ram;
                                }

                                // Send value to Firefly
                                FFY_Send_Data(vs_ram);
                            }
                        }

                        // Readout one frame complete
                        FFY_Send_Data(ttn_cali_vref);
                        temp = temp/72;
                        FFY_Send_Data(temp);
                        FFY_Send_Data(FFY_END_FLAG);
                        
                        // PID-based temperature regulation
                        // TTN_Temp_Reg_Run(frame_n, temp);
                        
                        frame_n++;
                    }
                }
            }
            
            Set_LED(GPIO_PIN_RESET);
        }
    }
}

/* User Function implementations ---------------------------------------------*/

void Save_Pixel_State_and_Send()
{
    uint32_t vs, vmin, vmax;
    
    for(int col=0; col<COL_WIDTH; col++)
    {
        TTN_Set_Pixel_Col(col);
        for(int row=0; row<ROW_WIDTH; row++)
        {
            TTN_Set_Pixel_Row(row);

            // Only search on ISFET pixels
            if((col%3==1) && (row%3==1))
            {
                vs = 511;
            }
            else
            {
                // Discharge behaviour of chemical px
                TTN_Write_Pixel_RAM(0);
                vmin = TTN_Read_Pixel_RAM_with_Vs(0);
                vmax = TTN_Read_Pixel_RAM_with_Vs(800);

                // Determine if pixel is ON/OFF
                if(vmin<100 && vmax>(VS_REG-VS_REG_TOL))                        // Pixel is active, write 511 to RAM
                    vs = 511;
                else if(vmin<100 && vmax<100)                                   // Discharge too fast, write 0 to RAM
                    vs = 0;
                else if(vmin>(VS_REG-VS_REG_TOL) && vmax>(VS_REG-VS_REG_TOL))   // Discharge too slow, write 1023 to RAM
                    vs = 1023;
            }

            // Write flags to RAM
            TTN_Write_Pixel_RAM(vs);
            // Send to Firefly
            FFY_Send_Data(vs);
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
