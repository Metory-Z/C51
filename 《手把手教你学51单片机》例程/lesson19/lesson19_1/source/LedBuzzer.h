/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����LedBuzzer.h
* ��  ��������LED������ܡ�����LED����Դ������������ģ��ͷ�ļ�
* �汾�ţ�v1.0.0
* ��  ע��
*******************************************************************************
*/

#ifndef _LED_BUZZER_H
#define _LED_BUZZER_H

struct sLedBuff {  //LED��ʾ�������ṹ
    uint8 array[8];   //���󻺳���
    uint8 number[6];  //����ܻ�����
    uint8 alone;      //����LED������
};

#ifndef _LED_BUZZER_C
extern bit staBuzzer;
extern struct sLedBuff ledBuff;
#endif

void InitLed();
void FlowingLight();
void ShowLedNumber(uint8 index, uint8 num, uint8 point);
void ShowLedArray(uint8 *ptr);

#endif
