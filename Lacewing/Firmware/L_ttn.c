// File:    L_ttn.c
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for TTN

/* Includes ------------------------------------------------------------------*/
#include "L_ttn.h"
#include "L_spi.h"
#include "L_dac.h"
#include "L_gpio.h"
#include "L_pwm.h"

#include "L_pc.h"

/* Private defines -----------------------------------------------------------*/
#define TEMP_DAC_MAX        600     // Maximum value of DAC for Peltier
#define TEMP_DAC_MIN        300     // Minimum value of DAC for Peltier
#define TEMP_DAC_OFFSET     395     // DAC Offset for Peltier

#define TEMP_P              70      // P Controller
#define TEMP_I              1       // I Controller
#define TEMP_D              150     // D Controller

/* Private variables ---------------------------------------------------------*/
uint32_t    temp_dac        = 0;        // Actuator
uint32_t    temp_ref        = 0;        // Setpoint
uint32_t    temp_err_new    = 0;        // Error
uint32_t    temp_err_sum    = 0;
uint32_t    temp_err_old    = 0;

/* Private function prototypes -----------------------------------------------*/
static  void        TTN_Delay_Clock         (uint32_t); 
static  uint32_t    TTN_SPI_Transfer        (uint16_t);

/* Private function implementations ------------------------------------------*/

// According to the PWM output period which defines the TTN clock frequency
// Approximately delay a number of TTN clock cycles
static void TTN_Delay_Clock(uint32_t n)
{
    uint32_t period = L_PWM_Out_Get_Period() >> 3;
    
    // Loop for counting microsecond
    for(int i=0; i<n; i++)
    {
        for(int j=0; j<period; j++)
        {
        }
    }
}

static uint32_t TTN_SPI_Transfer (uint16_t data)
{
    // Check if TTN is ready
    TTN_Delay_Clock(10);
    while(Chip_Ready()==GPIO_PIN_RESET);
    
    uint32_t spi_out;
    spi_out = L_SPI_Transfer(data);

    return spi_out;
}


/* Exported function implementations -----------------------------------------*/

