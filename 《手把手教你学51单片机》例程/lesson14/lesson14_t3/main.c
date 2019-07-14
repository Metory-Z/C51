/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� ��ҵ��3 ���޸�����ʱ��Ľ�ͨ��
* �汾�ţ�v1.0.0
* ��  ע������lesson10_4���̵Ļ�������ӹ���
*         ��EEPROM��ַ0x10��ʼ���汣��3�ֵƵ�����ʱ�䣬��ȡʱ�������Ч��
*         ͨ��UART����������Էֱ��޸�3��ʱ�䣬��������ʱͬ���������Ч��
*         ʱ���趨����Ϊ��setr xx������setg xx������sety xx�� (xxΪ��λ��Ч����)
*******************************************************************************
*/

#include <reg52.h>

sbit  ADDR3 = P1^3;
sbit  ENLED = P1^4;

unsigned char code LedChar[] = {  //�������ʾ�ַ�ת����
    0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8,
    0x80, 0x90, 0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E
};
unsigned char LedBuff[7] = {  //�����+����LED��ʾ������
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
};
bit flag1s = 1;          //1�붨ʱ��־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char RedTime, GrnTime, YlwTime; //��ơ��̵ơ��ƵƵ��趨ʱ��

void ConfigTimer0(unsigned int ms);
void GetLightTime();
void TrafficLight();
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main()
{
    EA = 1;            //�����ж�
    ENLED = 0;         //ʹ������ܺͶ���LED
    ADDR3 = 1;
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    GetLightTime();    //��ȡ��ͨ���趨ʱ��
    
    while (1)
    {
        UartDriver();  //���ô�������
        if (flag1s)    //ÿ��ִ��һ�ν�ͨ��ˢ��
        {
            flag1s = 0;
            TrafficLight();
        }
    }
}
/* ��E2PROM�л�ȡ�趨�ĵ���ʱ�� */
void GetLightTime()
{
    unsigned char buf[4];
    
    E2Read(buf, 0x10, 3);  //�ӵ�ַ0x10��ʼ��3���ֽڱ������ֵƵ�ʱ��
    if ((buf[0]<1) || (buf[0]>99))  //�����ʱ���Ƿ���Ч����Ч��Χ1��99
        RedTime = 30;       //��Чʱ��ȡĬ��ֵ30
    else
        RedTime = buf[0];   //��Чʱ��ȡ���趨ֵ
    
    if ((buf[1]<1) || (buf[1]>99))  //����̵�ʱ���Ƿ���Ч����Ч��Χ1��99
        GrnTime = 40;       //��Чʱ��ȡĬ��ֵ40
    else
        GrnTime = buf[1];   //��Чʱ��ȡ���趨ֵ
    
    if ((buf[2]<1) || (buf[2]>10))  //���Ƶ�ʱ���Ƿ���Ч����Ч��Χ1��10
        YlwTime = 3;        //��Чʱ��ȡĬ��ֵ3
    else
        YlwTime = buf[2];   //��Чʱ��ȡ���趨ֵ
}
/* ���õ���ʱ�䣬����ʱ�䵽E2�����µ�ǰֵ
   str-�����趨ʱ����ֵ���ַ���ָ�룬light-����ɫ����
   ����ֵ-ʱ������ȷ�趨����1�����򷵻�0 */
unsigned char SetLightTime(unsigned char *str, unsigned char light)
{
    unsigned char i;
    unsigned char tmp = 0;
    unsigned char result = 0;
    
    for (i=0; i<2; i++)  //���ȡ��λ�ַ���ת��Ϊ���ֽ���ֵ
    {
        if ((*str >= '0') && (*str <= '9'))  //0~9Ϊ��Ч�ַ�
        {
            tmp *= 10;         //��ǰ����������10��������ʮ��������1λ
            tmp += (*str-'0'); //��ǰ�ַ���ֵ��䵽��λ��
            str++;             //�ַ�ָ�������׼����һ�μ��
        }
        else  //������Ч�ַ����˳�
        {
            break;
        }
    }
    switch (light)  //����趨ֵ�Ƿ���Ч
    {
        case 0:  //�����ʱ��
            if ((tmp>=1) && (tmp<=99))
            {
                RedTime = tmp;
                result = 1;
            }
            break;
        case 1:  //����̵�ʱ��
            if ((tmp>=1) && (tmp<=99))
            {
                GrnTime = tmp;
                result = 1;
            }
            break;
        case 2:  //���Ƶ�ʱ��
            if ((tmp>=1) && (tmp<=10))
            {
                YlwTime = tmp;
                result = 1;
            }
            break;
        default:
            break;
    }
    if (result != 0)  //����趨ֵ��Ч��ͬʱ���浽E2��
    {
        E2Write(&tmp, 0x10+light, 1);
    }
    
    return result;
}

