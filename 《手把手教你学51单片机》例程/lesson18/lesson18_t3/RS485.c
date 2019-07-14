/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����RS485.c
* ��  ����RS485ͨ������ģ��
* �汾�ţ�v1.0.0
* ��  ע���ڱ�׼UART�����Ļ��������RS485�ķ�����ƹ���
*         ���Ӿ�������������ı�RS485_DIR��ָ���IO��
*******************************************************************************
*/

#include <reg52.h>
#include <intrins.h>

sbit RS485_DIR = P2^0;  //RS485����ѡ������

bit flagFrame = 0;  //֡������ɱ�־�������յ�һ֡������
bit flagTxd = 0;    //���ֽڷ�����ɱ�־���������TXD�жϱ�־λ
unsigned char cntRxd = 0;   //�����ֽڼ�����
unsigned char pdata bufRxd[64];  //�����ֽڻ�����

extern void UartAction(unsigned char *buf, unsigned char len);

/* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud)
{
    RS485_DIR = 0; //RS485����Ϊ���շ���
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
}
/* �����ʱ��������ʱʱ��(t*10)us */
void DelayX10us(unsigned char t)
{
    do {
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
        _nop_();
    } while (--t);
}
/* ��������д�룬�����ڷ��ͺ�����buf-���������ݵ�ָ�룬len-ָ���ķ��ͳ��� */
void UartWrite(unsigned char *buf, unsigned char len)
{
    RS485_DIR = 1;  //RS485����Ϊ����
    while (len--)   //ѭ�����������ֽ�
    {
        flagTxd = 0;      //���㷢�ͱ�־
        SBUF = *buf++;    //����һ���ֽ�����
        while (!flagTxd); //�ȴ����ֽڷ������
    }
    DelayX10us(5);  //�ȴ�����ֹͣλ��ɣ���ʱʱ���ɲ����ʾ���
    RS485_DIR = 0;  //RS485����Ϊ����
}
/* �������ݶ�ȡ������buf-����ָ�룬len-ָ���Ķ�ȡ���ȣ�����ֵ-ʵ�ʶ����ĳ��� */
unsigned char UartRead(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    
    if (len > cntRxd)  //ָ����ȡ���ȴ���ʵ�ʽ��յ������ݳ���ʱ��
    {                  //��ȡ��������Ϊʵ�ʽ��յ������ݳ���
        len = cntRxd;
    }
    for (i=0; i<len; i++)  //�������յ������ݵ�����ָ����
    {
        *buf++ = bufRxd[i];
    }
    cntRxd = 0;  //���ռ���������
    
    return len;  //����ʵ�ʶ�ȡ����
}
/* ���ڽ��ռ�أ��ɿ���ʱ���ж�֡���������ڶ�ʱ�ж��е��ã�ms-��ʱ��� */
void UartRxMonitor(unsigned char ms)
{
    static unsigned char cntbkp = 0;
    static unsigned char idletmr = 0;

    if (cntRxd > 0)  //���ռ�����������ʱ��������߿���ʱ��
    {
        if (cntbkp != cntRxd)  //���ռ������ı䣬���ս��յ�����ʱ��������м�ʱ
        {
            cntbkp = cntRxd;
            idletmr = 0;
        }
        else                   //���ռ�����δ�ı䣬�����߿���ʱ���ۻ�����ʱ��
        {
            if (idletmr < 30)  //���м�ʱС��30msʱ�������ۼ�
            {
                idletmr += ms;
                if (idletmr >= 30)  //����ʱ��ﵽ30msʱ�����ж�Ϊһ֡�������
                {
                    flagFrame = 1;  //����֡������ɱ�־
                }
            }
        }
    }
    else
    {
        cntbkp = 0;
    }
}
/* ���������������������֡�Ľ��գ����ȹ��ܺ�����������ѭ���е��� */
void UartDriver()
{
    unsigned char len;
    unsigned char pdata buf[40];

    if (flagFrame) //�������ʱ����ȡ���������
    {
        flagFrame = 0;
        len = UartRead(buf, sizeof(buf)-2); //�����յ��������ȡ����������
        UartAction(buf, len);  //��������֡�����ö���ִ�к���
    }
}
/* �����жϷ����� */
void InterruptUART() interrupt 4
{
    if (RI)  //���յ����ֽ�
    {
        RI = 0;  //��������жϱ�־λ
        if (cntRxd < sizeof(bufRxd)) //���ջ�������δ����ʱ��
        {                            //��������ֽڣ�������������
            bufRxd[cntRxd++] = SBUF;
        }
    }
    if (TI)  //�ֽڷ������
    {
        TI = 0;   //���㷢���жϱ�־λ
        flagTxd = 1;  //�����ֽڷ�����ɱ�־
    }
}
