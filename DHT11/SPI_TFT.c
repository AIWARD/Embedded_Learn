#include <ioCC2530.h>
#include "SPI_TFT.H"
#include "Font.H"
#include "DHT11.H"


#define LCD_COMMAND 0x00
#define LCD_DATA    0x01

#define LCD_MAX_BUF 25

#define SOFTWARE_SPI
//#define HARDWARE_SPI  //LCD���Ʋ���Ӳ��SPI

#define COL_SPACE   2   //�ַ����
#define ROW_SPACE   4   //�м��
#define ALL_CNS     128

// SPI TFT LCD�˿ڶ���
#define LCD_DC                 P0_0  // ����/����ѡ��, L_R
#define LCD_SDA                P1_3  // ˫������, L_D  P1_6   P1_3
#define LCD_SCL                P1_2  // ʱ��, L_C P1_2  P1_5
//#define LCD_CS               P1_2  // Ƭѡ, L_S ��Ƭѡ�ӵ�
#define LCD_RST                P1_7  // ����lcd��λ   

//Һ�����ƿ���1�������궨��
//#define LCD_CS_SET  	(LCD_CS_CLRx01)   
#define	LCD_DC_SET  	(LCD_DC = 0x01)  
#define	LCD_SDA_SET  	(LCD_SDA = 0x01)    
#define	LCD_SCL_SET  	(LCD_SCL = 0x01)   
#define	LCD_RST_SET  	(LCD_RST = 0x01)  

//Һ�����ƿ���0�������궨��
//#define LCD_CS_CLR  	(LCD_CS_CLRx00)    
#define	LCD_DC_CLR  	(LCD_DC = 0x00)  
#define	LCD_SDA_CLR  	(LCD_SDA = 0x00)     
#define	LCD_SCL_CLR  	(LCD_SCL = 0x00)
#define	LCD_RST_CLR  	(LCD_RST = 0x00)   


void SPI_Init(void);
void LCD_clear(uint color);
void LCD_write_EN_string(uchar X,uchar Y,uint8 *s);
static void LCD_set_window(uint X, uint Y, 
                           unsigned Width,uint Height);
static void LCD_write_EN(uint X, uint Y, uchar c);
static void LCD_write_CN(uint X, uint Y, uchar c);



uint  Color    = YELLOW; // ǰ����ɫ
uint  Color_BK = BLUE;   // ������ɫ



//��SPI���ߴ���һ��8λ����
void  SendByte(uchar Data)
{
  uchar i=0;
  
  for(i=8;i>0;i--)
  {
    if(Data&0x80)	
      LCD_SDA_SET;//�������
    else 
      LCD_SDA_CLR;
    
    LCD_SCL_CLR;       
    LCD_SCL_SET;
    Data<<=1; 
  }
}

//��SPI���ߴ���һ��8λ����
void  SPI_WriteData(uchar Data)
{
  uchar i=0;
  
  for(i=8;i>0;i--)
  {
    if(Data&0x80)	
      LCD_SDA_SET;//�������
    else 
      LCD_SDA_CLR;
    
    LCD_SCL_CLR;       
    LCD_SCL_SET;
    Data<<=1; 
  }
}

//��Һ����дһ��8λָ��
void Lcd_WriteIndex(uchar Index)
{ 
  //LCD_CS_CLR;  //ALD del  SPIд����ʱ��ʼ
  LCD_DC_CLR;
  SPI_WriteData(Index);
  //LCD_CS_SET;  //ALD del
}

//��Һ����дһ��8λ����
void Lcd_WriteData(uchar Data)
{
  //LCD_CS_CLR;  //ALD del
  LCD_DC_SET;
  SPI_WriteData(Data);
  //LCD_CS_SET;  //ALD del 
}

//��Һ����дһ��16λ����
void LCD_WriteData_16Bit(unsigned short Data)
{
  //LCD_CS_CLR;  //ALD del
  LCD_DC_SET;
  SPI_WriteData(Data>>8); //д���8λ����
  SPI_WriteData(Data); 	  //д���8λ����
  //LCD_CS_SET;  //ALD del 
}

