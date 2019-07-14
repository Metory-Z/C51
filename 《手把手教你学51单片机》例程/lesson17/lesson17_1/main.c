/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������17�� ADת������
* �汾�ţ�v1.0.0
* ��  ע���������17��17.4��
*         ��ģ������ͨ��0��1��3�ĵ�ѹֵ��ʾ��Һ����
*******************************************************************************
*/

#include <reg52.h>

bit flag300ms = 1;       //300ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
unsigned char GetADCValue(unsigned char chn);
void ValueToString(unsigned char *str, unsigned char val);
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    unsigned char val;
    unsigned char str[10];
    
    EA = 1;            //�����ж�
    ConfigTimer0(10);  //����T0��ʱ10ms
    InitLcd1602();     //��ʼ��Һ��    
    LcdShowStr(0, 0, "AIN0  AIN1  AIN3");  //��ʾͨ��ָʾ
    
    while (1)
    {
        if (flag300ms)
        {
            flag300ms = 0;
            //��ʾͨ��0�ĵ�ѹ
            val = GetADCValue(0);     //��ȡADCͨ��0��ת��ֵ
            ValueToString(str, val);  //תΪ�ַ�����ʽ�ĵ�ѹֵ
            LcdShowStr(0, 1, str);    //��ʾ��Һ����
            //��ʾͨ��1�ĵ�ѹ
            val = GetADCValue(1);
            ValueToString(str, val);
            LcdShowStr(6, 1, str);
            //��ʾͨ��3�ĵ�ѹ
            val = GetADCValue(3);
            ValueToString(str, val);
            LcdShowStr(12, 1, str);
        }
    }
}
/* ��ȡ��ǰ��ADCת��ֵ��chn-ADCͨ����0~3 */
unsigned char GetADCValue(unsigned char chn)
{
    unsigned char val;
    
    I2CStart();
    if (!I2CWrite(0x48<<1))  //ѰַPCF8591����δӦ����ֹͣ����������0
    {
        I2CStop();
        return 0;
    }
    I2CWrite(0x40|chn);        //д������ֽڣ�ѡ��ת��ͨ��
    I2CStart();
    I2CWrite((0x48<<1)|0x01);  //ѰַPCF8591��ָ������Ϊ������    
    I2CReadACK();              //�ȿն�һ���ֽڣ��ṩ����ת��ʱ��
    val = I2CReadNAK();        //��ȡ�ո�ת�����ֵ
    I2CStop();
    
    return val;
}
/* ADCת��ֵתΪʵ�ʵ�ѹֵ���ַ�����ʽ��str-�ַ���ָ�룬val-ADת��ֵ */
void ValueToString(unsigned char *str, unsigned char val)
{
    //��ѹֵ=ת�����*2.5V/255��ʽ�е�25������һλʮ����С��
    val = (val*25) / 255;
    str[0] = (val/10) + '0';  //����λ�ַ�
    str[1] = '.';             //С����
    str[2] = (val%10) + '0';  //С��λ�ַ�
    str[3] = 'V';             //��ѹ��λ
    str[4] = '\0';            //������
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 12;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ��300ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr300ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr300ms++;
    if (tmr300ms >= 30)  //��ʱ300ms
    {
        tmr300ms = 0;
        flag300ms = 1;
    }
}