/* ��ͨ����ʾˢ�º��� */
void TrafficLight()
{
    static unsigned char color = 2;  //��ɫ������0-��ɫ/1-��ɫ/2-��ɫ
    static unsigned char timer = 0;  //����ʱ��ʱ��
    
    if (timer == 0) //����ʱ��0ʱ���л���ͨ��
    {
        switch (color)  //LED8/9�����̵ƣ�LED5/6����Ƶƣ�LED2/3������
        {
            case 0:     //�л�����ɫ
                color = 1;
                timer = YlwTime - 1;
                LedBuff[6] = 0xE7;
                break;
            case 1:     //�л�����ɫ
                color = 2;
                timer = RedTime - 1;
                LedBuff[6] = 0xFC;
                break;
            case 2:     //�л�����ɫ
                color = 0;
                timer = GrnTime - 1;
                LedBuff[6] = 0x3F;
                break;
            default:
                break;
        }
    }
    else  //����ʱδ��0ʱ���ݼ������ֵ
    {
        timer--;
    }
    LedBuff[0] = LedChar[timer%10];  //����ʱ��ֵ��λ��ʾ
    LedBuff[1] = LedChar[timer/10];  //����ʱ��ֵʮλ��ʾ
}
/* �ڴ�ȽϺ������Ƚ�����ָ����ָ����ڴ������Ƿ���ͬ��
   ptr1-���Ƚ�ָ��1��ptr2-���Ƚ�ָ��2��len-���Ƚϳ���
   ����ֵ-�����ڴ�������ȫ��ͬʱ����1����ͬ����0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //�������������ʱ���̷���0
        {
            return 0;
        }
    }
    return 1;  //�Ƚ���ȫ���������ݶ�����򷵻�1
}
/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
   buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char code cmd0[] = "setr ";  //���ú��ʱ��
    unsigned char code cmd1[] = "setg ";  //�����̵�ʱ��
    unsigned char code cmd2[] = "sety ";  //���ûƵ�ʱ��
    unsigned char code cmdLen[] = {       //����Ȼ��ܱ�
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {      //����ָ����ܱ�
        &cmd0[0],  &cmd1[0], &cmd2[0],
    };

    for (i=0; i<sizeof(cmdLen); i++)  //���������б�������ͬ����
    {
        if (len >= cmdLen[i])  //���Ƚ��յ������ݳ���Ҫ��С�������
        {
            if (CmpMemory(buf, cmdPtr[i], cmdLen[i]))  //�Ƚ���ͬʱ�˳�ѭ��
            {
                break;
            }
        }
    }
    if (i < sizeof(cmdLen))
    {
        buf[len] = '\0';     //Ϊ���յ����ַ�����ӽ�����
        if (SetLightTime(buf+cmdLen[i], i) != 0)
        {
            buf[len++] = '\r';  //��Ч���ִ�к���ԭ����֮֡�����
            buf[len++] = '\n';  //�س����з��󷵻ظ���λ������ʾ��ִ��
            UartWrite(buf, len);
        }
        else  //δ����ȷ����ʱ��ʱ���ء������������Ϣ
        {
            UartWrite("bad parameter.\r\n", sizeof("bad parameter.\r\n")-1);
        }
    }
    else  //������Чʱ���ء����������Ϣ
    {
        UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
    }
}
/* ���ò�����T0��ms-T0��ʱʱ�� */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //��ʱ����
    
    tmp = 11059200 / 12;      //��ʱ������Ƶ��
    tmp = (tmp * ms) / 1000;  //��������ļ���ֵ
    tmp = 65536 - tmp;        //���㶨ʱ������ֵ
    tmp = tmp + 33;           //�����ж���Ӧ��ʱ��ɵ����
    T0RH = (unsigned char)(tmp>>8);  //��ʱ������ֵ���Ϊ�ߵ��ֽ�
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //����T0�Ŀ���λ
    TMOD |= 0x01;   //����T0Ϊģʽ1
    TH0 = T0RH;     //����T0����ֵ
    TL0 = T0RL;
    ET0 = 1;        //ʹ��T0�ж�
    TR0 = 1;        //����T0
}
/* LED��̬ɨ��ˢ�º��������ڶ�ʱ�ж��е��� */
void LedScan()
{
    static unsigned char i = 0;  //��̬ɨ������
    
    P0 = 0xFF;             //�ر����ж�ѡλ����ʾ����
    P1 = (P1 & 0xF8) | i;  //λѡ����ֵ��ֵ��P1�ڵ�3λ
    P0 = LedBuff[i];       //������������λ�õ������͵�P0��
    if (i < 6)             //��������ѭ������������������
        i++;
    else
        i = 0;
}
/* T0�жϷ�������ִ�д��ڽ��ռ�غͷ��������� */
void InterruptTimer0() interrupt 1
{
    static unsigned int tmr1s = 0;  //1�붨ʱ��
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    LedScan();   //LEDɨ����ʾ
    UartRxMonitor(1);  //���ڽ��ռ��
    tmr1s++;     //1�붨ʱ�Ĵ���
    if (tmr1s >= 1000)
    {
        tmr1s = 0;
        flag1s = 1;  //�����붨ʱ��־
    }
}
