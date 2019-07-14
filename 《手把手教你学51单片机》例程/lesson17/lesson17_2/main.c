/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������17�� DAת������
* �汾�ţ�v1.0.0
* ��  ע���������17��17.6��
*         �ɰ�������DA����ɵ���ѹֵ
*******************************************************************************
*/

#include <reg52.h>

unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
extern void KeyScan();
extern void KeyDriver();
extern void I2CStart();
extern void I2CStop();
extern bit I2CWrite(unsigned char dat);

void main()
{    
    EA = 1;           //�����ж�
    ConfigTimer0(1);  //����T0��ʱ1ms
    
    while (1)
    {
        KeyDriver();  //���ð�������
    }
}
/* ����DAC���ֵ��val-�趨ֵ */
void SetDACOut(unsigned char val)
{
    I2CStart();
    if (!I2CWrite(0x48<<1)) //ѰַPCF8591����δӦ����ֹͣ����������
    {
        I2CStop();
        return;
    }
    I2CWrite(0x40);         //д������ֽ�
    I2CWrite(val);          //д��DAֵ  
    I2CStop();
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    static unsigned char volt = 0;  //�����ѹֵ��������һλʮ����С��λ
    
    if (keycode == 0x26)  //���ϼ�������0.1V��ѹֵ
    {
        if (volt < 25)
        {
            volt++;
            SetDACOut(volt*255/25); //ת��ΪAD���ֵ
        }
    }
    else if (keycode == 0x28)  //���¼�����С0.1V��ѹֵ
    {
        if (volt > 0)
        {
            volt--;
            SetDACOut(volt*255/25); //ת��ΪAD���ֵ
        }
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 28;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�а���ɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    KeyScan();   //����ɨ��
}
