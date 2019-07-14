/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� ���׵��ӹ����ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������14��14.4��
*         ��ȡEEPROM�е�������ʾ��Һ��������ͨ��UART�޸�EEPROM�е�����
*******************************************************************************
*/

#include <reg52.h>

unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void InitShowStr();
void ConfigTimer0(unsigned int ms);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);

void main()
{
    EA = 1;            //�����ж�
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    InitLcd1602();     //��ʼ��Һ��
    InitShowStr();     //��ʼ��ʾ����
    
    while (1)
    {
        UartDriver();  //���ô�������
    }
}
/* ����Һ������ʼ��ʾ���� */
void InitShowStr()
{
    unsigned char str[17];
    
    str[16] = '\0';         //���������ַ�����������ȷ���ַ������Խ���
    E2Read(str, 0x20, 16);  //��ȡ��һ���ַ�������E2��ʼ��ַΪ0x20
    LcdShowStr(0, 0, str);  //��ʾ��Һ����
    E2Read(str, 0x40, 16);  //��ȡ�ڶ����ַ�������E2��ʼ��ַΪ0x40
    LcdShowStr(0, 1, str);  //��ʾ��Һ����
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
/* ��һ�ַ��������16�ֽڵĹ̶������ַ��������㲿�ֲ��ո�
   out-�������ַ������ָ�룬in-�������ַ���ָ�� */
void TrimString16(unsigned char *out, unsigned char *in)
{
    unsigned char i = 0;
    
    while (*in != '\0')  //�����ַ���ֱ�������ַ�������
    {
        *out++ = *in++;
        i++;
        if (i >= 16)    //�����������Ѵﵽ16�ֽ�ʱ��ǿ������ѭ��
        {
            break;
        }
    }
    for ( ; i<16; i++)  //�粻��16���ֽ����ÿո���
    {
        *out++ = ' ';
    }
    *out = '\0';        //�����ӽ�����
}
/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
   buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char str[17];
    unsigned char code cmd0[] = "showstr1 ";  //��һ���ַ���ʾ����
    unsigned char code cmd1[] = "showstr2 ";  //�ڶ����ַ���ʾ����
    unsigned char code cmdLen[] = {           //����Ȼ��ܱ�
        sizeof(cmd0)-1, sizeof(cmd1)-1,
    };
    unsigned char code *cmdPtr[] = {          //����ָ����ܱ�
        &cmd0[0],  &cmd1[0],
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
    switch (i) //���ݱȽϽ��ִ����Ӧ����
    {
        case 0:
            buf[len] = '\0';                   //Ϊ���յ����ַ�����ӽ�����
            TrimString16(str, buf+cmdLen[0]);  //�����16�ֽڹ̶������ַ���
            LcdShowStr(0, 0, str);             //��ʾ�ַ���1
            E2Write(str, 0x20, sizeof(str));   //�����ַ���1����ʼ��ַΪ0x20
            break;
        case 1:
            buf[len] = '\0';                   //Ϊ���յ����ַ�����ӽ�����
            TrimString16(str, buf+cmdLen[1]);  //�����16�ֽڹ̶������ַ���
            LcdShowStr(0, 1, str);             //��ʾ�ַ���1
            E2Write(str, 0x40, sizeof(str));   //�����ַ���2����ʼ��ַΪ0x40
            break;
        default:   //δ�ҵ��������ʱ�����ϻ����͡������������ʾ
            UartWrite("bad command.\r\n", sizeof("bad command.\r\n")-1);
            return;
    }
    buf[len++] = '\r';  //��Ч���ִ�к���ԭ����֮֡�����
    buf[len++] = '\n';  //�س����з��󷵻ظ���λ������ʾ��ִ��
    UartWrite(buf, len);
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
/* T0�жϷ�������ִ�д��ڽ��ռ�غͷ��������� */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    UartRxMonitor(1);  //���ڽ��ռ��
}
