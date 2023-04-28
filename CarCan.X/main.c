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

/*
                         Main application
 */
void main(void) {
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

    CanInit(1, CAN_250K_500K);
    initialiseCar(&myCar);


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
    uint8_t txd[8] = {0, 1, 2, 3, 4, 5, 6, 7};
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
    while (CanReceive(&rxObj, rxd) != 0) {
    }

    myCar.carId = rxd[0];
    fObj.ID = rxd[0];
    mObj.MID = 0x00F;
    CanSetFilter(CAN_FILTER0, &fObj, &mObj);
    sendTime(&myCar, myCar.hours, myCar.minutes, 0);

    while (1) {
        // Add your application code
        CAN_RX_MSGOBJ rxObj;
        updateCarstate(&myCar, rxObj);
        if (myCar.count50Ms == 5) {
            compaereAndUptadeCar(&myCar);
            myCar.count50Ms = 0;
        }
        if(myCar.count10Ms == 1){
            calculateKm(&myCar);
            myCar.count10Ms = 0;
        }
        if(myCar.count30Ms == 3){
            myCar.count30Ms = 0;
            if (myCar.driveMode != 0) {
                static uint8_t lastLeft = 0;
                static int8_t lastRight = 0;
                if (myCar.sensLeft < myCar.sensRight) {
                    lastRight += 7;
                    sendAutoSteering(&myCar, lastRight, 1);
                    lastLeft = 0;
                } else if (myCar.sensLeft > myCar.sensRight) {
                    lastLeft -= 7;
                    sendAutoSteering(&myCar, lastLeft, 1);
                    lastRight = 0;
                } else {
                    sendAutoSteering(&myCar, 0, 1);
                    lastLeft = 0;
                    lastRight = 0;
                }
            }
        }
        if(myCar.count1sec == 100){
            if(myCar.colon == 1){
                sendTime(&myCar,myCar.hours,myCar.minutes,0);
            } else {
                if(myCar.seconds == 59){
                    myCar.seconds = 0;
                    if(myCar.minutes == 59){
                        myCar.minutes = 0;
                        if(myCar.hours == 23){
                            myCar.hours = 0;
                        } else{
                            myCar.hours++;
                        }
                    } else {
                        myCar.minutes++;
                    }
                } else{
                    myCar.seconds++;
                }
                sendTime(&myCar,myCar.hours,myCar.minutes,1);
            }
            myCar.count1sec = 0;
        }

    }
}
/**
 End of File
 */