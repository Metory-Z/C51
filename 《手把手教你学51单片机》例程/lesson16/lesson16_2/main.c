/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������16�� �¶ȴ�����DS18B20ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������16��16.4��
*         ����DS18B20�����¶Ȳ����¶�ֵ��ʾ��Һ����
*******************************************************************************
*/

#include <reg52.h>

bit flag1s = 0;          //1s��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
unsigned char IntToString(unsigned char *str, int dat);
extern bit Start18B20();
extern bit Get18B20Temp(int *temp);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    bit res;
    int temp;        //��ȡ���ĵ�ǰ�¶�ֵ
    int intT, decT;  //�¶�ֵ��������С������
    unsigned char len;
    unsigned char str[12];

    EA = 1;            //�����ж�
    ConfigTimer0(10);  //T0��ʱ10ms
    Start18B20();      //����DS18B20
    InitLcd1602();     //��ʼ��Һ��
    
    while (1)
    {
        if (flag1s)  //ÿ�����һ���¶�
        {
            flag1s = 0;
            res = Get18B20Temp(&temp);  //��ȡ��ǰ�¶�
            if (res)                    //��ȡ�ɹ�ʱ��ˢ�µ�ǰ�¶���ʾ
            {
                intT = temp >> 4;             //������¶�ֵ��������
                decT = temp & 0xF;            //������¶�ֵС������
                len = IntToString(str, intT); //��������ת��Ϊ�ַ���
                str[len++] = '.';             //���С����
                decT = (decT*10) / 16;        //�����Ƶ�С������ת��Ϊ1λʮ����λ
                str[len++] = decT + '0';      //ʮ����С��λ��ת��ΪASCII�ַ�
                while (len < 6)               //�ÿո��뵽6���ַ�����
                {
                    str[len++] = ' ';
                }
                str[len] = '\0';              //����ַ���������
                LcdShowStr(0, 0, str);        //��ʾ��Һ������
            }
            else                        //��ȡʧ��ʱ����ʾ������Ϣ
            {
                LcdShowStr(0, 0, "error!");
            }
            Start18B20();               //����������һ��ת��
        }
    }
}
/* ������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char IntToString(unsigned char *str, int dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[6];
    
    if (dat < 0)  //���Ϊ����������ȡ����ֵ������ָ������Ӹ���
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
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
/* T0�жϷ����������1�붨ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr1s = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr1s++;
    if (tmr1s >= 100)  //��ʱ1s
    {
        tmr1s = 0;
        flag1s = 1;
    }
}
