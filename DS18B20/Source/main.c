#include <stdio.h>
#include <string.h>
#include "ds18b20.h"  
#include "SPI_TFT.h"
#include "DHT11.H"

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])

extern void Delay_ms(uint Time);

#define SYSCLK_16MHZ 0   
#define SYSCLK_32MHZ 1   

void SetSysClk(unsigned char clk_source)
{
    // 系统时钟设置函数
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

void InitDs18(){
    // Ds18b20传感器初始化函数
    P0SEL &= 0x7f;               
}

 void main()
{
    // 定义存储数据用的数组
    uchar DHT11_temp[3]; 
    uchar DHT11_humidity[3];
    char Ds18_strTemp[6];
    float Ds18_fTemp;

    SetSysClk(SYSCLK_32MHZ);   // 设置系统时钟位32MHz
    // 初始化传感器和显示屏
    InitLcd();
    InitDs18();

    LCD_clear(GREEN);
    while(1)
    {        
        // 清空字符串
        memset(DHT11_temp, 0, 3);
        memset(DHT11_humidity, 0, 3);
        memset(Ds18_strTemp, 0, ARRAY_SIZE(Ds18_strTemp)); 

        // 获取Ds18b20传感器数据并存储到数组
        Ds18_fTemp = floatReadDs18B20();      
        sprintf(Ds18_strTemp, "%.01f", Ds18_fTemp); 

        // 获取DHT11传感器数据并存储到数组
        DHT11();
        DHT11_temp[0]=wendu_shi+0x30;
        DHT11_temp[1]=wendu_ge+0x30;
        DHT11_humidity[0]=shidu_shi+0x30;
        DHT11_humidity[1]=shidu_ge+0x30;

        // 打印数组到屏幕上及设置一些参数
        Color = BLUE;
        Color_BK = GREEN;
        LCD_write_EN_string(1,34,"DS18_Temp:");
        LCD_write_EN_string(70,34,Ds18_strTemp);
        LCD_write_EN_string(1,49,"DHT1_Temp:");
        LCD_write_EN_string(70,49,DHT11_temp);
        LCD_write_EN_string(1,64,"DHT1_Humi:");        
        LCD_write_EN_string(70,64,DHT11_humidity);

        Delay_ms(2000);                
    }
}