/****************************************************************************
* ��    ��: LCD_write_byte()
* ��    ��: ���ݵ�LCD 
* ��ڲ���: data    ��д�������
*           command ��д����/����ѡ��  
* ���ڲ���: �� 
****************************************************************************/
static void LCD_write_byte(uchar data, uchar command)
{
  ////LCD_CS_CLR ;// ʹ��LCD
  if (command == 0)
    Lcd_WriteIndex(data); // ��������
  else
    Lcd_WriteData(data) ; // ��������
  //SendByte(data);
  //LCD_CS_SET; // �ر�LCD
  //LCD_DC_SET;	
}

/****************************************************************************
* ��    ��: SPI_Init()
* ��    ��: ��ʼ��Ӳ��SPI�ӿ�
* ��ڲ���: ��
* ���ڲ���: ��
****************************************************************************/
void SPI_Init(void)
{
  PERCFG |= 0x02; //��UART1����ΪSPIģʽ
  P1SEL  |= 0x60; //��P16/P15����ΪSPI�豸����
  U1BAUD  = 0x00; 
  U1CSR   = 0x00;
  U1GCR   = 0xF3; //SPICLK=ϵͳʱ��/2, ��ģʽ, MSB��ǰ
}

// ��λlcd
void Lcd_Reset(void)
{
  LCD_RST_CLR;
  Delay_ms(150);
  LCD_RST_SET;
  Delay_ms(50);
}

