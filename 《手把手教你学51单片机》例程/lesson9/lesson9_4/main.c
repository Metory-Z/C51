/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������9�� �������Ʋ������ת����ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������9��9.3.6��
*******************************************************************************
*/

#include <reg52.h>

sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
    { 0x31, 0x32, 0x33, 0x26 }, //���ּ�1�����ּ�2�����ּ�3�����ϼ�
    { 0x34, 0x35, 0x36, 0x25 }, //���ּ�4�����ּ�5�����ּ�6�������
    { 0x37, 0x38, 0x39, 0x28 }, //���ּ�7�����ּ�8�����ּ�9�����¼�
    { 0x30, 0x1B, 0x0D, 0x27 }  //���ּ�0��ESC����  �س����� ���Ҽ�
};
unsigned char KeySta[4][4] = {  //ȫ�����󰴼��ĵ�ǰ״̬
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};
signed long beats = 0;  //���ת����������

void KeyDriver();

void main()
{
    EA = 1;       //ʹ�����ж�
    TMOD = 0x01;  //����T0Ϊģʽ1
    TH0  = 0xFC;  //ΪT0����ֵ0xFC67����ʱ1ms
    TL0  = 0x67;
    ET0  = 1;     //ʹ��T0�ж�
    TR0  = 1;     //����T0
	
    while (1)
    {
        KeyDriver();   //���ð�����������
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
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    static bit dirMotor = 0;  //���ת������
    
    if ((keycode>=0x30) && (keycode<=0x39))  //���Ƶ��ת��1-9Ȧ
    {
        if (dirMotor == 0)
            StartMotor(360*(keycode-0x30));
        else
            StartMotor(-360*(keycode-0x30));
    }
    else if (keycode == 0x26)  //���ϼ�������ת������Ϊ��ת
    {
        dirMotor = 0;
    }
    else if (keycode == 0x28)  //���¼�������ת������Ϊ��ת
    {
        dirMotor = 1;
    }
    else if (keycode == 0x25)  //��������̶���ת90��
    {
        StartMotor(90);
    }
    else if (keycode == 0x27)  //���Ҽ����̶���ת90��
    {
        StartMotor(-90);
    }
    else if (keycode == 0x1B)  //Esc����ֹͣת��
    {
        StopMotor();
    }
}
/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char backup[4][4] = {  //����ֵ���ݣ�����ǰһ�ε�ֵ
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    
    for (i=0; i<4; i++)  //ѭ�����4*4�ľ��󰴼�
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])    //��ⰴ������
            {
                if (backup[i][j] != 0)           //��������ʱִ�ж���
                {
                    KeyAction(KeyCodeMap[i][j]); //���ð�����������
                }
                backup[i][j] = KeySta[i][j];     //ˢ��ǰһ�εı���ֵ
            }
        }
    }
}
/* ����ɨ�躯�������ڶ�ʱ�ж��е��ã��Ƽ����ü��1ms */
void KeyScan()
{
    unsigned char i;
    static unsigned char keyout = 0;   //���󰴼�ɨ���������
    static unsigned char keybuf[4][4] = {  //���󰴼�ɨ�軺����
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
    };

    //��һ�е�4������ֵ���뻺����
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //��������°���״̬
    for (i=0; i<4; i++)  //ÿ��4������������ѭ��4��
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   //����4��ɨ��ֵΪ0����4*4ms�ڶ��ǰ���״̬ʱ������Ϊ�������ȶ��İ���
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //����4��ɨ��ֵΪ1����4*4ms�ڶ��ǵ���״̬ʱ������Ϊ�������ȶ��ĵ���
            KeySta[keyout][i] = 1;
        }
    }
    //ִ����һ�ε�ɨ�����
    keyout++;                //�����������
    keyout = keyout & 0x03;  //����ֵ�ӵ�4������
    switch (keyout)          //�����������ͷŵ�ǰ������ţ������´ε��������
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
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
/* T0�жϷ����������ڰ���ɨ������ת������ */
void InterruptTimer0() interrupt 1
{
    static bit div = 0;
    
    TH0 = 0xFC;  //���¼��س�ֵ
    TL0 = 0x67;
    KeyScan();   //ִ�а���ɨ��
    //��һ����̬bit����ʵ�ֶ���Ƶ����2ms��ʱ�����ڿ��Ƶ��
    div = ~div;
    if (div == 1)
    {
        TurnMotor();
    }
}
