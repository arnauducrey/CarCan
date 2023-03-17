/*
 * File:   spi.c
 * Author: pascal.sartoret
 *
 * Created on 8. january 2021
 */

//------------------------------------------------------------------------------
// interrupts usage:
// mikroBus 1 : INT0
// mikrobus 2 : INT1 or IOC1 or IOC5 (to select with PPS lite)
// mikrobus 3 : IOC4 or IOC0/conflict with touch (to select with PPS lite)
// mikrobus 4 : INT3 or IOC3 or IOC7 (to select with PPS lite)
// mikrobus 5 : INT2 or IOC2 or IOC6 (to select with PPS lite)
//------------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  // inits SSP2 for SPI communication (mikcro 1,2,3) or SSSP1 (mikro 4,5)
  // caution, SSP1 is already used for the touchscreen
  // ---------------------------------------------------------------------------

#include <xc.h>

#include <stdint.h>

volatile uint8_t * ptrCS_SPI;
uint8_t csBitNr;

uint8_t  mikroNr=0;
int8_t SpiInit(uint8_t mikroBusNr)
{
  if((mikroBusNr > 5) | (mikroBusNr < 1))
    return -1;
  mikroNr = mikroBusNr;
  // ---------------------------------------------------------------------------
  // inits pins for communication
  // ---------------------------------------------------------------------------
  switch(mikroBusNr)
  {
    case 1:
      RPOR26_27bits.RPO26R = 5;   // assign SKC2 to pin R26 (RD6)
      RPOR24_25bits.RPO24R = 4;   // assign MOSI2 to pin R24 (RD4)
      RPINR12_13bits.SDI2R = 6;   // assign MISO2 to pin R25 (RD5)
      ptrCS_SPI = &LATL;             // assign chip select
      csBitNr = 1;
      TRISL1 = 0;                 // set it as output
      TRISDbits.TRISD6 = 0;       // SKC2 is an output
      TRISDbits.TRISD4 = 0;       // MOSI2 is an output
      break;
    case 2:
      RPOR26_27bits.RPO26R = 5;   // assign SKC2 to pin R26 (RD6)
      RPOR24_25bits.RPO24R = 4;   // assign MOSI2 to pin R24 (RD4)
      RPINR12_13bits.SDI2R = 6;   // assign MISO2 to pin R25 (RD5)
      ptrCS_SPI = &LATL;             // assign chip select
      csBitNr = 2;
      TRISL2 = 0;                 // set it as output
      TRISDbits.TRISD6 = 0;       // SKC2 is an output
      TRISDbits.TRISD4 = 0;       // MOSI2 is an output
      break;
    case 3:
      RPOR26_27bits.RPO26R = 5;   // assign SKC2 to pin R26 (RD6)
      RPOR24_25bits.RPO24R = 4;   // assign MOSI2 to pin R24 (RD4)
      RPINR12_13bits.SDI2R = 6;   // assign MISO2 to pin R25 (RD5)
      ptrCS_SPI = &LATL;             // assign chip select
      csBitNr = 3;
      TRISL3 = 0;                 // set it as output
      TRISDbits.TRISD6 = 0;       // SKC2 is an output
      TRISDbits.TRISD4 = 0;       // MOSI2 is an output
      break;
    case 4:
      RPOR22_23bits.RPO23R = 3;   // assign SKC1 to pin R23 (RD3)
      RPOR20_21bits.RPO21R = 4;   // assign MOSI1 to pin R21 (RD1)
      RPINR8_9bits.SDI1R = 5;     // assign MISO1 to pin 20 (RD0)
      ptrCS_SPI = &LATL;             // assign chip select
      csBitNr = 4;
      TRISL4 = 0;                 // set it as output
      TRISDbits.TRISD3 = 0;       // SKC1 is an output
      TRISDbits.TRISD1 = 0;       // MOSI1 is an output
      break;
    case 5:
      RPOR22_23bits.RPO23R = 3;   // assign SKC1 to pin R23 (RD3)
      RPOR20_21bits.RPO21R = 4;   // assign MOSI1 to pin R21 (RD1)
      RPINR8_9bits.SDI1R = 5;     // assign MISO1 to pin 20 (RD0)
      ptrCS_SPI = &LATL;             // assign chip select
      csBitNr = 5;
      TRISL5 = 0;                 // set it as output
      TRISDbits.TRISD3 = 0;       // SKC1 is an output
      TRISDbits.TRISD1 = 0;       // MOSI1 is an output
      break;
  }
  // ---------------------------------------------------------------------------
  // inits MSSP for communication
  // ---------------------------------------------------------------------------
  if(mikroBusNr < 4)              // MSSP2 is used in this case
  {
    SSP2STATbits.CKE = 1;         // clock phase
    SSP2CON1bits.CKP = 0;         // clock is active high
    SSP2CON1bits.SSPEN = 1;       // enable serial port
    SSP2CON1bits.SSPM = 0b0000;   // SPI clock is 4MHz (Fosc/16)
  }
  else                            // MSSP1 (caution with touch)
  {
    SSP1STATbits.CKE = 1;         // clock phase
    SSP1CON1bits.CKP = 0;         // clock is active high
    SSP1CON1bits.SSPEN = 1;       // enable serial port
    SSP1CON1bits.SSPM = 0b0000;   // SPI clock is 4MHz (Fosc/16)
  }
  return 0;
}

int8_t SpiTransfer(uint8_t * txPtr, uint8_t * rxPtr, uint16_t size)
{
  volatile uint8_t dummy;
  *ptrCS_SPI &= ~(1 << csBitNr);                     // activate chip select
  while(size > 0)
  {
    if(mikroNr < 4)               // MSSP2 is used
    {
      //SSP2STATbits.BF = 0;        // clear transfer flag
      dummy = SSP2BUF;
      SSP2BUF = *txPtr;           // send data
      while(SSP2STATbits.BF == 0){}
      *rxPtr = SSP2BUF;           // get received data
    }
    else                          // MSSP1 is used
    {
      SSP2STATbits.BF = 0;         // clear transfer flag
      SSP1BUF = *txPtr;           // send data
      while(SSP1STATbits.BF == 0){}
      *rxPtr = SSP1BUF;           // get received data
    }
    txPtr++;                      // increment tx pointer
    rxPtr++;                      // increment rx pointer
    size--;                       // decrement bytes counter
  };
  *ptrCS_SPI |= (1 << csBitNr);                     // deactivate chip select
  return 0;
}