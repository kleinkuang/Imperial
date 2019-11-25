Version 0.4
-   Move all IO pin and port defines to main.h
-   Modify TTN_Read_Pixel_RAM_with_Vs function in L_ttn.c
    > Send void cmd instead of writing 0 to pixel ram which prevents TTN ready signal from getting stuck at LOW
    > This modification only solves the case when ready signal gets stuck after sending 0xDC000000 to TTN through SPI