HAL_StatusTypeDef TTN_Init(uint8_t rst_dur, uint8_t pwm_max)
{
    uint16_t rst_dur_seq = 0x8500 | rst_dur;
    uint16_t pwm_max_seq = 0x8600 | pwm_max;

    // Reset TTN
    if(TTN_Reset()!=HAL_OK)
        return HAL_ERROR;

    // Define reset duration of pixel before measurement
    if(TTN_SPI_Transfer(rst_dur_seq)==0xFFFFFFFF)
        return HAL_ERROR;
    
    // Define PWM max width
    if(TTN_SPI_Transfer(pwm_max_seq)==0xFFFFFFFF)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef TTN_Reset()
{
    if(L_SPI_Transfer(0x2000)==0xFFFFFFFF)
        return HAL_ERROR;
    HAL_Delay(200);

    return HAL_OK;
}

// Set pixel row and column address
// - row: 0:77
// - col: 0:55
HAL_StatusTypeDef TTN_Set_Pixel_Row(uint16_t row)
{
    // Set pixel row
    if(TTN_SPI_Transfer(0xC000 | row << 4)==0xFFFFFFFF)
        return HAL_ERROR;

    return HAL_OK;
}

HAL_StatusTypeDef TTN_Set_Pixel_Col(uint16_t col)
{
    // Set pixel column
    if(TTN_SPI_Transfer(0xC800 | col << 5)==0xFFFFFFFF)
        return HAL_ERROR;
    
    return HAL_OK;
}

HAL_StatusTypeDef TTN_Set_Pixel_RC(uint16_t row, uint16_t col)
{
    // Set pixel row and column
    if(TTN_Set_Pixel_Row(row)!=HAL_OK)
        return HAL_ERROR;

    if(TTN_Set_Pixel_Col(col)!=HAL_OK)
        return HAL_ERROR;
    
    return HAL_OK;
}

// Write to pixel RAM
// - val: 0:1023
uint32_t TTN_Write_Pixel_RAM(uint16_t value)
{   
    uint32_t temp = TTN_SPI_Transfer(0xDC00 | value);
    
    return temp;
}

// Read from pixel RAM
// - 1st cmd -> FSM exectues cmd and generates response
// - 2nd cmd -> SPI accepts response from FSM
// - 3rd cmd -> SPI sends out response
uint32_t TTN_Read_Pixel_RAM()
{
    uint32_t spi_out;

    // Send read cmd
    if(TTN_SPI_Transfer(0xD800)==0xFFFFFFFF)
        return 0xFFFFFFFF;

    // Write 0 to RAM
    if(TTN_SPI_Transfer(0x00)==0xFFFFFFFF)
        return 0xFFFFFFFF;

    // Read
    spi_out = TTN_SPI_Transfer(0x00);
    
    return spi_out==0xFFFFFFFF ? 0xFFFFFFFF : spi_out>>6;
}

uint32_t TTN_Read_Pixel_RAM_with_Vs(uint16_t value)
{
    // Write Vs    
    if(TTN_SPI_Transfer(0xD000 | value)==0xFFFFFFFF)
        return 0xFFFFFFFF;
    TTN_Delay_Clock(50);
    
    // Write 0 to RAM
    if(TTN_Write_Pixel_RAM(0x00)==0xFFFFFFFF)
        return 0xFFFFFFFF;

    // Read    
    return TTN_SPI_Transfer(0x00);
}

HAL_StatusTypeDef TTN_Set_Vref(uint16_t vdac)
{
    // 10-bit vdac -> 12-bit DAC
    return L_DAC_SetValue(0, vdac<<2);
}

HAL_StatusTypeDef TTN_Set_Peltier(uint16_t vdac)
{
    // 10-bit vdac -> 12-bit DAC
    return L_DAC_SetValue(1, vdac<<2);
}

/* Exported advanced function implementations --------------------------------*/

uint32_t TTN_SweepSearch_Vs(uint32_t target)
{
    uint32_t cnt;
    uint32_t temp;
    
    for(cnt=0; cnt<1024; cnt++)
    {
        temp = TTN_Read_Pixel_RAM_with_Vs(cnt);
        //printf("%d\t\t%d\r\n", cnt, temp);
        if(temp >= VS_REG)
            break;
    }
    
    return cnt;
}

uint32_t TTN_BinarySearch_Vs(uint32_t target)
{
    uint32_t    min = 0;
    uint32_t    max = 1023;
    uint32_t    cnt = 0;
    uint32_t    data = 0;

    for(int i=0; i<11; i++)
    {
        cnt = (min+max+1) >> 1;
        data = TTN_Read_Pixel_RAM_with_Vs(cnt);
        //printf("%10d%10d%10d%10d\n", min, max, cnt, data);
        if(data >= target)
            max = cnt;
        else
            min = cnt;
    }
    
    return max;
}

uint32_t TTN_Count_Active_Pixels()
{
    uint32_t cnt = 0;
    uint32_t min, max;
    
    // Traverse on every ISFET pixel
    for(int col=0; col<COL_WIDTH; col++)
    {
        TTN_Set_Pixel_Col(col);
        for(int row=0; row<ROW_WIDTH; row++)
        {
            // Check if temperature pixel
            if(col%3==1 && row%3==1)
                continue;

            TTN_Set_Pixel_Row(row);

            // Estimate the discharge behaviour of ISFET pixels
            TTN_Write_Pixel_RAM(0);

            // Read pixel Vout at Vs=0
            min = TTN_Read_Pixel_RAM_with_Vs(0);
            // Read pixel Vout at Vs 800
            max = TTN_Read_Pixel_RAM_with_Vs(800);

            //printf("%5d%5d%10d%10d%5d\n", col, row, min, max, cnt);
            // If pixel is active
            if(min<100 && max>(VS_REG-VS_REG_TOL))
                cnt++;
        }
    }
    
    return cnt;
}

// Binary search algorithm to maximize the number of active (ON) ISFET pixels
uint32_t TTN_BinarySearch_Vref()
{
    uint32_t vref = 512;
    uint32_t step = 512;
    uint32_t cnt_ref, cnt;
    
    // Initialize at midpoint Vref = 0 (i.e. DAC 512/1023)
    TTN_Set_Vref(vref);
    // Count the initial number of active ISFET pixels
    cnt_ref = TTN_Count_Active_Pixels();

    while(step > 4)
    {
        step = step >> 1;

        // vref + step
        //TTN_Set_Vref(vref+step);
        cnt = TTN_Count_Active_Pixels();
        
        //printf("%5d%5d%5d\n", cnt_ref, cnt, vref+step);
        
        if(cnt>cnt_ref)
        {
            cnt_ref = cnt;
            vref = vref + step;
            continue;
        }
        
        // vref - step
        //TTN_Set_Vref(vref-step);
        cnt = TTN_Count_Active_Pixels();
        
        //printf("%5d%5d%5d\n", cnt_ref, cnt, vref-step);
        
        if(cnt>cnt_ref)
        {
            cnt_ref = cnt;
            vref = vref - step;
            continue;
        }
    }
    
    return vref;
}

// Temperature regulation for Peltier module
void TTN_Temp_Reg_Init()
{
    temp_dac     = 0;
    temp_ref     = 0;
    temp_err_new = 0;
    temp_err_sum = 0;
    temp_err_old = 0;
}

void TTN_Temp_Reg_Run(uint32_t frame_n, float temp)
{
    // Approximate positional PID function
    // - Form continuous to discrete
    //                           k
    //                          ---
    //   u[k] = P * e[k] + I *  \   e[i] + D * {e[k] - e[k-1]}
    //                          /
    //                          ---
    //                          i=0
    
    // PID
    if (frame_n > 10)
    {
        if(frame_n < 60)
        {
            temp_dac = TEMP_DAC_MAX; // Pre Heat up?
        }
        else if(frame_n < 200)
        {
            temp_dac = TEMP_DAC_OFFSET;

            if (frame_n == 199)
                temp_ref = temp;
        }
        else
        {
            // Error
            temp_err_new = temp_ref - temp;
            temp_err_sum = temp_err_sum + temp_err_new;
                                
            // Positional PID
            temp_dac = TEMP_DAC_OFFSET + TEMP_P * temp_err_new 
                                       + TEMP_I * temp_err_sum
                                       + TEMP_D * (temp_err_new-temp_err_old);
                                
            temp_err_old = temp_err_new;
            
            // Check if out of range
            if(temp_dac > TEMP_DAC_MAX) temp_dac = TEMP_DAC_MAX;
            if(temp_dac < TEMP_DAC_MIN) temp_dac = TEMP_DAC_MIN;
            
            // Update DAC value
            TTN_Set_Peltier(temp_dac);
        }
    }
}
