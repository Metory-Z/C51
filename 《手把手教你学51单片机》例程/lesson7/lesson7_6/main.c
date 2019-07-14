/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������7�� LED������ʾ�����ƶ��Ķ���
* �汾�ţ�v1.0.0
* ��  ע���������7��7.5.1��
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code image[] = {  //ͼƬ����ģ��
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xC3,0xE7,0xE7,0xE7,0xE7,0xE7,0xC3,0xFF,
    0x99,0x00,0x00,0x00,0x81,0xC3,0xE7,0xFF,
    0x99,0x99,0x99,0x99,0x99,0x81,0xC3,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF
};

void main()
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ʹ��U4��ѡ��LED����
    ADDR3 = 0;
    TMOD = 0x01;  //����T0Ϊģʽ1
    TH0  = 0xFC;  //ΪT0����ֵ0xFC67����ʱ1ms
    TL0  = 0x67;
    ET0  = 1;     //ʹ��T0�ж�
    TR0  = 1;     //����T0
    while (1);
}
/* ��ʱ��0�жϷ����� */
void InterruptTimer0() interrupt 1
{
    static unsigned char i = 0;  //��̬ɨ�������
    static unsigned char tmr = 0;  //250ms�����ʱ��
    static unsigned char index = 0;  //ͼƬˢ������

    TH0 = 0xFC;  //���¼��س�ֵ
    TL0 = 0x67;
    //���´������LED����̬ɨ��ˢ��
    P0 = 0xFF;   //��ʾ����
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image[index+0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image[index+1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image[index+2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image[index+3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image[index+4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image[index+5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image[index+6]; break;
        case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image[index+7]; break;
        default: break;
    }
    //���´������ÿ250ms�ı�һ֡ͼ��
    tmr++;
    if (tmr >= 250)  //�ﵽ250msʱ�ı�һ��ͼƬ����
    {
        tmr = 0;
        index++;
        if (index >= 32)  //ͼƬ�����ﵽ32�����
        {
            index = 0;
        }
    }
}
