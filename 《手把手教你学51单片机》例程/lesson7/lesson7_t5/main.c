/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������7�� ��ҵ��5 
* �汾�ţ�v1.0.0
* ��  ע������LED�����9��0������
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code image[11][8] = {
    {0xC3, 0x81, 0x99, 0x99, 0x99, 0x99, 0x81, 0xC3},  //����0
    {0xEF, 0xE7, 0xE3, 0xE7, 0xE7, 0xE7, 0xE7, 0xC3},  //����1
    {0xC3, 0x81, 0x9D, 0x87, 0xC3, 0xF9, 0xC1, 0x81},  //����2
    {0xC3, 0x81, 0x9D, 0xC7, 0xC7, 0x9D, 0x81, 0xC3},  //����3
    {0xCF, 0xC7, 0xC3, 0xC9, 0xC9, 0x81, 0xCF, 0xCF},  //����4
    {0x81, 0xC1, 0xF9, 0xC3, 0x87, 0x9D, 0x81, 0xC3},  //����5
    {0xC3, 0x81, 0xF9, 0xC1, 0x81, 0x99, 0x81, 0xC3},  //����6
    {0x81, 0x81, 0x9F, 0xCF, 0xCF, 0xE7, 0xE7, 0xE7},  //����7
    {0xC3, 0x81, 0x99, 0xC3, 0xC3, 0x99, 0x81, 0xC3},  //����8
    {0xC3, 0x81, 0x99, 0x81, 0x83, 0x9F, 0x83, 0xC1},  //����9
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},  //ȫ��
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
    static unsigned int  tmr = 0;  //1s�����ʱ��
    static unsigned char index = 9;  //ͼƬˢ������

    TH0 = 0xFC;  //���¼��س�ֵ
    TL0 = 0x67;
    //���´������LED����̬ɨ��ˢ��
    P0 = 0xFF;   //��ʾ����
    switch (i)
    {
        case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=image[index][0]; break;
        case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=image[index][1]; break;
        case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=image[index][2]; break;
        case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=image[index][3]; break;
        case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=image[index][4]; break;
        case 5: ADDR2=1; ADDR1=0; ADDR0=1; i++; P0=image[index][5]; break;
        case 6: ADDR2=1; ADDR1=1; ADDR0=0; i++; P0=image[index][6]; break;
        case 7: ADDR2=1; ADDR1=1; ADDR0=1; i=0; P0=image[index][7]; break;
        default: break;
    }
    //���´������ÿ��ı�һ֡ͼ��
    tmr++;
    if (tmr >= 1000)  //�ﵽ1000msʱ�ı�һ��ͼƬ����
    {
        tmr = 0;
        if (index == 0)  //ͼƬ����10~0ѭ��
            index = 10;
        else
            index--;
    }
}
