/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� EEPROM���ֽڶ�д����ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������14��14.3.2��
*         �ö��ֽڶ�дģʽ����EEPROM������+1,+2,+3...��д��
*******************************************************************************
*/

#include <reg52.h>

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
void MemToStr(unsigned char *str, unsigned char *src, unsigned char len);

void main()
{
    unsigned char i;
    unsigned char buf[5];
    unsigned char str[20];

    InitLcd1602();   //��ʼ��Һ��
    E2Read(buf, 0x90, sizeof(buf));   //��E2�ж�ȡһ������
    MemToStr(str, buf, sizeof(buf));  //ת��Ϊʮ�������ַ���
    LcdShowStr(0, 0, str);            //��ʾ��Һ����
    for (i=0; i<sizeof(buf); i++)     //��������+1,+2,+3...
    {
        buf[i] = buf[i] + 1 + i;
    }
    E2Write(buf, 0x90, sizeof(buf));  //��д�ص�E2��
    
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
/* E2��ȡ������buf-���ݽ���ָ�룬addr-E2�е���ʼ��ַ��len-��ȡ���� */
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len)
{
    do {                       //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д����
        I2CStart();
        if (I2CWrite(0x50<<1)) //Ӧ��������ѭ������Ӧ���������һ�β�ѯ
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);            //д����ʼ��ַ
    I2CStart();                //�����ظ������ź�
    I2CWrite((0x50<<1)|0x01);  //Ѱַ����������Ϊ������
    while (len > 1)            //������ȡlen-1���ֽ�
    {
        *buf++ = I2CReadACK(); //����ֽ�֮ǰΪ��ȡ����+Ӧ��
        len--;
    }
    *buf = I2CReadNAK();       //���һ���ֽ�Ϊ��ȡ����+��Ӧ��
    I2CStop();
}
/* E2д�뺯����buf-Դ����ָ�룬addr-E2�е���ʼ��ַ��len-д�볤�� */
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
    while (len--)
    {
        do {                       //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д����
            I2CStart();
            if (I2CWrite(0x50<<1)) //Ӧ��������ѭ������Ӧ���������һ�β�ѯ
            {
                break;
            }
            I2CStop();
        } while(1);
        I2CWrite(addr++);  //д����ʼ��ַ
        I2CWrite(*buf++);  //д��һ���ֽ�����
        I2CStop();         //����д�������Եȴ�д�����
    }
}
