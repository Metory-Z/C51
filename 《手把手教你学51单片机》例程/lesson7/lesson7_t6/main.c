/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������7�� ��ҵ��6 
* �汾�ţ�v1.0.0
* ��  ע������LED������ܡ�����LEDͬʱȫ�����������ʾ��ͬ���ݵķ������ں�����
*         ���з����õ�����ʱ������������п������ʵ�֡�
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

void main()
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ʹ��LED
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

    TH0 = 0xFC;  //���¼��س�ֵ
    TL0 = 0x67;
    //���´������LED����̬ɨ��ˢ��
    P0 = 0xFF;   //��ʾ����
    switch (i)
    {
        //����LED����
        case 0: ADDR3 = 0; ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 1: ADDR3 = 0; ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=0; break;
        case 2: ADDR3 = 0; ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=0; break;
        case 3: ADDR3 = 0; ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=0; break;
        case 4: ADDR3 = 0; ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 5: ADDR3 = 0; ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=0; break;
        case 6: ADDR3 = 0; ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=0; break;
        case 7: ADDR3 = 0; ADDR2=1; ADDR1=1; ADDR0=1; i++; P0=0; break;
        //����ܲ���
        case 8:  ADDR3 = 1; ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 9:  ADDR3 = 1; ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=0; break;
        case 10: ADDR3 = 1; ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=0; break;
        case 11: ADDR3 = 1; ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=0; break;
        case 12: ADDR3 = 1; ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=0; break;
        case 13: ADDR3 = 1; ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=0; break;
        //����LED����
        case 14: ADDR3 = 1; ADDR2=1; ADDR1=1; ADDR0=0; i=0; P0=0; break;
        default: break;
    }
}
