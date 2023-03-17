/************************************************************************//**
 * \file	main.c
 * \brief	Program main file for MODBUS controller
 ***************************************************************************/


/**
 * Include list
 */
#include <xc.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "can.h"
#include "lcd/lcd.h"
//To Be Completed


/**
 * Initialize clock, buttons, leds and interrupts
 */
void init_hw() {
    
    Lcd_Init(LCD_2x16);
    //To Be Completed
    
}
//#define _XTAL_FREQ  64000000L  //to check (only PIC18F97J94)

void main(void) {
	//OSCCON3 = 0;           //only PIC18F97J94 for 64MHz cpu clock (default is 32MHz)
    init_hw();              // Init hardware
                            //Call the init_hw function that sets up the start of our Picebs with base informations. Like general interrupts
    LCD_2x16_WriteMsg("Welcome to ...  ",0);      //Display on line 0
    LCD_2x16_WriteMsg("CAN-CAR demo !  ",1);      //Display on line 1

    //To Be Completed

    //--------------------------------------------------------------------------
    // Infinite loop
       while(true){
		   //To Be Completed
    }
}