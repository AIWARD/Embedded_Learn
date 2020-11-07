#include "ds18b20.h" 
#include "DHT11.H"
#define Ds18b20IO P0_7       //�¶ȴ���������

// void Delay_us(unsigned int k)//us��ʱ����
// {
//     T1CC0L = 0x06; 
//     T1CC0H = 0x00; 
//     T1CTL = 0x02; 
//     while(k)
//     { 
//         while(!(T1CNTL >= 0x04));
//         k--;
//     }
//     T1CTL = 0x00;  //�رն�ʱ��
// }

// void Delay_ms(unsigned int k)
// {
//     T1CC0L = 0xe8;
//     T1CC0H = 0x03;
//     T1CTL = 0x0a; //ģģʽ 32��Ƶ
//     while(k)
//     {
//         while(!((T1CNTL >= 0xe8)&&(T1CNTH >= 0x03)));
//         k--;
//     }
//     T1CTL = 0x00; //�رն�ʱ��
// }

void Delay_s(unsigned int k)
{
    while(k)
    {
        Delay_ms(1000);
        k--;
    }
}

//ʱ��Ƶ��Ϊ32M
void Ds18b20Delay(unsigned int k)
{
    unsigned int i,j;
    for(i=0;i<k;i++)
        for(j=0;j<2;j++);
}

void Ds18b20InputInitial(void)//���ö˿�Ϊ����
{
    P0DIR &= 0x7f;
}

void Ds18b20OutputInitial(void)//���ö˿�Ϊ���
{
    P0DIR |= 0x80;
}

//ds18b20��ʼ��  ��ʼ���ɹ�����0x00��ʧ�ܷ���0x01
unsigned char Ds18b20Initial(void)
{
    unsigned char Status = 0x00;
    unsigned int CONT_1 = 0;
    unsigned char Flag_1 = 1;
    Ds18b20OutputInitial();
    Ds18b20IO = 1;      //DQ��λ
    Ds18b20Delay(260);  //������ʱ
    Ds18b20IO = 0;      //��Ƭ����DQ����
    Ds18b20Delay(750);  //��ȷ��ʱ ���� 480us С��960us
    Ds18b20IO = 1;      //��������
    Ds18b20InputInitial();//����IO����
    while((Ds18b20IO != 0)&&(Flag_1 == 1))//�ȴ�ds18b20��Ӧ�����з�ֹ��ʱ����
    {                                      //�ȴ�Լ60ms����
        CONT_1++;
        Ds18b20Delay(10);
        if(CONT_1 > 8000)Flag_1 = 0;
        Status = Ds18b20IO;
    }
    Ds18b20OutputInitial();
    Ds18b20IO = 1;
    Ds18b20Delay(100);
    return Status;       //���س�ʼ��״̬
}

void Ds18b20Write(unsigned char infor)
{
    unsigned int i;
    Ds18b20OutputInitial();
    for(i=0;i<8;i++)
    {
        if((infor & 0x01))
        {
            Ds18b20IO = 0;
            Ds18b20Delay(6);
            Ds18b20IO = 1;
            Ds18b20Delay(50);
        }
        else
        {
            Ds18b20IO = 0;
            Ds18b20Delay(50);
            Ds18b20IO = 1;
            Ds18b20Delay(6);
        }
        infor >>= 1;
    }
}

unsigned char Ds18b20Read(void)
{
    unsigned char Value = 0x00;
    unsigned int i;
    Ds18b20OutputInitial();
    Ds18b20IO = 1;
    Ds18b20Delay(10);
    for(i=0;i<8;i++)
    {
        Value >>= 1; 
        Ds18b20OutputInitial();
        Ds18b20IO = 0;// �������ź�
        Ds18b20Delay(3);
        Ds18b20IO = 1;// �������ź�
        Ds18b20Delay(3);
        Ds18b20InputInitial();
        if(Ds18b20IO == 1) Value |= 0x80;
        Ds18b20Delay(15);
    } 
    return Value;
}

//�¶ȶ�ȡ����
unsigned char ReadDs18B20(void) 
{
    unsigned char V1,V2;   //����ߵ�8λ ����
    unsigned char temp;    //�����¶Ȼ���Ĵ���
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    // ����������кŵĲ���
    Ds18b20Write(0x44);    // �����¶�ת��
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    //����������кŵĲ��� 
    Ds18b20Write(0xbe);    //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
    
    V1 = Ds18b20Read();    //��λ
    V2 = Ds18b20Read();    //��λ
    temp = ((V1 >> 4)+((V2 & 0x07)*16)); //ת������ 

    return temp;
}

//�¶ȶ�ȡ���� ��1λС��λ
float floatReadDs18B20(void) 
{
    unsigned char V1,V2;   //����ߵ�8λ ����
    unsigned int temp;     //�����¶Ȼ���Ĵ���
    float fValue;
    Ds18b20Initial();
    Ds18b20Write(0xcc);    // ����������кŵĲ���
    Ds18b20Write(0x44);    // �����¶�ת��
    
    Ds18b20Initial();
    Ds18b20Write(0xcc);    //����������кŵĲ��� 
    Ds18b20Write(0xbe);    //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
    
    V1 = Ds18b20Read();    //��λ
    V2 = Ds18b20Read();    //��λ
    //temp = ((V1 >> 4)+((V2 & 0x07)*16)); //ת������ 
    temp=V2*0xFF+V1;
    fValue = temp*0.0625;
    
    return fValue;
}