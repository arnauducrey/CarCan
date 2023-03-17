/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * File:   
 * Author: 
 * Comments:
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef LCD_H
#define	LCD_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h>
//------------------------------------------------------------------------------
// colors definitions for LCD usage
//
// Bits are             0bRRRRRGGGGGGBBBBB
//------------------------------------------------------------------------------
#define     WHITE       0b1111111111111111
#define     ORANGE      0b1111101111000100
#define     BLACK       0b0000000000000000
#define     DARK_GRAY   0b0010000100000100
#define     GRAY        0b0110001100001100
#define     LIGHT_GRAY  0b1010010100010100
#define     RED         0b1111100000000000
#define     GREEN       0b0000011111100000
#define     DARK_GREEN  0b0000001111100000
#define     BLUE        0b0000000000011111
#define     LIGHTBLUE   0b0111101111111111
#define     CYAN        0b0000011111111111
#define     FUCHSIA     0b1111100000011111
#define     YELLOW      0b1111111111100000
#define     MEDIRIO_C   0b0000001111011001

#define LCD_WIDTH   800L
#define LCD_HEIGHT  480L
// 800x480 definitions
#define LCD_RST             LATLbits.LATL7
#define DIR_LCD_RST         TRISLbits.TRISL7
#define LCD_CS              LATJbits.LATJ3
#define DIR_LCD_CS          TRISJbits.TRISJ3
#define LCD_DC              LATJbits.LATJ2
#define DIR_LCD_DC          TRISJbits.TRISJ2
#define LCD_RD              LATJbits.LATJ1
#define DIR_LCD_RD          TRISJbits.TRISJ1
#define LCD_WR              LATGbits.LATG7
#define DIR_LCD_WR          TRISGbits.TRISG7
#define LCD_DATA_L_DIR      TRISK
#define LCD_DATA_L          PORTK
#define LCD_DATA_H_DIR      TRISH
#define LCD_DATA_H          PORTH
// 2x16 character definitions
#define LCD_RS              LATLbits.LATL7
#define DIR_LCD_RS          TRISLbits.TRISL7
#define LCD_CS              LATJbits.LATJ3
#define DIR_LCD_CS          TRISJbits.TRISJ3
#define LCD_DATA_L_DIR      TRISK
#define LCD_DATA_L          PORTK

#define TSC_I2C_ADDRESS 0x38
#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

#define LCD_2x16        1
#define LCD_800x480     2

typedef struct
{
    uint8_t width;
    uint16_t offset;
}FONT_CHAR_INFO;

typedef struct
{
    uint8_t height;
    uint8_t start_char;
    uint8_t end_char;
    const FONT_CHAR_INFO * descriptor;
    const uint8_t * bitmap;

}FONT_INFO;

typedef enum 
{
    A_LEFT,
    A_RIGHT,
    A_CENTER
}ALIGN;

/******************************************************************************/
/* Button descriptor                                                          */
/******************************************************************************/
typedef struct
{
  uint16_t posX;
  uint16_t posY;
  uint16_t width;
  uint16_t height;
  uint16_t txtColor;
  uint16_t bgColor;
  const uint8_t  *text;
  FONT_INFO * font;
  uint8_t  pressed;
  void    (*fpPress)(uint8_t);
  void    (*fpReleased)(uint8_t);
  void    (*fpOut)(uint8_t);
  uint8_t index;
}btn_t;
/******************************************************************************/
/* bmp-file information (File Header, Image Header)                           */
/******************************************************************************/
#define   BI_COLOR_TBL        54    // offset to color table (fixed)
#define   BI_RLE_8            1     // RLE 8 bits encoding mode
#define   BI_RLE_4            2     // RLE 4 bits encoding mode

typedef struct
{
	uint16_t bfType;                  // define BMP type of image
	uint32_t bfSize;                  // define size of file (bytes)
	uint16_t bfRvd1;                  // unused - 0
	uint16_t bfRvd2;                  // unused - 0
	uint32_t bfOffBits;               // offset to start of data pixel data
	uint32_t biSize;                  // size of header
	uint32_t biWidth;                 // image width in pixels
	uint32_t biHeight;                // image height in pixels
	uint16_t biPlanes;                // must be 1
	uint16_t biBitCount;              // bits per pixel
	uint32_t biCompression;           // 0 = uncompressed, 1=RLE
	uint32_t biSizeImage;             // image size (for compressed only)
	uint32_t biXPelsPerMeter;         // pref resol. in pixel/meter
	uint32_t biYPelsPerMeter;         // pref resol. in pixel/meter
	uint32_t biClrUsed;               // num. color map entries
	uint32_t biClrImportant;          // num. of signif. colors
}BMP_STRUCT;

typedef struct
{
    uint8_t  status;
    uint16_t posX;
    uint16_t posY;
    uint8_t weight;
    uint8_t pressed;
    uint8_t released;
    uint16_t posX2;
    uint16_t posY2;
    uint8_t weight2;
    uint16_t posX3;
    uint16_t posY3;
    uint8_t weight3;
    
}TS_STRUCT;

int8_t Lcd_Init(uint8_t lcdChoice); 
void LCD_Backlight(uint8_t intenisty);
void LCD_WriteCmd(uint8_t command);
void LCD_WriteData(uint8_t data);
uint8_t LCD_ReadData(void);
void LCD_Fill(uint16_t color);
void LCD_SetPixel(uint16_t posX,uint16_t posY, uint16_t color);
uint16_t LCD_GetPixel(uint16_t posX,uint16_t posY);
void LCD_DrawRect(uint16_t posX1,uint16_t posY1, uint16_t posX2,
		uint16_t posY2,uint8_t fill,uint16_t color);
void LCD_DrawText(const uint8_t * msg, FONT_INFO * font, ALIGN align,
        uint16_t posX, uint16_t posY, uint16_t color, uint16_t bg_color);
void LCD_2x16_WriteCmd(uint8_t command);
void LCD_2x16_WriteData(uint8_t command);
void LCD_2x16_WriteMsg(unsigned char * msg, uint8_t line);
void LCD_ButtonDraw(btn_t * button);
void LCD_ButtonCreate(uint16_t posX, uint16_t posY,
                      uint16_t width, uint16_t height,
                      uint16_t txtColor, uint16_t bgColor,
                      const uint8_t * textOrBmp, FONT_INFO * font,
                      void * fpPress, void * fpReleased, void * fpOut,
                      btn_t * button,
                      uint8_t index);
void LCD_TouchRead(TS_STRUCT * touch);
void TSC_ButtonExecute(btn_t * button, TS_STRUCT * tsc, uint8_t index);
uint8_t LCD_InButton(btn_t * button, uint16_t posX, uint16_t posY);
uint8_t LCD_Bitmap(const uint8_t * bmpPtr, uint16_t posX, uint16_t posY);
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* LCD_H */

