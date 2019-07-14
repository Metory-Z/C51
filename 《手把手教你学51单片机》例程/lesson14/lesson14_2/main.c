/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� EEPROM���ֽڶ�д����
* �汾�ţ�v1.0.0
* ��  ע���������14��14.3.1��
*         �õ��ֽڶ�дģʽ����EEPROM��ÿ��+1��д��
*******************************************************************************
*/

#include <reg52.h>

extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);
unsigned char E2ReadByte(unsigned char addr);
void E2WriteByte(unsigned char addr, unsigned char dat);

void main()
{
    unsigned char dat;
    unsigned char str[10];

    InitLcd1602();   //��ʼ��Һ��
    dat = E2ReadByte(0x02);    //��ȡָ����ַ�ϵ�һ���ֽ�
    str[0] = (dat/100) + '0';  //ת��Ϊʮ�����ַ�����ʽ
    str[1] = (dat/10%10) + '0';
    str[2] = (dat%10) + '0';
    str[3] = '\0';
    LcdShowStr(0, 0, str);     //��ʾ��Һ����
    dat++;                     //������ֵ+1
    E2WriteByte(0x02, dat);    //��д�ص���Ӧ�ĵ�ַ��
    
    while (1);
}

/* ��ȡEEPROM�е�һ���ֽڣ�addr-�ֽڵ�ַ */
unsigned char E2ReadByte(unsigned char addr)
{
    unsigned char dat;
    
    I2CStart();
    I2CWrite(0x50<<1); //Ѱַ����������Ϊд����
    I2CWrite(addr);    //д��洢��ַ
    I2CStart();        //�����ظ������ź�
    I2CWrite((0x50<<1)|0x01); //Ѱַ����������Ϊ������
    dat = I2CReadNAK();       //��ȡһ���ֽ�����
    I2CStop();
    
    return dat;
}
/* ��EEPROM��д��һ���ֽڣ�addr-�ֽڵ�ַ */
void E2WriteByte(unsigned char addr, unsigned char dat)
{
    I2CStart();
    I2CWrite(0x50<<1); //Ѱַ����������Ϊд����
    I2CWrite(addr);    //д��洢��ַ
    I2CWrite(dat);     //д��һ���ֽ�����
    I2CStop();
}
