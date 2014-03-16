#ifndef OLED12864_H
#define OLED12864_H
#include "support_common.h"

#define byte unsigned char 
#define uint unsigned int 

 extern char beyond96x64[512];
 extern char beyond64x64[512];

 void OLCD_init(void);
 void delay_ms(uint ms);
 void LCD_CLS(void);
 void LCD_P6x8Str(byte x,byte y,char ch[]);
 void LCD_P8x16Str(byte x,byte y,char ch[]);
 void LCD_P6x8float(byte x,byte y,float num);
 void LCD_P6x8int(byte x,byte y,int num);
 void LCD_PutPixel(byte x,byte y);
 void LCD_Rectangle(byte x1,byte y1,byte x2,byte y2,byte gif);
 void Draw_BMP(byte x0,byte y0,byte x1,byte y1,char bmp[]); 
 void LCD_Fill(byte dat);
 void LCD_test(byte x1,byte y1,byte x2,byte y2);
#endif

