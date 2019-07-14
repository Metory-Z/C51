/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� EEPROM���������ҳд����ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������14��14.3.3��
*         �����������ҳдģʽ����EEPROM������+1,+2,+3...��д��
*******************************************************************************
*/

#include <reg52.h>

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len);

void main()
{
    unsigned char i;
    unsigned char buf[5];
    unsigned char str[20];

    InitLcd1602();   //��ʼ��Һ��
    E2Read(buf, 0x8E, sizeof(buf));   //��E2�ж�ȡһ������
    MemToStr(str, buf, sizeof(buf));  //ת��Ϊʮ�������ַ���
    LcdShowStr(0, 0, str);            //��ʾ��Һ����
    for (i=0; i<sizeof(buf); i++)     //��������+1,+2,+3...
    {
        buf[i] = buf[i] + 1 + i;
    }
    E2Write(buf, 0x8E, sizeof(buf));  //��д�ص�E2��
    
    while(1);
}
/* ��һ���ڴ�����ת��Ϊʮ�����Ƹ�ʽ���ַ�����
   str-�ַ���ָ�룬src-Դ���ݵ�ַ��len-���ݳ��� */
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len)
{
    unsigned char tmp;

    while (len--)
    {
        tmp = *src >> 4;           //��ȡ��4λ
        if (tmp <= 9)              //ת��Ϊ0-9��A-F
            *str++ = tmp + '0';
        else
            *str++ = tmp - 10 + 'A';
        tmp = *src & 0x0F;         //��ȡ��4λ
        if (tmp <= 9)              //ת��Ϊ0-9��A-F
            *str++ = tmp + '0';
        else
            *str++ = tmp - 10 + 'A';
        *str++ = ' ';              //ת����һ���ֽ����һ���ո�
        src++;
    }
}
