/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������13�� ����֡ģʽ��ʵ�ô��ڳ���ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������13��13.5��
*         ��������֡ģʽ��ͨ��UART���ڿ��Ʒ��������غ�Һ����ʾ�ַ�
*******************************************************************************
*/

#include <reg52.h>

sbit BUZZ = P1^6;  //��������������

bit flagBuzzOn = 0;   //������������־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void LcdAreaClear(unsigned char x, unsigned char y, unsigned char len);

void main()
{
    EA = 1;            //�����ж�
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    InitLcd1602();     //��ʼ��Һ��
    
    while (1)
    {
        UartDriver();  //���ô�������
    }
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
    unsigned char code cmd0[] = "buzz on";   //������������
    unsigned char code cmd1[] = "buzz off";  //�ط���������
    unsigned char code cmd2[] = "showstr ";  //�ַ�����ʾ����
    unsigned char code cmdLen[] = {          //����Ȼ��ܱ�
        sizeof(cmd0)-1, sizeof(cmd1)-1, sizeof(cmd2)-1,
    };
    unsigned char code *cmdPtr[] = {         //����ָ����ܱ�
        &cmd0[0],  &cmd1[0],  &cmd2[0],
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
    switch (i)  //ѭ���˳�ʱi��ֵ���ǵ�ǰ���������ֵ
    {
        case 0:
            flagBuzzOn = 1; //����������
            break;
        case 1:
            flagBuzzOn = 0; //�رշ�����
            break;
        case 2:
            buf[len] = '\0';  //Ϊ���յ����ַ�����ӽ�����
            LcdShowStr(0, 0, buf+cmdLen[2]);  //��ʾ�������ַ���
            i = len - cmdLen[2];              //������Ч�ַ�����
            if (i < 16)  //��Ч�ַ�����16ʱ�����Һ���ϵĺ����ַ�λ
            {
                LcdAreaClear(i, 0, 16-i);
            }
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
    if (flagBuzzOn)  //ִ�з��������л�ر�
        BUZZ = ~BUZZ;
    else
        BUZZ = 1;
    UartRxMonitor(1);  //���ڽ��ռ��
}
