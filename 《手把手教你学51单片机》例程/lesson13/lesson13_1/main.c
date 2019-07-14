/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������13�� 1602Һ�������ƶ�ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������13��13.2��
*         1602Һ����ʾ�����ַ�������ʵ���������ظ�����
*******************************************************************************
*/

#include <reg52.h>

#define LCD1602_DB  P0
sbit LCD1602_RS = P1^0;
sbit LCD1602_RW = P1^1;
sbit LCD1602_E  = P1^5;

bit flag500ms = 0;   //500ms��ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
//����ʾ�ĵ�һ���ַ���
unsigned char code str1[] = "Kingst Studio";
//����ʾ�ĵڶ����ַ������豣�����һ���ַ����ȳ����϶̵��п��ÿո���
unsigned char code str2[] = "Let's move...";

void ConfigTimer0(unsigned int ms);
void InitLcd1602();
void LcdShowStr(unsigned char x, unsigned char y,
                unsigned char *str, unsigned char len);

void main()
{
    unsigned char i;
    unsigned char index = 0;  //�ƶ�����
    unsigned char pdata bufMove1[16+sizeof(str1)+16]; //�ƶ���ʾ������1
    unsigned char pdata bufMove2[16+sizeof(str2)+16]; //�ƶ���ʾ������2

    EA = 1;            //�����ж�
    ConfigTimer0(10);  //����T0��ʱ10ms
    InitLcd1602();     //��ʼ��Һ��
    //��������ͷһ�����Ϊ�ո�
    for (i=0; i<16; i++)
    {
        bufMove1[i] = ' ';
        bufMove2[i] = ' ';
    }
    //����ʾ�ַ����������������м�λ��
    for (i=0; i<(sizeof(str1)-1); i++)
    {
        bufMove1[16+i] = str1[i];
        bufMove2[16+i] = str2[i];
    }
    //��������βһ��Ҳ���Ϊ�ո�
    for (i=(16+sizeof(str1)-1); i<sizeof(bufMove1); i++)
    {
        bufMove1[i] = ' ';
        bufMove2[i] = ' ';
    }
    
    while (1)
    {
        if (flag500ms)  //ÿ500ms�ƶ�һ����Ļ
        {
            flag500ms = 0;
            //�ӻ������������ʾ��һ���ַ���ʾ��Һ����
            LcdShowStr(0, 0, bufMove1+index, 16);
            LcdShowStr(0, 1, bufMove2+index, 16);
            //�ƶ�����������ʵ������
            index++;
            if (index >= (16+sizeof(str1)-1))
            {   //��ʼλ�ôﵽ�ַ���β���󼴷��ش�ͷ��ʼ
                index = 0;
            }
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
/* �ȴ�Һ��׼���� */
void LcdWaitReady()
{
    unsigned char sta;
    
    LCD1602_DB = 0xFF;
    LCD1602_RS = 0;
    LCD1602_RW = 1;
    do {
        LCD1602_E = 1;
        sta = LCD1602_DB; //��ȡ״̬��
        LCD1602_E = 0;
    } while (sta & 0x80); //bit7����1��ʾҺ����æ���ظ����ֱ�������0Ϊֹ
}
/* ��LCD1602Һ��д��һ�ֽ����cmd-��д������ֵ */
void LcdWriteCmd(unsigned char cmd)
{
    LcdWaitReady();
    LCD1602_RS = 0;
    LCD1602_RW = 0;
    LCD1602_DB = cmd;
    LCD1602_E  = 1;
    LCD1602_E  = 0;
}
/* ��LCD1602Һ��д��һ�ֽ����ݣ�dat-��д������ֵ */
void LcdWriteDat(unsigned char dat)
{
    LcdWaitReady();
    LCD1602_RS = 1;
    LCD1602_RW = 0;
    LCD1602_DB = dat;
    LCD1602_E  = 1;
    LCD1602_E  = 0;
}
/* ������ʾRAM��ʼ��ַ���༴���λ�ã�(x,y)-��Ӧ��Ļ�ϵ��ַ����� */
void LcdSetCursor(unsigned char x, unsigned char y)
{
    unsigned char addr;
    
    if (y == 0)  //���������Ļ���������ʾRAM�ĵ�ַ
        addr = 0x00 + x;  //��һ���ַ���ַ��0x00��ʼ
    else
        addr = 0x40 + x;  //�ڶ����ַ���ַ��0x40��ʼ
    LcdWriteCmd(addr | 0x80);  //����RAM��ַ
}
/* ��Һ������ʾ�ַ�����(x,y)-��Ӧ��Ļ�ϵ���ʼ���꣬
   str-�ַ���ָ�룬len-����ʾ���ַ����� */
void LcdShowStr(unsigned char x, unsigned char y, 
                unsigned char *str, unsigned char len)
{
    LcdSetCursor(x, y);   //������ʼ��ַ
    while (len--)         //����д��len���ַ�����
    {
        LcdWriteDat(*str++);  //��ȡstrָ������ݣ�Ȼ��str�Լ�1
    }
}
/* ��ʼ��1602Һ�� */
void InitLcd1602()
{
    LcdWriteCmd(0x38);  //16*2��ʾ��5*7����8λ���ݽӿ�
    LcdWriteCmd(0x0C);  //��ʾ���������ر�
    LcdWriteCmd(0x06);  //���ֲ�������ַ�Զ�+1
    LcdWriteCmd(0x01);  //����
}
/* T0�жϷ���������ʱ500ms */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr500ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    tmr500ms++;
    if (tmr500ms >= 50)
    {
        tmr500ms = 0;
        flag500ms = 1;
    }
}
