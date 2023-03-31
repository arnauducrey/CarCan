/**
  Generated Main Source File

  Company:
    Microchip Technology Inc.

  File Name:
    main.c

  Summary:
    This is the main file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F97J60
        Driver Version    :  2.00
 */

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 

    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */
    
#include "mcc_generated_files/mcc.h"
#include "can_defines.h"
#include "can.h" 
#include "car.h"


CARSTATE myCar;
CARSTATE myOldCar;

/*
                         Main application
 */
void main(void)
{
    // Initialize the device
    SYSTEM_Initialize();

    // If using interrupts in PIC18 High/Low Priority Mode you need to enable the Global High and Low Interrupts
    // If using interrupts in PIC Mid-Range Compatibility Mode you need to enable the Global and Peripheral Interrupts
    // Use the following macros to:

    // Enable the Global Interrupts
    INTERRUPT_GlobalInterruptEnable();

    // Disable the Global Interrupts
    //INTERRUPT_GlobalInterruptDisable();

    // Enable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptEnable();

    // Disable the Peripheral Interrupts
    //INTERRUPT_PeripheralInterruptDisable();
    
    CanInit(1,CAN_250K_500K);
    initialiseCar(&myCar);
    initialiseCar(&myOldCar);

    CAN_FILTEROBJ_ID fObj;
    fObj.ID = 0xFF;
    fObj.SID11 = 0;
    fObj.EXIDE = 0;
    CAN_MASKOBJ_ID mObj;
    mObj.MID = 0x7FF;
    mObj.MSID11 = 0;
    mObj.MIDE = 1;
    CanSetFilter(CAN_FILTER0, &fObj, &mObj);
    
    
    
    CAN_TX_MSGOBJ txObj;
    uint8_t txd[8] = {0,1,2,3,4,5,6,7};
    txObj.bF.id.ID = 0xFF;
    txObj.bF.ctrl.DLC = CAN_DLC_0;
    txObj.bF.ctrl.RTR = 1;
    txObj.bF.id.SID11 = 0;
    txObj.bF.ctrl.FDF = 0;
    txObj.bF.ctrl.IDE = 0;
    txObj.bF.ctrl.BRS = 0; 
    txObj.bF.ctrl.ESI = 0;
    CanSend(&txObj, txd);
    //Set filter
    CAN_RX_MSGOBJ rxObj; 
    uint8_t rxd[8];
    while(CanReceive(&rxObj, rxd) != 0) {}

    myCar.carId = rxd[0];
    fObj.ID = rxd[0];
    mObj.MID = 0x00F;
    CanSetFilter(CAN_FILTER0, &fObj, &mObj);
    //TMR0_SetInterruptHandler(compaereAndUptadeCar);
    while (1)
    {
        // Add your application code
        CAN_RX_MSGOBJ rxObj; 
        updateCarstate(&myCar,rxObj);
        if(tenMs == 2){
            compaereAndUptadeCar(&myCar, &myOldCar);
            tenMs = 0;
            //myOldCar = myCar;
        }
        
        //compaereAndUptadeCar(&myCar, &myOldCar);
    }
}
/**
 End of File
*/