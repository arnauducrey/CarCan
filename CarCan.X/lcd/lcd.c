/*
 * File:   lcd.c
 * Author: pascal.sartoret
 *
 * Created on 16. décembre 2020, 14:04
 */

#define _XTAL_FREQ 64000000L

#include <xc.h>
#include <string.h>
#include "lcd.h"
#include "ssd1963_cmd.h"
#define WR_STROBE()   LCD_WR = 0; LCD_WR = 1; 
uint8_t lcd = 0;

#define REG_COUNT   18
void LCD_TouchRead(TS_STRUCT *touch)
{
    uint8_t regs[REG_COUNT];
    //--------------------------------------------------------------------------
    SEN1 = 1;               // start condition
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0;    
    //--------------------------------------------------------------------------
    SSP1BUF = (TSC_I2C_ADDRESS << 1);  // address of controller to write
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0;
    //--------------------------------------------------------------------------
    SSP1BUF = (0x02);       // address to read after
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0;    
    //--------------------------------------------------------------------------
    PEN1 = 1;               // stop condition
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0;    
    //**************************************************************************
    SEN1 = 1;               // start condition
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0;    
    //--------------------------------------------------------------------------
    SSP1BUF = (TSC_I2C_ADDRESS << 1) | 1;  // address of controller to read
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0; 
    //--------------------------------------------------------------------------
    for(uint8_t i=0;i<REG_COUNT;i++)
    {
        RCEN1 = 1;               // read one byte
        while(SSP1IF == 0){}    // wait end of operation
        SSP1IF = 0;       
        //----------------------------------------------------------------------
        regs[i] = SSP1BUF;
        //----------------------------------------------------------------------
        if(i == (REG_COUNT-1))
        {
            ACKDT1 = 1;
        }
        else
        {
            ACKDT1 = 0;
        }
        ACKEN1 = 1;               // not acknowledge condition
        while(SSP1IF == 0){}    // wait end of operation
        SSP1IF = 0;        
    }
    //--------------------------------------------------------------------------
    PEN1 = 1;               // stop condition
    while(SSP1IF == 0){}    // wait end of operation
    SSP1IF = 0;    
    //--------------------------------------------------------------------------
    touch->status = regs[0];
    touch->posX = ((uint16_t)(regs[1] & 0x0F) << 8) | regs[2];
    touch->posY = ((uint16_t)(regs[3] & 0x0F) << 8) | regs[4];
    touch->weight = regs[5];
    touch->posX = 800 - touch->posX;    // swap x coordiantes 
    touch->posY = 480 - touch->posY;    // swap y coordiantes 

    touch->posX2 = ((uint16_t)(regs[7] & 0x0F) << 8) | regs[8];
    touch->posY2 = ((uint16_t)(regs[9] & 0x0F) << 8) | regs[10];
    touch->weight2 = regs[11];
    touch->posX2 = 800 - touch->posX2;    // swap x coordiantes 
    touch->posY2 = 480 - touch->posX2;    // swap y coordiantes 

    touch->posX3 = ((uint16_t)(regs[13] & 0x0F) << 8) | regs[14];
    touch->posY3 = ((uint16_t)(regs[15] & 0x0F) << 8) | regs[16];
    touch->weight3 = regs[17];
    touch->posX3 = 800 - touch->posX3;    // swap x coordiantes 
    touch->posY3 = 480 - touch->posY3;    // swap y coordiantes 
    if((touch->status & 0x01) == 1)
    {
        touch->pressed = 1;
        touch->released = 0;
    }
    else if(touch->pressed == 1)
    {
        touch->released = 1;
    }
}

void LCD_TouchInit(void)
{
    TRISLbits.TRISL0 = 0;   // reset as output
    LATLbits.LATL0 = 0;     // reset IC
    RPINR18_19 = 0xF4;      // IOC on pin RC5 (pin16)
    IOCIE = 1;
    IOCN = 1;
    IOCIF = 0;
    PEIE = 1;
    GIE = 1;
    
    LATLbits.LATL0 = 1;     // unreset IC
    SSP1CON1 = 0x28;        // I2C master mode on RC3, RC4
    SSP1ADD = 39;           // works at 400 kHz
    SSP1IF = 0;    
    
}

