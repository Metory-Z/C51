/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������6�� ����ܶ�̬��ʾԭ��ʾ��(switch...case...���ʾ��)
* �汾�ţ�v1.0.0
* ��  ע���������6��6.4��
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //�������ʾ����������ֵ0xFFȷ������ʱ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};

void main()
{
    unsigned char i = 0;    //��̬ɨ�������
    unsigned int  cnt = 0;  //��¼T0�жϴ���
    unsigned long sec = 0;  //��¼����������

    ENLED = 0;    //ʹ��U3��ѡ����������
    ADDR3 = 1;    //��Ϊ��Ҫ��̬�ı�ADDR0-2��ֵ�����Բ���Ҫ�ٳ�ʼ����
    TMOD = 0x01;  //����T0Ϊģʽ1
    TH0  = 0xFC;  //ΪT0����ֵ0xFC67����ʱ1ms
    TL0  = 0x67;
    TR0  = 1;     //����T0
    
    while (1)
    {
        if (TF0 == 1)         //�ж�T0�Ƿ����
        {
            TF0 = 0;          //T0����������жϱ�־
            TH0 = 0xFC;       //�����¸���ֵ
            TL0 = 0x67;
            cnt++;            //����ֵ�Լ�1
            if (cnt >= 1000)  //�ж�T0����Ƿ�ﵽ1000��
            {
                cnt = 0;      //�ﵽ1000�κ����ֵ����
                sec++;        //������Լ�1
                //���´��뽫sec��ʮ����λ�ӵ͵���������ȡ��תΪ�������ʾ�ַ�
                LedBuff[0] = LedChar[sec%10];
                LedBuff[1] = LedChar[sec/10%10];
                LedBuff[2] = LedChar[sec/100%10];
                LedBuff[3] = LedChar[sec/1000%10];
                LedBuff[4] = LedChar[sec/10000%10];
                LedBuff[5] = LedChar[sec/100000%10];
            }
            //���´����������ܶ�̬ɨ��ˢ��
            switch (i)
            {
                case 0: ADDR2=0; ADDR1=0; ADDR0=0; i++; P0=LedBuff[0]; break;
                case 1: ADDR2=0; ADDR1=0; ADDR0=1; i++; P0=LedBuff[1]; break;
                case 2: ADDR2=0; ADDR1=1; ADDR0=0; i++; P0=LedBuff[2]; break;
                case 3: ADDR2=0; ADDR1=1; ADDR0=1; i++; P0=LedBuff[3]; break;
                case 4: ADDR2=1; ADDR1=0; ADDR0=0; i++; P0=LedBuff[4]; break;
                case 5: ADDR2=1; ADDR1=0; ADDR0=1; i=0; P0=LedBuff[5]; break;
                default: break;
            }
        }
    }
}
