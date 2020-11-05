#include <ioCC2530.h>
#include <string.h>
#include "DHT11.H" 
#include "SPI_TFT.h"

#define SYSCLK_16MHZ 0    
#define SYSCLK_32MHZ 1    

void SetSysClk(unsigned char clk_source)
{
  // 设置系统时钟
  if(clk_source==SYSCLK_16MHZ)
  {
    CLKCONCMD |= 0x40; 
    CLKCONCMD |= 0x01; 
  }
  if(clk_source==SYSCLK_32MHZ)
  {         
    CLKCONCMD &=~0x40; 
    while (!(CLKCONSTA &(~0x40))); 
    CLKCONCMD &=~0x01;  
  }
}

void main(void)
{  
    uchar temp[3]; 
    uchar humidity[3];   
    SetSysClk(SYSCLK_32MHZ);
    InitLcd();

    Delay_ms(1000);         
    LCD_clear(GREEN);
    while(1)
    {         
        memset(temp, 0, 3);
        memset(humidity, 0, 3);
        DHT11();             // 获取DHT11传感器数据

        // 转换数据格式
        temp[0]=wendu_shi+0x30;
        temp[1]=wendu_ge+0x30;
        humidity[0]=shidu_shi+0x30;
        humidity[1]=shidu_ge+0x30;

        // 屏幕显示数值及设置字体
        Color = BLUE;
        Color_BK = GREEN;
        LCD_write_EN_string(20,49,"Temp     :");
        LCD_write_EN_string(20,64,"Humidity:");                
        LCD_write_EN_string(95,49,temp);
        LCD_write_EN_string(95,64,humidity);


        Delay_ms(2000);  //?????2S???1?? 
    }
}