int8_t Lcd_Init(uint8_t lcdChoice) 
{
    if(lcdChoice == LCD_2x16)
    {
        lcd = lcdChoice;
        //--------------------------------------------------------------------------
        // define GPIO for LCD
        DIR_LCD_RS = 0;         // RS is an output
        LCD_RS = 0;             // command mode
        DIR_LCD_CS = 0;         // chip select is an output
        LCD_CS = 0;             // do not select the LCD
        LCD_DATA_L_DIR = 0x0F;  // data port is 4 bits output
        LCD_2x16_WriteCmd(0x38);
        LCD_2x16_WriteCmd(0x2B);
        LCD_2x16_WriteCmd(0x2B);
        LCD_2x16_WriteCmd(0x0C);
        LCD_2x16_WriteCmd(0x14);
//        LCD_2x16_WriteCmd(0x01);    // clear display 
        __delay_ms(2);
        LCD_2x16_WriteCmd(0x06);
        __delay_ms(100);        
        return 0;
    }
    else if(lcdChoice == LCD_800x480)
    {
        if(lcd == LCD_2x16)     // was previously on 2x16 selected lock address
        {
            DIR_LCD_RS = 0;         // RS is an output
            LCD_RS = 0;             // command mode
            DIR_LCD_CS = 0;         // chip select is an output
            LCD_CS = 0;             // do not select the LCD
            LCD_DATA_L_DIR = 0x0F;  // data port is 4 bits output
            LCD_2x16_WriteCmd(0x38);
            LCD_2x16_WriteCmd(0x2B);
            LCD_2x16_WriteCmd(0x2B);
            LCD_2x16_WriteCmd(0x40);    // set CG RAM address                   
        }
        lcd = lcdChoice;
        //--------------------------------------------------------------------------
        // define GPIO for LCD
        DIR_LCD_RST = 0;        // reset is an output
        LCD_RST = 0;            // activate LCD reset
        DIR_LCD_CS = 0;         // chip select is an output
        LCD_CS = 1;             // do not select the LCD
        DIR_LCD_DC = 0;         // Data / control is an output
        LCD_DC = 0;             // default 0 is command
        DIR_LCD_RD = 0;         // LCD read is an output
        LCD_RD = 1;             // default no read
        DIR_LCD_WR = 0;         // LCD write is an output
        LCD_WR = 1;             // default no write
        LCD_DATA_L_DIR = 0;     // data port is default output
        LCD_DATA_H_DIR = 0;     // data port is default output
        LCD_RST = 1;            // deactivate LCD reset
        //
        __delay_ms(100);
        //--------------------------------------------------------------------------
        // initialisation based on tft8.c _reset_ssd1963_controller_800x480
        LCD_WriteCmd(SSD1963_CMD_SOFT_RESET);
        LCD_WriteCmd(SSD1963_CMD_SOFT_RESET);
        LCD_WriteCmd(SSD1963_CMD_SOFT_RESET);
        // configure pll
        LCD_WriteCmd(SSD1963_CMD_SET_PLL_MN);
        LCD_WriteData(35);
        LCD_WriteData(2);
        LCD_WriteData(0x54);
        // start pll
        LCD_WriteCmd(SSD1963_CMD_SET_PLL);
        LCD_WriteData(0x01);
        __delay_us(200);
        // lock pll
        LCD_WriteCmd(SSD1963_CMD_SET_PLL);
        LCD_WriteData(0x03);
        // set lcd mode
        LCD_WriteCmd(SSD1963_CMD_SET_LCD_MODE);
        LCD_WriteData(0x28);
        LCD_WriteData(0x20);
        LCD_WriteData(0x03);
        LCD_WriteData(0x1F);
        LCD_WriteData(0x01);
        LCD_WriteData(0xDF);
        LCD_WriteData(0x00);    
        LCD_WriteCmd(SSD1963_CMD_SET_PIXEL_DATA_INTERFACE);
        LCD_WriteData(0x03);    // set 16 bits interface (565)
        // set pixel data
        LCD_WriteCmd(0x3A); // ??
        LCD_WriteData(0x60);
      //SET PCLK freq=10MHz. Pixel clock frequency.
        LCD_WriteCmd(SSD1963_CMD_SET_LSHIFT_FREQ);
        LCD_WriteData(0x04);
        LCD_WriteData(0x3A);
        LCD_WriteData(0x60);
        LCD_WriteCmd(SSD1963_CMD_SET_HORI_PERIOD);
        LCD_WriteData(0x04);
        LCD_WriteData(0x20);
        LCD_WriteData(0x00);
        LCD_WriteData(0x30);
        LCD_WriteData(0x30);
        LCD_WriteData(0x00);
        LCD_WriteData(0x00);
        LCD_WriteData(0x00);
        LCD_WriteCmd(SSD1963_CMD_SET_VERT_PERIOD);
        LCD_WriteData(0x02);
        LCD_WriteData(0x0D);
        LCD_WriteData(0x00);
        LCD_WriteData(0x16);
        LCD_WriteData(0x04);
        LCD_WriteData(0x00);
        LCD_WriteData(0x00);
        LCD_WriteCmd(SSD1963_CMD_SET_TEAR_ON);
        LCD_WriteData(0x00);
        
     // Rotation dependent.
        LCD_WriteCmd(SSD1963_CMD_SET_ADDRESS_MODE);
        LCD_WriteData(0x03);
        LCD_WriteCmd(SSD1963_CMD_SET_DISPLAY_ON);

        LCD_Backlight(255);

        //--------------------------------------------------------------------------
        LCD_TouchInit();
        return 0;
    }
    lcd = 0;
    return -1;      // no LCD defined
}

