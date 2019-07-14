/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������8�� ���ھ��󰴼��������ʵ�ֵļ��׼ӷ�������
* �汾�ţ�v1.0.0
* ��  ע���������8��8.5��
*******************************************************************************
*/

#include <reg52.h>

sbit ADDR0 = P1^0;
sbit ADDR1 = P1^1;
sbit ADDR2 = P1^2;
sbit ADDR3 = P1^3;
sbit ENLED = P1^4;
sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[6] = {  //�������ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
unsigned char code KeyCodeMap[4][4] = { //���󰴼���ŵ���׼���̼����ӳ���
    { 0x31, 0x32, 0x33, 0x26 }, //���ּ�1�����ּ�2�����ּ�3�����ϼ�
    { 0x34, 0x35, 0x36, 0x25 }, //���ּ�4�����ּ�5�����ּ�6�������
    { 0x37, 0x38, 0x39, 0x28 }, //���ּ�7�����ּ�8�����ּ�9�����¼�
    { 0x30, 0x1B, 0x0D, 0x27 }  //���ּ�0��ESC����  �س����� ���Ҽ�
};
unsigned char KeySta[4][4] = {  //ȫ�����󰴼��ĵ�ǰ״̬
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};

void KeyDriver();

void main()
{
    EA = 1;       //ʹ�����ж�
    ENLED = 0;    //ѡ������ܽ�����ʾ
    ADDR3 = 1;
    TMOD = 0x01;  //����T0Ϊģʽ1
    TH0  = 0xFC;  //ΪT0����ֵ0xFC67����ʱ1ms
    TL0  = 0x67;
    ET0  = 1;     //ʹ��T0�ж�
    TR0  = 1;     //����T0
    LedBuff[0] = LedChar[0];  //�ϵ���ʾ0
	
    while (1)
    {
        KeyDriver();   //���ð�����������
    }
}
/* ��һ���޷��ų����͵�������ʾ��������ϣ�num-����ʾ���� */
void ShowNumber(unsigned long num)
{
    signed char i;
    unsigned char buf[6];
    
    for (i=0; i<6; i++)   //�ѳ�������ת��Ϊ6λʮ���Ƶ�����
    {
        buf[i] = num % 10;
        num = num / 10;
    }
    for (i=5; i>=1; i--)  //�����λ������0ת��Ϊ�ո�������0���˳�ѭ��
    {
        if (buf[i] == 0)
            LedBuff[i] = 0xFF;
        else
            break;
    }
    for ( ; i>=0; i--)    //ʣ���λ����ʵת��Ϊ�������ʾ�ַ�
    {
        LedBuff[i] = LedChar[buf[i]];
    }
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    static unsigned long result = 0;  //���ڱ���������
    static unsigned long addend = 0;  //���ڱ�������ļ���
    
    if ((keycode>=0x30) && (keycode<=0x39))  //����0-9������
    {
        addend = (addend*10)+(keycode-0x30); //����ʮ�������ƣ������ֽ����λ
        ShowNumber(addend);    //��������ʾ�������
    }
    else if (keycode == 0x26)  //���ϼ������Ӻţ�ִ�мӷ�����������
    {
        result += addend;      //���мӷ�����
        addend = 0;
        ShowNumber(result);    //��������ʾ�������
    }
    else if (keycode == 0x0D)  //�س�����ִ�мӷ�����(ʵ��Ч����Ӻ���ͬ)
    {
        result += addend;      //���мӷ�����
        addend = 0;
        ShowNumber(result);    //��������ʾ�������
    }
    else if (keycode == 0x1B)  //Esc����������
    {
        addend = 0;
        result = 0;
        ShowNumber(addend);    //�����ļ�����ʾ�������
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
/* ����ܶ�̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ�������
    
    P0 = 0xFF;   //��ʾ����
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
/* T0�жϷ������������������ʾɨ���밴��ɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = 0xFC;  //���¼��س�ֵ
    TL0 = 0x67;
    LedScan();   //�����������ʾɨ�躯��
    KeyScan();   //���ð���ɨ�躯��
}
