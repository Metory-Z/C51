/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������13�� ����1602Һ���ļ��׼�����ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������13��13.4��
*         ʵ��������Ϊ�������ļӼ��˳�������
*******************************************************************************
*/

#include <reg52.h>

unsigned char step = 0;  //��������
unsigned char oprt = 0;  //��������
signed long num1 = 0;    //������1
signed long num2 = 0;    //������2
signed long result = 0;  //������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);
extern void LcdFullClear();

void main()
{
    EA = 1;           //�����ж�
    ConfigTimer0(1);  //����T0��ʱ1ms
	InitLcd1602();    //��ʼ��Һ��
    LcdShowStr(15, 1, "0");  //��ʼ��ʾһ������0
    
    while (1)
    {
        KeyDriver();  //���ð�������
    }
}
/* ��������ת��Ϊ�ַ�����str-�ַ���ָ�룬dat-��ת����������ֵ-�ַ������� */
unsigned char LongToString(unsigned char *str, signed long dat)
{
    signed char i = 0;
    unsigned char len = 0;
    unsigned char buf[12];
    
    if (dat < 0)  //���Ϊ����������ȡ����ֵ������ָ������Ӹ���
    {
        dat = -dat;
        *str++ = '-';
        len++;
    }
    do {          //��ת��Ϊ��λ��ǰ��ʮ��������
        buf[i++] = dat % 10;
        dat /= 10;
    } while (dat > 0);
    len += i;     //i����ֵ������Ч�ַ��ĸ���
    while (i-- > 0)   //������ֵת��ΪASCII�뷴�򿽱�������ָ����
    {
        *str++ = buf[i] + '0';
    }
    *str = '\0';  //����ַ���������
    
    return len;   //�����ַ�������
}
/* ��ʾ���������ʾλ��y�����������type */
void ShowOprt(unsigned char y, unsigned char type)
{
    switch (type)
    {
        case 0: LcdShowStr(0, y, "+"); break;  //0����+
        case 1: LcdShowStr(0, y, "-"); break;  //1����-
        case 2: LcdShowStr(0, y, "*"); break;  //2����*
        case 3: LcdShowStr(0, y, "/"); break;  //3����/
        default: break;
    }
}
/* ��������λ���������ֵ�������Ļ��ʾ */
void Reset()
{
    num1 = 0;
    num2 = 0;
    step = 0;
    LcdFullClear();
}
/* ���ּ�����������n-�����������ֵ */
void NumKeyAction(unsigned char n)
{
    unsigned char len;
    unsigned char str[12];
    
    if (step > 1)  //���������ɣ������¿�ʼ�µļ���
    {
        Reset();
    }
    if (step == 0)  //�����һ������
    {
        num1 = num1*10 + n;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString(str, num1);  //����ֵת��Ϊ�ַ���
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
    else            //����ڶ�������
    {
        num2 = num2*10 + n;             //������ֵ�ۼӵ�ԭ��������
        len = LongToString(str, num2);  //����ֵת��Ϊ�ַ���
        LcdShowStr(16-len, 1, str);     //��ʾ��Һ���ڶ�����
    }
}
/* ����������������������������type */
void OprtKeyAction(unsigned char type)
{
    unsigned char len;
    unsigned char str[12];
    
    if (step == 0)  //�ڶ���������δ����ʱ��Ӧ������֧����������
    {
        len = LongToString(str, num1); //��һ������ת��Ϊ�ַ���
        LcdAreaClear(0, 0, 16-len);    //�����һ����ߵ��ַ�λ
        LcdShowStr(16-len, 0, str);    //�ַ���������ʾ�ڵ�һ��
        ShowOprt(1, type);             //�ڵڶ�����ʾ������
        LcdAreaClear(1, 1, 14);        //����ڶ����м���ַ�λ
        LcdShowStr(15, 1, "0");        //�ڵڶ������Ҷ���ʾ0
        oprt = type;                   //��¼��������
        step = 1;
    }
}
/* ���������� */
void GetResult()
{
    unsigned char len;
    unsigned char str[12];
    
    if (step == 1) //�ڶ�������������ʱ��ִ�м���
    {
        step = 2;
        switch (oprt)  //������������ͼ�������δ�����������
        {
            case 0: result = num1 + num2; break;
            case 1: result = num1 - num2; break;
            case 2: result = num1 * num2; break;
            case 3: result = num1 / num2; break;
            default: break;
        }
        len = LongToString(str, num2);  //ԭ�ڶ����������������ʾ����һ��
        ShowOprt(0, oprt);
        LcdAreaClear(1, 0, 16-1-len);
        LcdShowStr(16-len, 0, str);
        len = LongToString(str, result);  //�������͵Ⱥ���ʾ�ڵڶ���
        LcdShowStr(0, 1, "=");
        LcdAreaClear(1, 1, 16-1-len);
        LcdShowStr(16-len, 1, str);
    }
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    if  ((keycode>='0') && (keycode<='9'))  //�����ַ�
    {
        NumKeyAction(keycode - '0');
    }
    else if (keycode == 0x26)  //���ϼ���+
    {
        OprtKeyAction(0);
    }
    else if (keycode == 0x28)  //���¼���-
    {
        OprtKeyAction(1);
    }
    else if (keycode == 0x25)  //�������*
    {
        OprtKeyAction(2);
    }
    else if (keycode == 0x27)  //���Ҽ�����
    {
        OprtKeyAction(3);
    }
    else if (keycode == 0x0D)  //�س�����������
    {
        GetResult();
    }
    else if (keycode == 0x1B)  //Esc�������
    {
        Reset();
        LcdShowStr(15, 1, "0");
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 28;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* T0�жϷ�������ִ�а���ɨ�� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    KeyScan();   //����ɨ��
}