void LCD_ButtonCreate(uint16_t posX, uint16_t posY,
                      uint16_t width, uint16_t height,
                      uint16_t txtColor, uint16_t bgColor,
                      const uint8_t * textOrBmp, FONT_INFO * font,
                      void * fpPress, void * fpReleased, void * fpOut,
                      btn_t * button,
                      uint8_t index)
{
  button->posX = posX;
  button->posY = posY;
  button->height = height;
  button->width = width;
  button->txtColor = txtColor;
  button->bgColor = bgColor;
  button->text = textOrBmp;
  button->font = font;
  button->fpPress = fpPress;
  button->fpReleased = fpReleased;
  button->fpOut = fpOut;
  button->index = index;
  button->pressed = 0;
}
void LCD_ButtonUpdate(btn_t * button)
{
  uint16_t x1,x2,y1,y2;
  uint16_t colorOut, colorIn;

  x1 = button->posX;
  x2 = button->posX + button->width;
  y1 = button->posY;
  y2 = button->posY + button->height;

  if(button->pressed == 0)  // button not pressed
  {
    colorOut = button->txtColor;
    colorIn = button->bgColor;
  }
  else
  {

    colorOut = LCD_GetPixel(x1,y2+1);
    colorIn = button->txtColor;
  }
  if(button->bgColor != button->txtColor)
  {

    LCD_DrawRect(x1+3, y2+1, x2+3, y2+3, 1, colorOut); // bottom
    LCD_DrawRect(x2+1, y1+3, x2+3, y2+3, 1, colorOut); // right
    LCD_DrawRect(x1+1, y1+1, x2-1, y1+2, 1, colorIn);  // top
    LCD_DrawRect(x1+1, y1+1, x1+2, y2-1, 1, colorIn);  // left
  }
}

void LCD_ButtonDraw(btn_t * button)
{
  uint16_t x1,x2,y1,y2;
  uint16_t xText,yText;
  uint8_t i, tempChar;
  uint16_t width=0;
  BMP_STRUCT *bmpPtr;

  x1 = button->posX;
  x2 = button->posX + button->width;
  y1 = button->posY;
  y2 = button->posY + button->height;
  if(button->bgColor != button->txtColor)
  {
    LCD_DrawRect(x1, y1, x2, y2, 1, button->bgColor);
    LCD_DrawRect(x1, y1, x2, y2, 0, button->txtColor);
  }
  //----------------------------------------------------------------------------
  // check if is a bitmap button
  //----------------------------------------------------------------------------
  if((button->text[0] == 'B')&&(button->text[1] == 'M'))
  {
    bmpPtr = (BMP_STRUCT *)button->text;
    yText =  button->height / 2 + button->posY - (bmpPtr->biHeight / 2);
    xText =   button->width / 2 + button->posX - (bmpPtr->biWidth / 2);
   // SAP REMOVED LCD_Bitmap((const uint8_t *)bmpPtr,xText,yText);
  }
  //----------------------------------------------------------------------------
  // is a text button
  //----------------------------------------------------------------------------
  else
  {
    yText =  button->height / 2 + button->posY - (button->font->height / 2);
    for(i=0;i<strlen(button->text);i++)
    {
      tempChar = (button->text[i]) - button->font->start_char;      // get char of table
      width += button->font->descriptor[tempChar].width+2;  // width of char
    }
    xText =   button->width / 2 + button->posX - (width / 2);
    LCD_DrawText(button->text, button->font,A_LEFT,
          xText, yText, button->txtColor,  button->bgColor);
  }
  LCD_ButtonUpdate(button);
}