//��ʼ��SPI�ӿڲ�ɫTFTҺ����������ΪST7735 
void InitLcd(void)
{
  P0SEL &= 0xFE; //����P00Ϊ��ͨIO�ڣ�
  P0DIR |= 0x01; //����P00Ϊ���
  P1SEL &= 0x73; //����P12��P13,P17Ϊ��ͨIO��
  P1DIR |= 0x8c; //����P12��P13,P17Ϊ��� 

  Lcd_Reset();
  
#ifdef HARDWARE_SPI                                         
  P1DIR &= 0x9F;
  SPI_Init();   //��ʼ��SPI�˿�  
#endif
 
  LCD_write_byte(0x11,LCD_COMMAND); //�˳�˯��ģʽ
  Delay_ms(1); 

  ///ST7735R Frame Rate
  LCD_write_byte(0xB1,LCD_COMMAND); 
  LCD_write_byte(0x01,LCD_DATA); 
  LCD_write_byte(0x2C,LCD_DATA); 
  LCD_write_byte(0x2D,LCD_DATA); 
  LCD_write_byte(0xB2,LCD_COMMAND); 
  LCD_write_byte(0x01,LCD_DATA); 
  LCD_write_byte(0x2C,LCD_DATA); 
  LCD_write_byte(0x2D,LCD_DATA); 
  LCD_write_byte(0xB3,LCD_COMMAND); 
  LCD_write_byte(0x01,LCD_DATA); 
  LCD_write_byte(0x2C,LCD_DATA); 
  LCD_write_byte(0x2D,LCD_DATA); 
  LCD_write_byte(0x01,LCD_DATA); 
  LCD_write_byte(0x2C,LCD_DATA); 
  LCD_write_byte(0x2D,LCD_DATA); 

  LCD_write_byte(0xB4,LCD_COMMAND);//�жԵ�
  LCD_write_byte(0x07,LCD_DATA); 
  //ST7735R�ӵ�����
  LCD_write_byte(0xC0,LCD_COMMAND); 
  LCD_write_byte(0xA2,LCD_DATA); 
  LCD_write_byte(0x02,LCD_DATA); 
  LCD_write_byte(0x84,LCD_DATA); 
  LCD_write_byte(0xC1,LCD_COMMAND); 
  LCD_write_byte(0xC5,LCD_DATA); 
  LCD_write_byte(0xC2,LCD_COMMAND); 
  LCD_write_byte(0x0A,LCD_DATA); 
  LCD_write_byte(0x00,LCD_DATA); 
  LCD_write_byte(0xC3,LCD_COMMAND); 
  LCD_write_byte(0x8A,LCD_DATA); 
  LCD_write_byte(0x2A,LCD_DATA); 
  LCD_write_byte(0xC4,LCD_COMMAND); 
  LCD_write_byte(0x8A,LCD_DATA); 
  LCD_write_byte(0xEE,LCD_DATA); 

  LCD_write_byte(0xC5,LCD_COMMAND); //��VCOM 
  LCD_write_byte(0x0E,LCD_DATA); 

  LCD_write_byte(0x36,LCD_COMMAND); //RGBģʽ 
  LCD_write_byte(0xC8,LCD_DATA); 
  
  // ST7735R Gamma У������
  LCD_write_byte(0xe0,LCD_COMMAND); 
  LCD_write_byte(0x0f,LCD_DATA); 
  LCD_write_byte(0x1a,LCD_DATA); 
  LCD_write_byte(0x0f,LCD_DATA); 
  LCD_write_byte(0x18,LCD_DATA); 
  LCD_write_byte(0x2f,LCD_DATA); 
  LCD_write_byte(0x28,LCD_DATA); 
  LCD_write_byte(0x20,LCD_DATA); 
  LCD_write_byte(0x22,LCD_DATA); 
  LCD_write_byte(0x1f,LCD_DATA); 
  LCD_write_byte(0x1b,LCD_DATA); 
  LCD_write_byte(0x23,LCD_DATA); 
  LCD_write_byte(0x37,LCD_DATA); 
  LCD_write_byte(0x00,LCD_DATA); 

  LCD_write_byte(0x07,LCD_DATA); 
  LCD_write_byte(0x02,LCD_DATA); 
  LCD_write_byte(0x10,LCD_DATA); 
  LCD_write_byte(0xe1,LCD_COMMAND); 
  LCD_write_byte(0x0f,LCD_DATA); 
  LCD_write_byte(0x1b,LCD_DATA); 
  LCD_write_byte(0x0f,LCD_DATA); 
  LCD_write_byte(0x17,LCD_DATA); 
  LCD_write_byte(0x33,LCD_DATA); 
  LCD_write_byte(0x2c,LCD_DATA); 
  LCD_write_byte(0x29,LCD_DATA); 
  LCD_write_byte(0x2e,LCD_DATA); 
  LCD_write_byte(0x30,LCD_DATA); 
  LCD_write_byte(0x30,LCD_DATA); 
  LCD_write_byte(0x39,LCD_DATA); 
  LCD_write_byte(0x3f,LCD_DATA); 
  LCD_write_byte(0x00,LCD_DATA); 
  LCD_write_byte(0x07,LCD_DATA); 
  LCD_write_byte(0x03,LCD_DATA); 
  LCD_write_byte(0x10,LCD_DATA);  

  LCD_write_byte(0x2a,LCD_COMMAND);
  LCD_write_byte(0x00,LCD_DATA);
  LCD_write_byte(0x00,LCD_DATA);
  LCD_write_byte(0x00,LCD_DATA);
  LCD_write_byte(0x7f,LCD_DATA);
  LCD_write_byte(0x2b,LCD_COMMAND);
  LCD_write_byte(0x00,LCD_DATA);
  LCD_write_byte(0x00,LCD_DATA);
  LCD_write_byte(0x00,LCD_DATA);
  LCD_write_byte(0x9f,LCD_DATA);

  LCD_write_byte(0xF0,LCD_COMMAND); //ʹ�ܲ�������  
  LCD_write_byte(0x01,LCD_DATA); 
  LCD_write_byte(0xF6,LCD_COMMAND); //�ر�ramʡ��ģʽ 
  LCD_write_byte(0x00,LCD_DATA); 

  LCD_write_byte(0x3A,LCD_COMMAND); //ɫ��16bit 
  LCD_write_byte(0x05,LCD_DATA); 
  
  LCD_write_byte(0x29,LCD_COMMAND); //����ʾ  
  LCD_clear(WINDOW_BK_COLOR);       //�Ա���ɫ���� 
}

