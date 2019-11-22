// File:    L_pc.h
// Author:  Lei Kuang
// Date:    2019.11.18
// @ Imperial College London

// Application Drivers for PC Debug

/* Includes ------------------------------------------------------------------*/
#include "stm32f3xx_hal.h"
#include "stdio.h"

/* Private defines -----------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/
void        PC_Command_Service      (void);
void        Print_Error             (void);
void        Print_End_Flag          (void);

/* Exported variables --------------------------------------------------------*/
extern uint8_t  pc_cmd_avail;
extern uint16_t func, par1, par2;