uint8_t LCD_InButton(btn_t * button, uint16_t posX, uint16_t posY)
{
  //----------------------------------------------------------------------------
  if(posX < button->posX) // x check
  {
    return 0;             // not in button
  }
  //----------------------------------------------------------------------------
  if(posX > (button->posX + button->width)) // x check
  {
    return 0;             // not in button
  }
  //----------------------------------------------------------------------------
  if(posY < button->posY) // x check
  {
    return 0;             // not in button
  }
  //----------------------------------------------------------------------------
  if(posY > (button->posY + button->height)) // x check
  {
    return 0;             // not in button
  }
  return 1;               // in button !
}

void TSC_ButtonExecute(btn_t * button, TS_STRUCT * tsc, uint8_t index)
{
      if(tsc->pressed == 1)
      {
        if(LCD_InButton(button,tsc->posX,tsc->posY) == 1)
        // EVENT BUTTON PRESSED
        {
          if(button->fpPress != NULL)
            button->fpPress(button->index);
        }
        if(button->pressed == 0)
        {
          if(LCD_InButton(button,tsc->posX,tsc->posY) == 1)
          {
            button->pressed = 1;
            LCD_ButtonUpdate(button);

          }
        }
        else
        {
          if(LCD_InButton(button,tsc->posX,tsc->posY) == 0)
          {
            if(button->fpOut != NULL)
                button->fpOut(button->index);
            button->pressed = 0;
            LCD_ButtonUpdate(button);
          }
        }
      }
      if(tsc->released == 1)
      {
        if(LCD_InButton(button,tsc->posX,tsc->posY) == 1)
        {
          tsc->released = 0;
          button->pressed = 0;
          LCD_ButtonUpdate(button);
          if(button->fpReleased != NULL)
            button->fpReleased(button->index);
          // EVENT BUTTON RELEASED

        }
      }
}

void LCD_Backlight(uint8_t intenisty)
{
    if(lcd != LCD_800x480)
    {
        return;
    }
    LCD_WriteCmd(SSD1963_CMD_SET_DISPLAY_OFF);
    LCD_WriteCmd(SSD1963_CMD_SET_PWM_CONF);
    LCD_WriteData(0x06);
    LCD_WriteData(intenisty);
    LCD_WriteData(0x01);
    LCD_WriteData(0xFF);
    LCD_WriteData(0x00);
    LCD_WriteData(0x01);
    LCD_WriteCmd(SSD1963_CMD_SET_DISPLAY_ON);
}

void LCD_SetPixel(uint16_t posX,uint16_t posY, uint16_t color)
{
  //----------------------------------------------------------------------------
  LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);            // column address set
  LCD_WriteData(posX>>8);                  // start address
  LCD_WriteData(posX);                     //
  LCD_WriteData((LCD_WIDTH-1) >> 8);       // end address
  LCD_WriteData((LCD_WIDTH-1));            //
  //----------------------------------------------------------------------------
  LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);              // page address set
  LCD_WriteData(posY>>8);                  // start address
  LCD_WriteData(posY);                     // is 0
  LCD_WriteData((LCD_HEIGHT-1) >> 8);      // end address
  LCD_WriteData((LCD_HEIGHT-1));           //
  //----------------------------------------------------------------------------
  LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);              // write data command
  LCD_CS = 0;             // chip select
  LCD_DC = 1;             // select data mode
  LCD_DATA_L = color;
  LCD_DATA_H = color >> 8;
  WR_STROBE();
  LCD_CS = 1;             // chip deselect
  
}