/****************************************************************************
* ��    ��: LCD_clear()
* ��    ��: ��ָ����ɫ����
* ��ڲ���: color ��������ɫ
* ���ڲ���: ��
****************************************************************************/
void LCD_clear(uint color)
{
  register uint  i=0,j=0;
  register uchar ch;
  register uchar cl;
  
  LCD_set_window(0,0,X_MAX_PIXEL,Y_MAX_PIXEL);
  ch = color>>8;
  cl = color&0xFF;
  LCD_write_byte(0x2C,LCD_COMMAND);
  //LCD_CS_CLR;
  LCD_DC_SET;
  for (i=0;i<Y_MAX_PIXEL;i++)
    for (j=0;j<2;j++)
    {
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);      
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
    }
  LCD_DC_SET;
  //LCD_CS_SET;
}

/****************************************************************************
* ��    ��: LCD_set_window()
* ��    ��: ������ʾ����
* ��ڲ���: X - ��ʾ�������ϽǺ�����      Y ����ʾ�������Ͻ�������
*           Width ����ʾ���ڿ���          Height ����ʾ���ڸ߶�
* ���ڲ���: ��
****************************************************************************/
static void LCD_set_window(uint X    , uint Y, 
                           uint Width, uint Height)
{
  LCD_write_byte(0x2A,LCD_COMMAND);         //�е�ַ��������
  LCD_write_byte(X>>8,LCD_DATA) ;           //������ʾ�������ϽǺ������λ
  LCD_write_byte((X+2)&0xFF,LCD_DATA);      //������ʾ�������ϽǺ������λ 	
  LCD_write_byte((X+Width+1)>>8,LCD_DATA);  //���ÿ��ȸ�λ 	
  LCD_write_byte((X+Width+1)&0xFF,LCD_DATA);// ���ÿ��ȵ�λ 	
  
  LCD_write_byte(0x2B,LCD_COMMAND);         //�е�ַ��������
  LCD_write_byte(Y>>8,LCD_DATA) ;           //������ʾ�������Ͻ��������λ
  LCD_write_byte((Y+3)&0xFF,LCD_DATA);      //������ʾ�������Ͻ��������λ 
  LCD_write_byte((Y+Height+2)>>8,LCD_DATA); // ������ʾ���ڸ߶ȸ�λ 	
  LCD_write_byte((Y+Height+2)&0xFF,LCD_DATA);// ������ʾ���ڸ߶ȵ�λ 	  
  LCD_write_byte(0x2C,LCD_COMMAND) ;
}

/****************************************************************************
* ��    ��: DrawPixel()
* ��    ��: ��ָ��λ����ָ����ɫ��ʾ����
****************************************************************************/
void DrawPixel(uint x, uint y, uint color)
{
  //LCD_set_XY(x,y);  //�趨��ʾλ�� andy
  LCD_write_byte(color>>8,LCD_DATA);  //������ʾ��ɫ���ݸ�λ�ֽ�
  LCD_write_byte(color&0xFF,LCD_DATA);//������ʾ��ɫ���ݵ�λ�ֽ�
}

/****************************************************************************
* ��    ��: DrawVerticalLine()
* ��    ��: ��ָ��λ����ָ����ɫ����ֱ��
* ��ڲ���: X - ��ֱ����ʼλ�ú�����      Y ����ֱ����ʼλ��������
*           Length ����ֱ�߳���           Color ����ֱ����ɫ
* ���ڲ���: ��
****************************************************************************/
void DrawVerticalLine(uint x, uint y, 
                      uint Length,uint Color)               
{
  register int  index = 0;
  register char ch,cl;
  
  ch = (uchar)(Color>>8);
  cl = (uchar)(Color);
  LCD_set_window(x,y,1,Length);
  for(index=0;index<Length;index++)
  {
  LCD_write_byte(ch,LCD_DATA) ;
  LCD_write_byte(cl,LCD_DATA) ;
  }
}

