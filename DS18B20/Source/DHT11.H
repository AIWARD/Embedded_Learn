#ifndef __DHT11_H__
#define __DHT11_H__


#define uchar unsigned char
#define uint unsigned int

extern void COM(void);                  // ��ʪд��
extern void DHT11(void);                //��ʪ��������

extern void Delay_ms(uint Time);

extern uchar temp[2]; 
extern uchar temp1[5];
extern uchar humidity[2];
extern uchar humidity1[9];
extern uchar shidu_shi,shidu_ge,wendu_shi,wendu_ge;


#endif