uint16_t LCD_GetPixel(uint16_t posX,uint16_t posY)
{
    uint16_t color;
  //----------------------------------------------------------------------------
  LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);            // column address set
  LCD_WriteData(posX>>8);                  // start address
  LCD_WriteData(posX);                     //
  LCD_WriteData((LCD_WIDTH-1) >> 8);       // end address
  LCD_WriteData((LCD_WIDTH-1));            //
  //----------------------------------------------------------------------------
  LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);              // page address set
  LCD_WriteData(posY>>8);                  // start address
  LCD_WriteData(posY);                     // is 0
  LCD_WriteData((LCD_HEIGHT-1) >> 8);      // end address
  LCD_WriteData((LCD_HEIGHT-1));           //
  //----------------------------------------------------------------------------
  LCD_WriteCmd(SSD1963_CMD_READ_MEMORY_START);              // write data command
  LCD_CS = 0;             // chip select
  LCD_DC = 1;             // select data mode
  LCD_RD = 0;
  color = LCD_DATA_L;
  color = color | ((uint16_t)LCD_DATA_H << 8);
  LCD_RD = 1;
  LCD_CS = 1;             // chip deselect
  
}

void LCD_DrawText(const uint8_t * msg, FONT_INFO * font, ALIGN align,
        uint16_t posX, uint16_t posY, uint16_t color, uint16_t bg_color)
{
  uint8_t	x,y,i;
  int8_t	tempChar;
  uint16_t offset;
  uint8_t width=0;
  uint8_t fontByte,fontPixel;
  uint16_t posYEnd, posXEnd, yOffset;

  #define INTER_CHAR  2

  for(i=0;i<strlen(msg);i++)
  {
    tempChar = (msg[i]) - font->start_char;      // get char of table
    width += font->descriptor[tempChar].width+2;  // width of char
  }
  if(align == A_RIGHT)
  {
    posX = posX - width;
  }
  else if (align == A_CENTER)
  {
    posX = posX - width / 2;
  }

  do
  {
    tempChar = (*msg) - font->start_char;      // get char of table
    offset = font->descriptor[tempChar].offset;// offset of char in table
    width = font->descriptor[tempChar].width;  // width of char

    //--------------------------------------------------------------------------
    // set working area as the character width & height
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
    LCD_WriteData(posX>>8);                    // start address
    LCD_WriteData(posX);                       // is X
    posXEnd = posX+width-1+INTER_CHAR;
    LCD_WriteData(posXEnd>>8);                 // end address
    LCD_WriteData(posXEnd);                    //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);                // page address set
    LCD_WriteData(posY>>8);                    // start address
    LCD_WriteData(posY);                       // is Y
    posYEnd = posY + font->height-1;
    LCD_WriteData(posYEnd>>8);                 // end address
    LCD_WriteData(posYEnd);                    // 
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data

    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode   
    for(y=0;y<font->height;y++)            // for the height
    {
      yOffset = offset + (y * ((width+7)/8));
      for(x=0;x<width;x++)              // for the width
      {
        fontByte = font->bitmap[yOffset + (x/8)];
        fontPixel = fontByte & (0x80 >> (x%8)); // get pixel in char
        if(fontPixel != 0)
        {
          LCD_DATA_L = color;
          LCD_DATA_H = color >> 8;
          WR_STROBE();
        }
        else
        {
          LCD_DATA_L = bg_color;
          LCD_DATA_H = bg_color >> 8;
          WR_STROBE();
        }
      }
      for(x=0;x<INTER_CHAR;x++)
      {
        LCD_DATA_L = bg_color;
        LCD_DATA_H = bg_color >> 8;
        WR_STROBE();
      }
    }
    LCD_CS = 1;             // chip deselect
    posX = posX + 2 + width;              // next char position
    msg++;                                // next char in string
 }while(*msg != 0);                       // until end of string
}