/****************************************************************************
* ��    ��: DrawHorizonLine()
* ��    ��: ��ָ��λ����ָ����ɫ��ˮƽ��
* ��ڲ���: X - ˮƽ����ʼλ�ú�����      Y ��ˮƽ����ʼλ��������
*           Length ��ˮƽ�߳���           Color ��ˮƽ����ɫ
* ���ڲ���: ��
****************************************************************************/
void DrawHorizonLine(uint x, uint y, 
                     uint Length,uint Color)               
{
  register int  index = 0;
  register char ch,cl;
  
  ch = (uchar)(Color>>8);
  cl = (uchar)(Color);
  LCD_set_window(x,y,Length,1);
  for(index=0;index<Length;index++)
  {
    LCD_write_byte(ch,LCD_DATA);
    LCD_write_byte(cl,LCD_DATA);
  }
}

/****************************************************************************
* ��    ��: DrawRect()
* ��    ��: ��ָ��λ����ָ����ɫ������
* ��ڲ���: X - ������ʼλ�ú�����      Y ��������ʼλ��������
*           Width  �����ο���           Height �����θ߶�                   
*           Color  ��������ɫ             
* ���ڲ���: ��
****************************************************************************/
void DrawRect(uint Xpos, uint Ypos, uint Width, 
              uint Height,unsigned Color)
{
  DrawHorizonLine(Xpos, Ypos, Width, Color) ;
  DrawHorizonLine(Xpos, (Ypos + Height), Width, Color) ;
  DrawVerticalLine(Xpos, Ypos, Height, Color) ;
  DrawVerticalLine((Xpos+Width-1) ,Ypos, Height, Color);
}

/****************************************************************************
* ��    ��: DrawRectFill()
* ��    ��: ��ָ��λ����ָ����ɫ������
* ��ڲ���: X - ������ʼλ�ú�����      Y ��������ʼλ��������
*           Width  �����ο���           Height �����θ߶�                   
*           Color  �����������ɫ             
* ���ڲ���: ��
****************************************************************************/
void DrawRectFill(uint Xpos  , uint Ypos, uint Width, 
                  uint Height, unsigned Color                        )
{
  register uint i;
  register uint index = Width*Height/16+1;
  register char ch,cl;
  
  ch = (uchar)(Color>>8);
  cl = (uchar)(Color);
  LCD_set_window(Xpos,Ypos,Width,Height);
  LCD_write_byte(0x2C,LCD_COMMAND);
  //LCD_CS_CLR ; andy
  LCD_DC_SET ;
  for(i=0;i<index;i++)
  {
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl); 
      SendByte(ch);
      SendByte(cl);      
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
      SendByte(ch);
      SendByte(cl);
  }    
}

/****************************************************************************
* ��    ��: LCD_write_EN()
* ��    ��: ��ָ��λ����ʾ 7 ��14 ����ASCII�ַ� 
* ��ڲ���: X - ��ʾλ�����ϽǺ�����      Y ����ʾλ�����Ͻ�������
*           c ����ʾ�ַ�                  
* ���ڲ���: ��
****************************************************************************/
static void LCD_write_EN(uint X, uint Y, uchar c)
{
  uchar i,j,temp;
  
  LCD_set_window(X,Y,7,14);
  //LCD_CS_CLR ;
  LCD_DC_SET ;
  for(i=0;i<14;i++)
  {
    temp = ASCII7x14[c-32][i];
    for(j=0;j<7;j++)
    {
      if(temp&0x80)
      {
        SendByte(Color>>8);  
        SendByte(Color) ;
      }
      else
      {
        SendByte(Color_BK>>8) ;  
        SendByte(Color_BK);
      }            
      temp <<= 1 ;
    }
  }
  //LCD_CS_SET ; andy
  LCD_DC_SET ;
}

/****************************************************************************
* ��    ��: LCD_write_EN_string()
* ��    ��: ��ָ��λ����ʾ 7 ��14 ����ASCII�ַ��� 
* ��ڲ���: X - ��ʾλ�����ϽǺ�����      Y ����ʾλ�����Ͻ�������
*           s ���ַ���ָ��                  
* ���ڲ���: ��
****************************************************************************/
void LCD_write_EN_string(uchar X,uchar Y,uint8 *s)
{
  uchar i=0;
  
  while (*s)
  {
    LCD_write_EN(X+i*7,Y,*s);
    s++;
    i++;
    if(i>=HAL_LCD_MAX_CHARS)return;
  }
  return;
}

