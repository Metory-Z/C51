/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������7�� ����LED�����ϵ�ȫ����
* �汾�ţ�v1.0.0
* ��  ע���������7��7.3��
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
    ENLED = 0;    //ʹ��U4��ѡ��LED����
    ADDR3 = 0;    //��Ϊ��Ҫ��̬�ı�ADDR0-2��ֵ�����Բ���Ҫ�ٳ�ʼ����
    TMOD = 0x01;  //����T0Ϊģʽ1
    TH0  = 0xFC;  //ΪT0����ֵ0xFC67����ʱ1ms
    TL0  = 0x67;
    ET0  = 1;     //ʹ��T0�ж�
    TR0  = 1;     //����T0
    while (1);    //����ͣ������ȴ���ʱ���ж�
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
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=0x00; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=0x00; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=0x00; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=0x00; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=0x00; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=0x00; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=0x00; break;
        case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=0x00; break;
        default: break;
    }
}