void LCD_DrawRect(uint16_t posX1,uint16_t posY1, uint16_t posX2,
		uint16_t posY2,uint8_t fill,uint16_t color)
{
  uint16_t i;
  uint16_t j;
  uint16_t temp;
  uint8_t colorH,colorL;

  colorH = color>>8;
  colorL = color;
  //----------------------------------------------------------------------------
  // check swap left-right, top-bottom
  //----------------------------------------------------------------------------
  if(posX1 > posX2)
  {
    temp = posX1;
    posX1 = posX2;
    posX2 = temp;
  }
  if(posY1 > posY2)
  {
    temp = posY1;
    posY1 = posY2;
    posY2 = temp;
  }
  //----------------------------------------------------------------------------
  if(fill==1)								// if the rectangle is filled
  {
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
    LCD_WriteData(posX1>>8);                   // start address
    LCD_WriteData(posX1);                      //
    LCD_WriteData(posX2>>8);                   // end address
    LCD_WriteData(posX2);                      //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);                // page address set
    LCD_WriteData(posY1>>8);                   // start address
    LCD_WriteData(posY1);                      // is 0
    LCD_WriteData((LCD_HEIGHT-1) >> 8);                   // end address
    LCD_WriteData((LCD_HEIGHT-1));             //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    while(PORTEbits.RE7 == 0){}
    for(i=posX1;i<=posX2;i++)
    {
      for(j=posY1;j<=posY2;j++)
      {                                   // write a filled rectangle
        LCD_DATA_L = colorL;
        LCD_DATA_H = colorH;
        WR_STROBE();
      }
    }
    LCD_CS = 1;             // chip select
  }
  //----------------------------------------------------------------------------
  else                                  // if the rectangle is not filled
  {
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
    LCD_WriteData(posX1>>8);                 // start address is X1
    LCD_WriteData(posX1);                    //
    LCD_WriteData(posX2>>8);                 // end address is X2
    LCD_WriteData(posX2);                    //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);                // page address set
    LCD_WriteData(posY1>>8);                 // start address
    LCD_WriteData(posY1);                    // is Y1
    LCD_WriteData((LCD_HEIGHT-1) >> 8);      // end address
    LCD_WriteData((LCD_HEIGHT-1));           //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    for(i=posX1;i<=posX2;i++)           // write one horizontal line
    {
        LCD_DATA_L = colorL;
        LCD_DATA_H = colorH;
        WR_STROBE();
    }
    LCD_CS = 1;             // chip deselect
    //--------------------------------------------------------------------------
//    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
//    LCD_WriteData(posX1>>8);                 // start address is X1
//    LCD_WriteData(posX1);                    //
//    LCD_WriteData(posX2>>8);                 // end address is X2
//    LCD_WriteData(posX2);                    //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);                // page address set
    LCD_WriteData(posY2>>8);                 // start address
    LCD_WriteData(posY2);                    // is 0
    LCD_WriteData((LCD_HEIGHT-1) >> 8);      // end address
    LCD_WriteData((LCD_HEIGHT-1));           //
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    for(i=posX1;i<=posX2;i++)           // write second horizontal line
    {
        LCD_DATA_L = colorL;
        LCD_DATA_H = colorH;
        WR_STROBE();
    }
    LCD_CS = 1;             // chip deselect
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
    LCD_WriteData(posX1>>8);                 // start address is X1
    LCD_WriteData(posX1);                    //
    LCD_WriteData(posX1>>8);                 // end address is same (width = 1)
    LCD_WriteData(posX1);                    //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);                // page address set
    LCD_WriteData(posY1>>8);                 // start address
    LCD_WriteData(posY1);                    // is 0
    LCD_WriteData((LCD_HEIGHT-1) >> 8);      // end address
    LCD_WriteData((LCD_HEIGHT-1));           //
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    for(i=posY1;i<=posY2;i++)           // write one vertical line
    {
        LCD_DATA_L = colorL;
        LCD_DATA_H = colorH;
        WR_STROBE();
    }
    LCD_CS = 1;             // chip deselect
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
    LCD_WriteData(posX2>>8);                 // start address is X2
    LCD_WriteData(posX2);                    //
    LCD_WriteData(posX2>>8);                 // end address is same (width = 1)
    LCD_WriteData(posX2);                    //
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    for(i=posY1;i<=posY2;i++)           // write second vertical line
    {
        LCD_DATA_L = colorL;
        LCD_DATA_H = colorH;
        WR_STROBE();
    }
    LCD_CS = 1;             // chip deselect
  }
}

