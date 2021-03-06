// File:    L_pc.c
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for PC Debug

/* Includes ------------------------------------------------------------------*/
#include "L_pc.h"

#include "L_uart.h"
#include "L_gpio.h"
#include "L_pwm.h"
#include "L_spi.h"
#include "L_i2c.h"
#include "L_dac.h"
#include "L_adc.h"

#include "L_ttn.h"
#include "L_ffy.h"

/* Private defines -----------------------------------------------------------*/
#define FIRMWARE_VERSION    (float) 0.01

/* Private variables ---------------------------------------------------------*/
uint8_t     pc_cmd_avail = 0x00;

uint16_t    func = 0x00;
uint16_t    par1 = 0x00;
uint16_t    par2 = 0x00;

/* Private function prototypes -----------------------------------------------*/
// Retargets the C library printf function to the USART
#ifdef __GNUC__
    /*  With GCC, small printf (option LD Linker->Libraries->Small printf
        set to 'Yes') calls __io_putchar() */
    #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
    #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Exported function implementations -----------------------------------------*/

// Cooperate with the Python GUI for debug
void PC_Command_Service()
{
    uint32_t    temp_u;
    float       temp_f;

    switch(func)
    {
        // ----------------------------------------------------------------
        // - Function only
        // - "test_serial":
        case 0x0000:    printf("M: Serial works well\n");
                        Print_End_Flag();
                        break;
        // - "get_version"
        case 0x0001:    printf("M: Firmware for Lacewing Project\n");
                        printf("   Version: %.2f\n", FIRMWARE_VERSION);
                        Print_End_Flag();
                                break;
        // - "get_pwm_out"
        case 0x0020:    temp_u = L_PWM_Out_Get_Period() + 1;
                        printf("M: PWM Out\n");
                        printf("   Period: %d\n", temp_u);
                        temp_f = L_PWM_Out_Get_Frequency();
                        if(temp_f >= (float)1)
                            printf("   Freq:   %.3fMHz\n", temp_f);
                        else
                            printf("   Freq:   %.3fKHz\n", temp_f*1000);
                            printf("$%d#", temp_u);
                        break;
                        
        // - "get_pwm_in"
        case 0x0022:    if(L_PWM_In_Get_DutyCycle()!=HAL_OK)
                            Print_Error();
                        else
                        {
                            if(pwmFrequency==(float)-1)
                                printf("M: Input PWM frequency must <= 32MHz\n$#");
                            else
                            {
                                printf("M: PWM In\n");
                                if(pwmFrequency>=(float)1)
                                    printf("   Freq: %.3fMHz\n", pwmFrequency);
                                else
                                    printf("   Freq: %.3fKHz\n", pwmFrequency*1000);
                                                    
                                if(pwmFrequency>(float)16)
                                    printf("   DutyCycle: Input PWM frequency must be <= 16MHz\n");
                                else
                                    printf("   DutyCycle: %.3f%%\n", pwmDutyCycle);
                                            
                                    Print_End_Flag();
                            }
                        }
                        break;
        // - "get_adc_value"
        case 0x0060:    temp_u = L_ADC_Get_Value();
                        if(temp_u==0xFFFFFFFF)
                            Print_Error();
                        else
                        {
                            printf("M: ADC In\n");
                            printf("   Val: %d\n", temp_u);
                            printf("   Vol: %.3f\n", (float)3.3*temp_u/4095);
                            Print_End_Flag();
                        }
                        break;
        // - "ttn_reset"
        case 0x00A0:    if(TTN_Reset()!=HAL_OK)
                            Print_Error();
                        else
                        {
                            printf("M: Reset TTN\n");
                            Print_End_Flag();
                        }
                        break;
        // - "ttn_read_ram"
        case 0x00A1:    temp_u = TTN_Read_Pixel_RAM();
                        if(temp_u==0xFFFFFFFF)
                            Print_Error();
                        else
                        {
                            printf("M: TTN Read Pixel RAM\n");
                            printf("   Val: 0x%x\n$%d#", temp_u, temp_u);
                        }
                        break;
        // ----------------------------------------------------------------
        // - Function + Parameter 1
        // - "test_func_1"
        case 0x1000:    printf("M: Test command\n");
                        printf("   Func: 0x%x\n", func);
                        printf("   Par1: 0x%x\n", par1);
                        Print_End_Flag();
                        break;
        // - "set_led":
        case 0x1010:    Set_LED(par1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
                        printf("M: Set LED %s\n", par1 ? "ON" : "OFF");
                        Print_End_Flag();
                        break;
        // - "set_pwm_out_period"
        case 0x1020:    if(L_PWM_Out_Set_Period(par1-1)!= HAL_OK)
                            Print_Error();
                        else
                        {
                            temp_u = L_PWM_Out_Get_Period() + 1;
                            printf("M: Set PWM Out Period\n");
                            printf("   Period: %d\n", temp_u);
                            temp_f = L_PWM_Out_Get_Frequency();
                            if(temp_f >= (float)1)
                                printf("   Freq:   %.3fMHz\n", temp_f);
                            else
                                printf("   Freq:   %.3fKHz\n", temp_f*1000);
                            Print_End_Flag();
                        }
                        break;
        // - "write_spi"
        case 0x1030:    temp_u = L_SPI_Transfer(par1);
                        if(temp_u==0xFFFFFFFF)
                            Print_Error();
                        else
                        {
                            printf("M: SPI Transfer\n");
                            printf("   Write: 0x%x\n", par1);
                            printf("   Read:  0x%x\n$%d#", temp_u, temp_u);
                        }
                        break;
        // - "read_i2c_1"
        case 0x1040:    temp_u = L_I2C_Mem_Read_1(par1);
                        if(temp_u==0xFFFFFFFF)
                            Print_Error();
                        else
                        {
                            printf("M: I2C Read\n");
                            printf("   Add: 0x%x\n", par1);
                            printf("   Val: 0x%x\n$%d#", temp_u, temp_u);
                        }
                        break;
        // - "read_i2c_2"
        case 0x1041:    temp_u = L_I2C_Mem_Read_2(par1);
                        if(temp_u==0xFFFFFFFF)
                            Print_Error();
                        else
                        {
                            printf("M: I2C Read\n");
                            printf("   Add: 0x%x\n", par1);
                            printf("   Val: 0x%x\n$%d#", temp_u, temp_u);
                        }
                        break;
        // - "get_dac_value"
        case 0x1050:    temp_u = L_DAC_GetValue(par1);
                        printf("M: Get DAC Value\n");
                        printf("   Cha: %d\n", par1);
                        printf("   Val: %d\n", temp_u);
                        printf("   Vol: %.3f\n", (float)3.3*temp_u/4095);
                        printf("$%d#", temp_u);
                        break;
        // - "stop_dac"
        case 0x1051:    temp_u = L_DAC_Stop(par1);
                        printf("M: Stop DAC\n");
                        printf("   Cha: %d\n", par1);
                        Print_End_Flag();
                        break;
        // - "ttn_write_ram"
        case 0x10A0:    if(par1>1023)
                        {
                            printf("M: Value out of range 0:1023\n");
                            Print_End_Flag();
                        }
                        else
                        {
                            if(TTN_Write_Pixel_RAM(par1)==0xFFFFFFFF)
                                Print_Error();
                            else
                            {
                                printf("M: TTN Write Pixel RAM \n");
                                printf("   Val: 0x%x\n", par1);
                                Print_End_Flag();
                            }
                        }
                        break;
        // - "ttn_read_ram_witn_vs"
        case 0x10A1:    if(par1>1023)
                        {
                            printf("M: Vs out of range 0:1023\n");
                            Print_End_Flag();
                        }
                        else
                        {
                            temp_u = TTN_Read_Pixel_RAM_with_Vs(par1);
                            if(temp_u==0xFFFFFFFF)
                                Print_Error();
                            else
                            {
                                printf("M: TTN Read Pixel RAM with Vs\n");
                                printf("   Val: 0x%x\n", temp_u);
                                Print_End_Flag();
                            }
                        }
                        break;
        // ----------------------------------------------------------------
        // - Function + Parameter 1 + Parameter 2
        case 0x2000:    printf("M: Test command\n");
                        printf("   Func: 0x%x\n", func);
                        printf("   Par1: 0x%x\n", par1);
                        printf("   Par2: 0x%x\n", par2);
                        Print_End_Flag();
                        break;
        // - "write_i2c_1"
        case 0x2040:    if(L_I2C_Mem_Write_1(par1, par2)!=HAL_OK)
                            Print_Error();
                        else
                        {
                            printf("M: I2C Write\n");
                            printf("   Add: 0x%x\n", par1);
                            printf("   Val: 0x%x\n", par2&0xFF);
                            Print_End_Flag();
                        }
                        break;
        // - "write_i2c_2"
        case 0x2041:    if(L_I2C_Mem_Write_2(par1, par2)!=HAL_OK)
                            Print_Error();
                        else
                        {
                            printf("M: I2C Write\n");
                            printf("   Add: 0x%x\n", par1);
                            printf("   Val: 0x%x\n", par2);
                            Print_End_Flag();
                        }
                        break;
        // - "set_dac_value"
        case 0x2050:    par2 = par2 > 4095 ? 4095 : par2;
                        if(L_DAC_SetValue(par1, par2)!=HAL_OK)
                            Print_Error();
                        else
                        {
                            temp_u = L_DAC_GetValue(par1);
                            printf("M: Get DAC Value\n");
                            printf("   Cha: %d\n", par1);
                            printf("   Val: %d\n", temp_u);
                            printf("   Vol: %.3f\n", (float)3.3*temp_u/4095);
                            Print_End_Flag();
                        }
                        break;              
        // - "ttn_set_rc"
        case 0x20A0:    if(par1 > 77)
                        {
                            printf("M: Row addr out of range 0:77\n");
                            Print_End_Flag();
                        }
                        else
                            if(par2 > 55)
                            {
                                printf("M: Col addr out of range 0:55\n");
                                Print_End_Flag();
                            }
                            else
                                if(TTN_Set_Pixel_RC(par1, par2)!= HAL_OK)
                                    Print_Error();
                                else
                                {
                                    printf("M: Set TTN Pixel Addr\n");
                                    printf("   Row: %d\n", par1);
                                    printf("   Col: %d\n", par2);
                                    Print_End_Flag();
                                }
                        break;
        // ----------------------------------------------------------------
        // - Default
        default:        printf("Function not implemented\n");
                        Print_End_Flag();
    }
}

void Print_End_Flag()
{
    printf("$#");
}

void Print_Error()
{
    printf("M: Error!\n");
    Print_End_Flag();
}

/* Callback functions --------------------------------------------------------*/

void PC_RxCpltCallback(UART_HandleTypeDef *huart)
{
    pc_cmd_avail = 0x01;

    func   = rx2Buffer[0] << 8 | rx2Buffer[1];
    par1   = rx2Buffer[2] << 8 | rx2Buffer[3];
    par2   = rx2Buffer[4] << 8 | rx2Buffer[5];
}

/* Private function implementations ------------------------------------------*/

// Retargets the C library printf function to the USART.
PUTCHAR_PROTOTYPE
{
    /* Place your implementation of fputc here */
    /* e.g. write a character to the EVAL_COM1 and Loop until the end of transmission */
    HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
