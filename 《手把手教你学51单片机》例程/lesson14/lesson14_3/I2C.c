/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����I2C.c
* ��  ����I2C��������ģ��
* �汾�ţ�v1.0.0
* ��  ע������IO��ģ��ʵ�֣�����ʱ����ʱ�Ƚ��������ʽʵ��
*******************************************************************************
*/

#include <reg52.h>
#include <intrins.h>

#define I2CDelay()  {_nop_();_nop_();_nop_();_nop_();}
sbit I2C_SCL = P3^7;
sbit I2C_SDA = P3^6;

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

    return (~ack); //Ӧ��ֵȡ���Է���ͨ�����߼���
                   //0=�����ڻ�æ��д��ʧ�ܣ�1=�����ҿ��л�д��ɹ�
}
/* I2C���߶������������ͷ�Ӧ���źţ�����ֵ-�������ֽ� */
unsigned char I2CReadNAK()
{
    unsigned char mask;
    unsigned char dat;

    I2C_SDA = 1;  //����ȷ�������ͷ�SDA
    for (mask=0x80; mask!=0; mask>>=1) //�Ӹ�λ����λ���ν���
    {
        I2CDelay();
        I2C_SCL = 1;      //����SCL
        if(I2C_SDA == 0)  //��ȡSDA��ֵ
            dat &= ~mask; //Ϊ0ʱ��dat�ж�Ӧλ����
        else
            dat |= mask;  //Ϊ1ʱ��dat�ж�Ӧλ��1
        I2CDelay();
        I2C_SCL = 0;      //������SCL����ʹ�ӻ����ͳ���һλ
    }
    I2C_SDA = 1;   //8λ���ݷ����������SDA�����ͷ�Ӧ���ź�
    I2CDelay();
    I2C_SCL = 1;   //����SCL
    I2CDelay();
    I2C_SCL = 0;   //������SCL��ɷ�Ӧ��λ��������ס����

    return dat;
}
/* I2C���߶�������������Ӧ���źţ�����ֵ-�������ֽ� */
unsigned char I2CReadACK()
{
    unsigned char mask;
    unsigned char dat;

    I2C_SDA = 1;  //����ȷ�������ͷ�SDA
    for (mask=0x80; mask!=0; mask>>=1) //�Ӹ�λ����λ���ν���
    {
        I2CDelay();
        I2C_SCL = 1;      //����SCL
        if(I2C_SDA == 0)  //��ȡSDA��ֵ
            dat &= ~mask; //Ϊ0ʱ��dat�ж�Ӧλ����
        else
            dat |= mask;  //Ϊ1ʱ��dat�ж�Ӧλ��1
        I2CDelay();
        I2C_SCL = 0;      //������SCL����ʹ�ӻ����ͳ���һλ
    }
    I2C_SDA = 0;   //8λ���ݷ����������SDA������Ӧ���ź�
    I2CDelay();
    I2C_SCL = 1;   //����SCL
    I2CDelay();
    I2C_SCL = 0;   //������SCL���Ӧ��λ��������ס����

    return dat;
}