uint16_t RGB2LCD(uint8_t * colorTableEntry)
{
    uint16_t red,green,blue;            // three colors
    uint16_t color;                     // LCD color 16 bits

    red = colorTableEntry[2] >> 3;      // get 5 bits red information
    green = colorTableEntry[1] >> 2;    // get 6 bits green information
    blue = colorTableEntry[0] >> 3;     // get 5 bits blue information
    color = (red << 11) | (green << 5) | (blue << 0); // concatene
    return color;
}
uint8_t LCD_Bitmap(const uint8_t * bmpPtr, uint16_t posX, uint16_t posY)
{
  uint32_t bfOffBits;
  uint16_t biWidth, biHeight;
  uint8_t * colorTablePtr;
  uint8_t endOfImage;
  uint16_t i,curXPos,curYPos;
  uint8_t  pixelCount;
  uint16_t color;
  uint16_t jumpColor;
  uint16_t jumpCount;
  uint16_t newY;
  BMP_STRUCT * strPtr;

  strPtr = (BMP_STRUCT *)bmpPtr;
  //----------------------------------------------------------------------------
  bfOffBits 	= strPtr->bfOffBits;    // get offset to start of image
  biWidth	= strPtr->biWidth;          // get width in pixel
  biHeight	= strPtr->biHeight;       // get height in pixel
  if((strPtr->biBitCount != 8) ||     // check 8 bits per pixel
     (strPtr->biCompression != BI_RLE_8))    // check compression RLE_8
  {
    return 1;                         // error code return
  }
  if(((posX + biWidth) > (LCD_WIDTH)) ||  // check width
     ((posY + biHeight) > (LCD_HEIGHT)))  // check height
  {
    return 2;                               // error code return
  }
  //----------------------------------------------------------------------------
// invert order ?
        LCD_WriteCmd(SSD1963_CMD_SET_ADDRESS_MODE);
        LCD_WriteData(0x83);

  colorTablePtr = (uint8_t *)bmpPtr + BI_COLOR_TBL;
  bmpPtr += bfOffBits;
  endOfImage = 0;
  jumpColor = RGB2LCD(colorTablePtr);
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);              // column address set
    LCD_WriteData(posX>>8);                 // start address is X1
    LCD_WriteData(posX);                    //
    posX = posX + biWidth-1;
    LCD_WriteData(posX>>8);                 // end address is X2
    LCD_WriteData(posX);                    //
    //--------------------------------------------------------------------------
    newY = posY;
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);                // page address set
    LCD_WriteData(newY>>8);                 // start address
    LCD_WriteData(newY);                    // is Y1
    newY = newY + biHeight-1;
    LCD_WriteData(newY>>8);                  // end address
    LCD_WriteData(newY);                     //
    //--------------------------------------------------------------------------
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);                // write data  
  //----------------------------------------------------------------------------
  LCD_CS = 0;             // chip select
  LCD_DC = 1;             // select data mode
  curXPos = 0;
  curYPos = 0;
  do
  {
    if(bmpPtr[0] == 0)                // escape char in RLE data
    {
      switch (bmpPtr[1])              // check second parameter
      {
        case 0:                       // next line
          bmpPtr += 2;                // increment image pointer
          for(i=0;i<biWidth - curXPos;i++)
          {
            LCD_DATA_L = jumpColor;
            LCD_DATA_H = jumpColor >> 8;
            WR_STROBE();
          }
          curXPos = 0;
          curYPos++;
        break;
        case 1:                       // end of image
          for(i=0;i<biWidth - curXPos;i++)  // terminate line if any
          {
            LCD_DATA_L = jumpColor;
            LCD_DATA_H = jumpColor >> 8;
            WR_STROBE();
          }
          for(i=0;i<(biHeight - curYPos -1)*biWidth;i++) // and others lines
          {
            LCD_DATA_L = jumpColor;
            LCD_DATA_H = jumpColor >> 8;
            WR_STROBE();
          }
          endOfImage = 1;             // set flag
        break;
        case 2:                       // jump to (entry 0 for color is used)
          jumpCount = bmpPtr[2] + (bmpPtr[3]*biWidth);
          for(i=0;i<jumpCount;i++)
          {
            curXPos++;
            LCD_DATA_L = jumpColor;
            LCD_DATA_H = jumpColor >> 8;
            WR_STROBE();
          }
          curYPos = curYPos + (jumpCount / biWidth);
          curXPos = curXPos % biWidth;
          bmpPtr += 4;
        break;
        default:                      // not coded pixels
          pixelCount = bmpPtr[1];     // get number of following pixels
          for(i=0;i < pixelCount;i++)
          {
            color = RGB2LCD(colorTablePtr+(bmpPtr[2+i]*4));
            curXPos++;
            LCD_DATA_L = color;
            LCD_DATA_H = color >> 8;
            WR_STROBE();
          }
          if(pixelCount < 255)
          {
            pixelCount = (pixelCount+1) & 0xFE; // to upper even (RLE defs)
            bmpPtr+=pixelCount+2;       // increment image pointer
          }
          else
          {
              bmpPtr += 258;
          }
        break;
      }
    }
    else                              // coded pixels
    {
      pixelCount = bmpPtr[0];         // number of same pixels
      color = RGB2LCD(colorTablePtr+(bmpPtr[1]*4));
      for(i=0;i < pixelCount;i++)
      {
        curXPos++;
        LCD_DATA_L = color;
        LCD_DATA_H = color >> 8;
        WR_STROBE();
      }
      bmpPtr+=2;                      // increment image pointer
    }
  }while(endOfImage == 0);            // until end of image
