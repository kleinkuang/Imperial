// File:    L_ffy.c
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for FFY

/* Includes ------------------------------------------------------------------*/
#include "L_ffy.h"
#include "L_uart.h"
#include "L_pc.h"

#include "L_ttn.h"

/* Private defines -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
uint8_t     bl_cmd_avail = 0x00;
uint32_t    bl_cmd = 0;

uint32_t    ttn_cali_vref;

/* Private function prototypes -----------------------------------------------*/
void        Save_Pixel_State_and_Send       (void);

/* Exported function implementations -----------------------------------------*/

// Cooperate with the Python GUI for debug
void BL_Command_Service()
{
    uint32_t    temp_u;

    switch(bl_cmd)
    {
        // Initialize TTN
        case 1:
        {
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
            for(int t=0; t<100; t++)
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
                printf("--%d\n", t);
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
}

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
