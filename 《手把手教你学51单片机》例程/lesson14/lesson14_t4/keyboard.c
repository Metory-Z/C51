/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����keyboard.c
* ��  ����4*4���󰴼�����ģ��
* �汾�ţ�v1.0.0
* ��  ע��
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
    { '1',  '2',  '3', 0x26 }, //���ּ�1�����ּ�2�����ּ�3�����ϼ�
    { '4',  '5',  '6', 0x25 }, //���ּ�4�����ּ�5�����ּ�6�������
    { '7',  '8',  '9', 0x28 }, //���ּ�7�����ּ�8�����ּ�9�����¼�
    { '0', 0x1B, 0x0D, 0x27 }  //���ּ�0��ESC����  �س����� ���Ҽ�
};
unsigned char pdata KeySta[4][4] = {  //ȫ�����󰴼��ĵ�ǰ״̬
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

extern void KeyAction(unsigned char keycode);

/* ����������������ⰴ��������������Ӧ����������������ѭ���е��� */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char pdata backup[4][4] = {  //����ֵ���ݣ�����ǰһ�ε�ֵ
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
    keyout++;         //�����������
    keyout &= 0x03;   //����ֵ�ӵ�4������
    switch (keyout)   //�����������ͷŵ�ǰ������ţ������´ε��������
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}