// restore direction ?
  LCD_CS = 1;             // chip unselect
        LCD_WriteCmd(SSD1963_CMD_SET_ADDRESS_MODE);
        LCD_WriteData(0x03);
  
  return 0;                           // return no error
}

void LCD_Fill(uint16_t color)
{
    if(lcd != LCD_800x480)
    {
        return;
    }    
    uint16_t startColumn=0;
    uint16_t endColumn = LCD_WIDTH - 1;
    uint16_t startRaw=0;
    uint16_t endRaw = LCD_HEIGHT - 1;
    uint32_t pixelCount = LCD_WIDTH * LCD_HEIGHT;
    uint32_t value1 = ((color & 0xF800) >> 8) & 0x00FF;
    uint32_t value2 = ((color & 0x07E0 ) >> 3) & 0x00FF;
    uint32_t value3 = ((color & 0x001F ) << 3) & 0x00FF;    
    
    LCD_WriteCmd(SSD1963_CMD_SET_COLUMN_ADDRESS);
    LCD_WriteData(startColumn >> 8);
    LCD_WriteData(startColumn);
    LCD_WriteData(endColumn >> 8);
    LCD_WriteData(endColumn);
    LCD_WriteCmd(SSD1963_CMD_SET_PAGE_ADDRESS);
    LCD_WriteData(startRaw >> 8);
    LCD_WriteData(startRaw);
    LCD_WriteData(endRaw >> 8);
    LCD_WriteData(endRaw);
    LCD_WriteCmd(SSD1963_CMD_WRITE_MEMORY_START);
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    while(pixelCount--)
    {
        LCD_DATA_L = color;
        LCD_DATA_H = color >> 8;
        WR_STROBE();
    }
    LCD_CS = 1;             // chip deselect
}

void LCD_WriteCmd(uint8_t command)
{
    LCD_CS = 0;             // chip select
    LCD_DC = 0;             // select command mode
    LCD_DATA_L = command;   // place data
    WR_STROBE();            // toggle write pin
    LCD_CS = 1;             // chip deselect
}

void LCD_WriteData(uint8_t data)
{
    LCD_CS = 0;             // chip select
    LCD_DC = 1;             // select data mode
    LCD_DATA_L = data;      // place data
    WR_STROBE();            // toggle write pin
    LCD_CS = 1;             // chip deselect
}

void LCD_2x16_WriteCmd(uint8_t command)
{
    LCD_RS = 0;             // command mode
    LCD_DATA_L = command;   // place data
    LCD_CS = 1;             // chip select
    __delay_us(2);
    LCD_CS = 0;             // chip deselect
    __delay_us(2);
    LCD_DATA_L = (command << 4);   // place data
    LCD_CS = 1;             // chip select
    __delay_us(2);
    LCD_CS = 0;             // chip deselect
    __delay_us(40);
}
void LCD_2x16_WriteData(uint8_t command)
{
    LCD_RS = 1;             // command mode
    LCD_DATA_L = command;   // place data
    LCD_CS = 1;             // chip select
    __delay_us(2);
    LCD_CS = 0;             // chip deselect
    __delay_us(2);
    LCD_DATA_L = (command << 4);   // place data
    LCD_CS = 1;             // chip select
    __delay_us(2);
    LCD_CS = 0;             // chip deselect
    __delay_us(44);
}

void LCD_2x16_WriteMsg(unsigned char * msg, uint8_t line)
{
    LCD_2x16_WriteCmd(0x80 | (line << 6));
    do
    {
        LCD_2x16_WriteData(*msg);
        msg++;
    }while(*msg != 0);
}