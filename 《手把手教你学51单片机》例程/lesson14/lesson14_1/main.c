/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� I2CѰַʾ��
* �汾�ţ�v1.0.0
* ��  ע���������14��14.2��
*         ѰַI2C�����ϴ��ڵĺͲ����ڵĵ�ַ����Ӧ��״̬��ʾ��Һ����
*******************************************************************************
*/

#include <reg52.h>
#include <intrins.h>

#define I2CDelay()  {_nop_();_nop_();_nop_();_nop_();}
sbit I2C_SCL = P3^7;
sbit I2C_SDA = P3^6;

bit I2CAddressing(unsigned char addr);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    bit ack;
    unsigned char str[10];

    InitLcd1602();   //��ʼ��Һ��
    
    ack = I2CAddressing(0x50); //��ѯ��ַΪ0x50������
    str[0] = '5';              //����ַ��Ӧ��ֵת��Ϊ�ַ���
    str[1] = '0';
    str[2] = ':';
    str[3] = (unsigned char)ack + '0';
    str[4] = '\0';
    LcdShowStr(0, 0, str);     //��ʾ��Һ����
    
    ack = I2CAddressing(0x62); //��ѯ��ַΪ0x62������
    str[0] = '6';              //����ַ��Ӧ��ֵת��Ϊ�ַ���
    str[1] = '2';
    str[2] = ':';
    str[3] = (unsigned char)ack + '0';
    str[4] = '\0';
    LcdShowStr(8, 0, str);     //��ʾ��Һ����
    
    while (1);
}
/* ����������ʼ�ź� */
void I2CStart()
{
    I2C_SDA = 1; //����ȷ��SDA��SCL���Ǹߵ�ƽ
    I2C_SCL = 1;
    I2CDelay();
    I2C_SDA = 0; //������SDA
    I2CDelay();
    I2C_SCL = 0; //������SCL
}
/* ��������ֹͣ�ź� */
void I2CStop()
{
    I2C_SCL = 0; //����ȷ��SDA��SCL���ǵ͵�ƽ
    I2C_SDA = 0;
    I2CDelay();
    I2C_SCL = 1; //������SCL
    I2CDelay();
    I2C_SDA = 1; //������SDA
    I2CDelay();
}
/* I2C����д������dat-��д���ֽڣ�����ֵ-�ӻ�Ӧ��λ��ֵ */
bit I2CWrite(unsigned char dat)
{
    bit ack;  //�����ݴ�Ӧ��λ��ֵ
    unsigned char mask;  //����̽���ֽ���ĳһλֵ���������

    for (mask=0x80; mask!=0; mask>>=1) //�Ӹ�λ����λ���ν���
    {
        if ((mask&dat) == 0)  //��λ��ֵ�����SDA��
            I2C_SDA = 0;
        else
            I2C_SDA = 1;
        I2CDelay();
        I2C_SCL = 1;          //����SCL
        I2CDelay();
        I2C_SCL = 0;          //������SCL�����һ��λ����
    }
    I2C_SDA = 1;   //8λ���ݷ�����������ͷ�SDA���Լ��ӻ�Ӧ��
    I2CDelay();
    I2C_SCL = 1;   //����SCL
    ack = I2C_SDA; //��ȡ��ʱ��SDAֵ����Ϊ�ӻ���Ӧ��ֵ
    I2CDelay();
    I2C_SCL = 0;   //������SCL���Ӧ��λ��������ס����

    return ack;    //���شӻ�Ӧ��ֵ
}
/* I2CѰַ������������ַΪaddr�������Ƿ���ڣ�����ֵ-������Ӧ��ֵ */
bit I2CAddressing(unsigned char addr)
{
    bit ack;

    I2CStart();  //������ʼλ��������һ�����߲���
    ack = I2CWrite(addr<<1);  //������ַ������һλ����Ѱַ��������λ
                              //Ϊ��дλ�����ڱ�ʾ֮��Ĳ����Ƕ���д
    I2CStop();   //������к�����д����ֱ��ֹͣ�������߲���
    
    return ack;
}
