/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������16�� ��ҵ��2 ����ң�������Ƶ��ת��
* �汾�ţ�v1.0.0
* ��  ע����ң�����ϵļӺźͼ��ż��ֱ���Ʋ��������ת��תһ�ܡ�
*         ��ͬң�����ļ���ֵ���ܲ�ͬ������ȷ��ÿ�����ļ���ֵ��ʹ��lesson16_1��
*         ʾ������������Ҷ���ʾ����ֵ���Ǽ���ֵ��ʮ�����Ƹ�ʽ��ʾ����
*******************************************************************************
*/

#include <reg52.h>

signed long beats = 0;  //���ת����������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

extern bit irflag;
extern unsigned char ircode[4];
extern void InitInfrared();
void ConfigTimer0(unsigned int ms);
void StartMotor(signed long angle);
void StopMotor();

void main()
{
    EA = 1;      //�����ж�
    InitInfrared();   //��ʼ�����⹦��
    ConfigTimer0(2);  //����T0��ʱ2ms
    
    while (1)
    {
        if (irflag)  //���յ���������ʱˢ����ʾ
        {
            irflag = 0;
            if (ircode[2] == 0x09)  //0x09Ϊ+���룬��ת1Ȧ
            {
                StartMotor(360);
            }
            else if (ircode[2] == 0x15)  //0x15Ϊ-���룬��ת1Ȧ
            {
                StartMotor(-360);
            }
        }
    }
}
/* �����������������angle-��ת���ĽǶ� */
void StartMotor(signed long angle)
{
    //�ڼ���ǰ�ر��жϣ���ɺ��ٴ򿪣��Ա����жϴ�ϼ�����̶���ɴ���
    EA = 0;
    beats = (angle * 4076) / 360; //ʵ��Ϊ4076��ת��һȦ
    EA = 1;
}
/* �������ֹͣ���� */
void StopMotor()
{
    EA = 0;
    beats = 0;
    EA = 1;
}
/* ���ת�����ƺ��� */
void TurnMotor()
{
    unsigned char tmp;  //��ʱ����
    static unsigned char index = 0;  //�����������
    unsigned char code BeatCode[8] = {  //����������Ķ�Ӧ��IO���ƴ���
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };
    
    if (beats != 0)  //��������Ϊ0�����һ����������
    {
        if (beats > 0)  //����������0ʱ��ת
        {
            index++;               //��תʱ���������������
            index = index & 0x07;  //��&����ʵ�ֵ�8����
            beats--;               //��תʱ���ļ����ݼ�
        }
        else            //������С��0ʱ��ת
        {
            index--;               //��תʱ������������ݼ�
            index = index & 0x07;  //��&����ͬ������ʵ�ֵ�-1ʱ��7
            beats++;               //��תʱ���ļ�������
        }
        tmp = P1;                    //��tmp��P1�ڵ�ǰֵ�ݴ�
        tmp = tmp & 0xF0;            //��&���������4λ
        tmp = tmp | BeatCode[index]; //��|�����ѽ��Ĵ���д����4λ
        P1  = tmp;                   //�ѵ�4λ�Ľ��Ĵ���͸�4λ��ԭֵ�ͻ�P1
    }
    else  //������Ϊ0��رյ�����е���
    {
        P1 = P1 | 0x0F;
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 13;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�������ɨ����ʾ */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    TurnMotor(); //ִ�е������
}