/****************************************************************************
* ��    ��: LCD_write_CN()
* ��    ��: ��ָ��λ����ʾ 14 ��14 �������ַ�  
* ��ڲ���: X - ��ʾλ�����ϽǺ�����      Y ����ʾλ�����Ͻ�������
*           c ����ʾ�ַ�ָ��                  
* ���ڲ���: ��
****************************************************************************/
static void LCD_write_CN(uint X, uint Y, uchar c)
{
  uchar i,j,temp;
  
  LCD_set_window(X,Y,14,14);  //���ַ������С������ʾ����
  //LCD_CS_CLR;               //Һ��SPIʹ��andy
  LCD_DC_SET;                 //���������ź�ʹ��
  for(i=0;i<28;i++)           // 14x14�ַ��������ݴ�СΪ2x14=28�ֽ�
  {
    temp = CN14x14[c][i] ;    //����ģ����
    for(j=0;j<8;j++)          // ��λ��ʾ
    {
      if(temp&0x80)           //���λֵΪ1����ʾ�ַ�ɫ
      {
        SendByte(Color>>8);  
        SendByte(Color) ;
      }
      else                    //���λֵΪ0����ʾ����ɫ
      {
        SendByte(Color_BK>>8) ;  
        SendByte(Color_BK);
      }            
      temp <<= 1 ;
    }
    i++ ;                     //ָ����һ�ֽ�
    temp = CN14x14[c][i];
    for(j=0;j<6;j++)          // ��λ��ʾ���������Ϊ14���أ�
    {                         // ����ÿ�е�2�ֽ����2λ����
      if(temp&0x80)           // ���λֵΪ1����ʾ�ַ�ɫ
      {
        SendByte(Color>>8);  
        SendByte(Color) ;
      }
      else                    // ���λֵΪ0����ʾ����ɫ
      {
        SendByte(Color_BK>>8) ;  
        SendByte(Color_BK);
      }            
      temp <<= 1 ;
    }
  }
  //LCD_CS_SET;               //Һ��SPI��ֹ
  LCD_DC_SET;                 //���ݿ����źŽ�ֹ
}

/****************************************************************************
* ��    ��: LCD_write_CN_string()
* ��    ��: ��ָ��λ����ʾ 14 ��14 ���������ַ���   
* ��ڲ���: X - ��ʾλ�����ϽǺ�����      Y ����ʾλ�����Ͻ�������
*           s ���ַ���ָ��                  
* ���ڲ���: ��
****************************************************************************/
void LCD_write_CN_string(uchar X,uchar Y,uint8 *s)
{
  uchar i=0;
  static uchar j=0;
  while (*s)
  {
    for(j=0;j<ALL_CNS;j++)                  //����������ģ
    { 
      if(  *s    ==CN14x14_Index[j*2]            
         &&*(s+1)==CN14x14_Index[j*2+1])    //���������������ģ����������
      {
        LCD_write_CN(X+i*14,Y,j) ;          //��ʾ����
        break; 
      }
    }
    if(j>=ALL_CNS)                          //�޴��ֵĵ�����ģ
    {
      DrawRectFill(X+i*14,Y,14,14,Color_BK);//��ʾ�ո�
    }
    s++; s++ ;                              //�ַ�ָ����λ
    i++;
    if(i>=HAL_LCD_MAX_CHARS/2)  
      return;                               //���������ʾ�ַ���������
  }
  return ;
}

/****************************************************************************
* ��    ��: ShowImage()
* ��    ��: ��ʾͼƬ   
* ��ڲ���: p - ͼƬ��������  ȡģ��ʽ ˮƽɨ�� ������ ��λ��ǰ           
* ���ڲ���: ��
****************************************************************************/
void ShowImage(const uchar *p) 
{
  int i; 
  uchar picH,picL;
  //LCD_clear(White); //����
  
  LCD_set_window(24,5,80,40); //��������
  for(i=0;i<80*40;i++)
  {	
    picL=*(p+i*2);	//���ݵ�λ��ǰ
    picH=*(p+i*2+1);				
    LCD_WriteData_16Bit(picH<<8|picL);  						
  }	
}