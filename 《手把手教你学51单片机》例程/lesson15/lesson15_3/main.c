/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������15�� ��DS1302ʵ�ּ��׵�����
* �汾�ţ�v1.0.0
* ��  ע���������15��15.5��
*         ��Һ����ʾ����ʱ�䣬����ͨ������Уʱ
*******************************************************************************
*/

#include <reg52.h>

struct sTime {  //����ʱ��ṹ�嶨��
    unsigned int  year;
    unsigned char mon;
    unsigned char day;
    unsigned char hour;
    unsigned char min;
    unsigned char sec;
    unsigned char week;
};

bit flag200ms = 1;  //200ms��ʱ��־
struct sTime bufTime;  //����ʱ�仺����
unsigned char setIndex = 0;  //ʱ����������
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
void RefreshTimeShow();
extern void InitDS1302();
extern void GetRealTime(struct sTime *time);
extern void SetRealTime(struct sTime *time);
extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdSetCursor(unsigned char x, unsigned char y);
extern void LcdOpenCursor();
extern void LcdCloseCursor();

void main()
{
    unsigned char psec=0xAA;  //�뱸�ݣ���ֵAAȷ���״ζ�ȡʱ����ˢ����ʾ

    EA = 1;           //�����ж�
    ConfigTimer0(1);  //T0��ʱ1ms
    InitDS1302();     //��ʼ��ʵʱʱ��
    InitLcd1602();    //��ʼ��Һ��
    
    //��ʼ����Ļ�Ϲ̶����������
    LcdShowStr(3, 0, "20  -  -  ");
    LcdShowStr(4, 1, "  :  :  ");
    
    while (1)
    {
        KeyDriver();  //���ð�������
        if (flag200ms && (setIndex == 0))
        {                             //ÿ��200ms��δ��������״̬ʱ��
            flag200ms = 0;
            GetRealTime(&bufTime);    //��ȡ��ǰʱ��
            if (psec != bufTime.sec)  //��⵽ʱ���б仯ʱˢ����ʾ
            {
                RefreshTimeShow();
                psec = bufTime.sec;   //�õ�ǰֵ�����ϴ�����
            }
        }
    }
}
/* ��һ��BCD���ֽ���ʾ����Ļ�ϣ�(x,y)-��Ļ��ʼ���꣬bcd-����ʾBCD�� */
void ShowBcdByte(unsigned char x, unsigned char y, unsigned char bcd)
{
    unsigned char str[4];
    
    str[0] = (bcd >> 4) + '0';
    str[1] = (bcd&0x0F) + '0';
    str[2] = '\0';
    LcdShowStr(x, y, str);
}
/* ˢ������ʱ�����ʾ */
void RefreshTimeShow()
{
    ShowBcdByte(5,  0, bufTime.year);
    ShowBcdByte(8,  0, bufTime.mon);
    ShowBcdByte(11, 0, bufTime.day);
    ShowBcdByte(4,  1, bufTime.hour);
    ShowBcdByte(7,  1, bufTime.min);
    ShowBcdByte(10, 1, bufTime.sec);
}
/* ˢ�µ�ǰ����λ�Ĺ��ָʾ */
void RefreshSetShow()
{
    switch (setIndex)
    {
        case 1:  LcdSetCursor(5,  0); break;
        case 2:  LcdSetCursor(6,  0); break;
        case 3:  LcdSetCursor(8,  0); break;
        case 4:  LcdSetCursor(9,  0); break;
        case 5:  LcdSetCursor(11, 0); break;
        case 6:  LcdSetCursor(12, 0); break;
        case 7:  LcdSetCursor(4,  1); break;
        case 8:  LcdSetCursor(5,  1); break;
        case 9:  LcdSetCursor(7,  1); break;
        case 10: LcdSetCursor(8,  1); break;
        case 11: LcdSetCursor(10, 1); break;
        case 12: LcdSetCursor(11, 1); break;
        default:  break;
    }
}
/* ����һ��BCD��ĸ�λ */
unsigned char IncBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) < 0x90)
        bcd += 0x10;
    else
        bcd &= 0x0F;
    
    return bcd;
}
/* ����һ��BCD��ĵ�λ */
unsigned char IncBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) < 0x09)
        bcd += 0x01;
    else
        bcd &= 0xF0;
    
    return bcd;
}
/* �ݼ�һ��BCD��ĸ�λ */
unsigned char DecBcdHigh(unsigned char bcd)
{
    if ((bcd&0xF0) > 0x00)
        bcd -= 0x10;
    else
        bcd |= 0x90;
    
    return bcd;
}
/* �ݼ�һ��BCD��ĵ�λ */
unsigned char DecBcdLow(unsigned char bcd)
{
    if ((bcd&0x0F) > 0x00)
        bcd -= 0x01;
    else
        bcd |= 0x09;
    
    return bcd;
}
/* ����ʱ�䵱ǰ����λ��ֵ */
void IncSetTime()
{
    switch (setIndex)
    {
        case 1:  bufTime.year = IncBcdHigh(bufTime.year); break;
        case 2:  bufTime.year = IncBcdLow(bufTime.year);  break;
        case 3:  bufTime.mon  = IncBcdHigh(bufTime.mon);  break;
        case 4:  bufTime.mon  = IncBcdLow(bufTime.mon);   break;
        case 5:  bufTime.day  = IncBcdHigh(bufTime.day);  break;
        case 6:  bufTime.day  = IncBcdLow(bufTime.day);   break;
        case 7:  bufTime.hour = IncBcdHigh(bufTime.hour); break;
        case 8:  bufTime.hour = IncBcdLow(bufTime.hour);  break;
        case 9:  bufTime.min  = IncBcdHigh(bufTime.min);  break;
        case 10: bufTime.min  = IncBcdLow(bufTime.min);   break;
        case 11: bufTime.sec  = IncBcdHigh(bufTime.sec);  break;
        case 12: bufTime.sec  = IncBcdLow(bufTime.sec);   break;
        default:  break;
    }
    RefreshTimeShow();
    RefreshSetShow();
}
/* �ݼ�ʱ�䵱ǰ����λ��ֵ */
void DecSetTime()
{
    switch (setIndex)
    {
        case 1:  bufTime.year = DecBcdHigh(bufTime.year); break;
        case 2:  bufTime.year = DecBcdLow(bufTime.year);  break;
        case 3:  bufTime.mon  = DecBcdHigh(bufTime.mon);  break;
        case 4:  bufTime.mon  = DecBcdLow(bufTime.mon);   break;
        case 5:  bufTime.day  = DecBcdHigh(bufTime.day);  break;
        case 6:  bufTime.day  = DecBcdLow(bufTime.day);   break;
        case 7:  bufTime.hour = DecBcdHigh(bufTime.hour); break;
        case 8:  bufTime.hour = DecBcdLow(bufTime.hour);  break;
        case 9:  bufTime.min  = DecBcdHigh(bufTime.min);  break;
        case 10: bufTime.min  = DecBcdLow(bufTime.min);   break;
        case 11: bufTime.sec  = DecBcdHigh(bufTime.sec);  break;
        case 12: bufTime.sec  = DecBcdLow(bufTime.sec);   break;
        default:  break;
    }
    RefreshTimeShow();
    RefreshSetShow();
}
/* ����ʱ������λ */
void RightShiftTimeSet()
{
    if (setIndex != 0)
    {
        if (setIndex < 12)
            setIndex++;
        else
            setIndex = 1;
        RefreshSetShow();
    }
}
/* ����ʱ������λ */
void LeftShiftTimeSet()
{
    if (setIndex != 0)
    {
        if (setIndex > 1)
            setIndex--;
        else
            setIndex = 12;
        RefreshSetShow();
    }
}
/* ����ʱ������״̬ */
void EnterTimeSet()
{
    setIndex = 2;       //��������������Ϊ2�����ɽ�������״̬
    LeftShiftTimeSet(); //�������ֳɵ����Ʋ����Ƶ�λ��1�������ʾˢ��
    LcdOpenCursor();    //�򿪹����˸Ч��
}
/* �˳�ʱ������״̬��save-�Ƿ񱣴浱ǰ���õ�ʱ��ֵ */
void ExitTimeSet(bit save)
{
    setIndex = 0;     //��������������Ϊ0�������˳�����״̬
    if (save)         //�豣��ʱ���ѵ�ǰ����ʱ��д��DS1302
    {
        SetRealTime(&bufTime);
    }
    LcdCloseCursor(); //�رչ����ʾ
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    if  ((keycode>='0') && (keycode<='9'))  //�����в���Ӧ�ַ���
    {
    }
    else if (keycode == 0x26)  //���ϼ���������ǰ����λ��ֵ
    {
        IncSetTime();
    }
    else if (keycode == 0x28)  //���¼����ݼ���ǰ����λ��ֵ
    {
        DecSetTime();
    }
    else if (keycode == 0x25)  //������������л�����λ
    {
        LeftShiftTimeSet();
    }
    else if (keycode == 0x27)  //���Ҽ��������л�����λ
    {
        RightShiftTimeSet();
    }
    else if (keycode == 0x0D)  //�س�������������ģʽ/���õ�ǰ����ֵ
    {
        if (setIndex == 0)  //����������״̬ʱ����������״̬
        {
            EnterTimeSet();
        }
        else                //�Ѵ�������״̬ʱ������ʱ�䲢�˳�����״̬
        {
            ExitTimeSet(1);
        }
    }
    else if (keycode == 0x1B)  //Esc����ȡ����ǰ����
    {
        ExitTimeSet(0);
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
/* T0�жϷ�������ִ�а���ɨ���200ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    KeyScan();   //����ɨ��
    tmr200ms++;
    if (tmr200ms >= 200)  //��ʱ200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
