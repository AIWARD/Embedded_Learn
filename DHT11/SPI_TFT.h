#ifndef _SPI_LCD_H_
#define _SPI_LCD_H_

typedef unsigned char uint8  ;
typedef unsigned int  uint ;

//LCD ��ɫ
#define  WHITE          0xFFFF
#define  BLACK          0x0000
#define  GREY           0xF7DE
#define  GREY2          0xF79E
#define  DARK_GREY      0x6B4D
#define  DARK_GREY2     0x52AA
#define  LIGHT_GREY     0xE71C
#define  BLUE           0x001F
#define  BLUE2          0x051F
#define  RED            0xF800
#define  MAGENTA        0xF81F
#define  GREEN          0x07E0
#define  CYAN           0x7FFF
#define  YELLOW         0xFFE0


//��ʾ������ɫ
#define WINDOW_BK_COLOR 0xDFFF //���ڱ���ɫ
#define WINDOW_COLOR    0x11FA //����ǰ��ɫ
#define TITLE_BK_COLOR  0x11FA //����������ɫ
#define TITLE_COLOR     0xDFFF //������ǰ��ɫ
#define STATUS_BK_COLOR 0x0014 //״̬������ɫ
#define STATUS_COLOR    0xDFFF //״̬��ǰ��ɫ

//Ӳ��SPIģʽʱ���ݷ��ͺ�
/*#define SendByte(Data) {  U1DBUF = Data;}  */

#define HAL_LCD_MAX_CHARS   18    // ÿ������ַ���
#define HAL_LCD_MAX_BUFF    25
#define X_MAX_PIXEL	    128
#define Y_MAX_PIXEL	    128

extern unsigned int  Color;      // ǰ����ɫ
extern unsigned int  Color_BK;   // ������ɫ

extern void InitLcd(void);
extern void LCD_clear(unsigned int color);
extern void DrawPixel(unsigned int x, unsigned int y, unsigned int color); 
extern void DrawHorizonLine(unsigned int x, unsigned int y, 
                            unsigned int Length,unsigned int Color);
extern void DrawVerticalLine(unsigned int x, unsigned int y, 
                             unsigned int Length,unsigned int Color);    
extern void DrawRect(unsigned int Xpos, unsigned int Ypos, unsigned int Width, 
                     unsigned int Height,unsigned Color);
extern void DrawRectFill(unsigned int Xpos, unsigned int Ypos, 
                         unsigned int Width,unsigned int Height,unsigned Color);
extern unsigned int Read_Reg(unsigned short Reg_Addr);
extern void LCD_write_EN_string(unsigned char X,unsigned char Y,uint8 *s);
extern void LCD_write_CN_string(unsigned char X,unsigned char Y,uint8 *s);

extern void ShowImage(const unsigned char *p);
#ifdef __cplusplus
}
#endif

#endif
