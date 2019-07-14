/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������18�� ����RS485��Modbusͨ��ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������18��18.3��
*         ֧��ModbusЭ��ļĴ�����ȡ�͵��Ĵ���д�����
*******************************************************************************
*/

#include <reg52.h>

sbit BUZZ = P1^6;

bit flagBuzzOn = 0;   //������������־
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�
unsigned char regGroup[5];  //Modbus�Ĵ����飬��ַΪ0x00��0x04

void ConfigTimer0(unsigned int ms);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern unsigned int GetCRC16(unsigned char *ptr,  unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

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
/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
   buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char cnt;
    unsigned char str[4];
    unsigned int  crc;
    unsigned char crch, crcl;
    
    if (buf[0] != 0x01) //�����еı�����ַ�趨Ϊ0x01��
    {                   //������֡�еĵ�ַ�ֽ��뱾����ַ������
        return;         //��ֱ���˳�����������֡���ݲ����κδ���
    }
    //��ַ���ʱ���ٶԱ�֡���ݽ���У��
    crc = GetCRC16(buf, len-2);  //����CRCУ��ֵ
    crch = crc >> 8;
    crcl = crc & 0xFF;
    if ((buf[len-2]!=crch) || (buf[len-1]!=crcl))
    {
        return;   //��CRCУ�鲻��ʱֱ���˳�
    }
    //��ַ��У���־�����󣬽��������룬ִ����ز���
    switch (buf[1])
    {
        case 0x03:  //��ȡһ���������ļĴ���
            if ((buf[2]==0x00) && (buf[3]<=0x05)) //ֻ֧��0x0000��0x0005
            {
                if (buf[3] <= 0x04)
                {
                    i = buf[3];      //��ȡ�Ĵ�����ַ
                    cnt = buf[5];    //��ȡ����ȡ�ļĴ�������
                    buf[2] = cnt*2;  //��ȡ���ݵ��ֽ�����Ϊ�Ĵ�����*2
                    len = 3;         //֡ǰ�����е�ַ�������롢�ֽ�����3���ֽ�
                    while (cnt--)
                    {
                        buf[len++] = 0x00;          //�Ĵ������ֽڲ�0
                        buf[len++] = regGroup[i++]; //�Ĵ������ֽ�
                    }
                }
                else  //��ַ0x05Ϊ������״̬
                {
                    buf[2] = 2;  //��ȡ���ݵ��ֽ���
                    buf[3] = 0x00;
                    buf[4] = flagBuzzOn;
                    len = 5;
                }
                break;
            }
            else  //�Ĵ�����ַ����֧��ʱ�����ش�����
            {
                buf[1] = 0x83;  //���������λ��1
                buf[2] = 0x02;  //�����쳣��Ϊ02-��Ч��ַ
                len = 3;
                break;
            }
            
        case 0x06:  //д�뵥���Ĵ���
            if ((buf[2]==0x00) && (buf[3]<=0x05)) //ֻ֧��0x0000��0x0005
            {
                if (buf[3] <= 0x04)
                {
                    i = buf[3];             //��ȡ�Ĵ�����ַ
                    regGroup[i] = buf[5];   //����Ĵ�������
                    cnt = regGroup[i] >> 4; //��ʾ��Һ����
                    if (cnt >= 0xA)
                        str[0] = cnt - 0xA + 'A';
                    else
                        str[0] = cnt + '0';
                    cnt = regGroup[i] & 0x0F;
                    if (cnt >= 0xA)
                        str[1] = cnt - 0xA + 'A';
                    else
                        str[1] = cnt + '0';
                    str[2] = '\0';
                    LcdShowStr(i*3, 0, str);
                }
                else  //��ַ0x05Ϊ������״̬
                {
                    flagBuzzOn = (bit)buf[5]; //�Ĵ���ֵתΪ�������Ŀ���
                }
                len -= 2; //����-2�����¼���CRC������ԭ֡
                break;
            }
            else  //�Ĵ�����ַ����֧��ʱ�����ش�����
            {
                buf[1] = 0x86;  //���������λ��1
                buf[2] = 0x02;  //�����쳣��Ϊ02-��Ч��ַ
                len = 3;
                break;
            }
            
        default:  //������֧�ֵĹ�����
            buf[1] |= 0x80;  //���������λ��1
            buf[2] = 0x01;   //�����쳣��Ϊ01-��Ч����
            len = 3;
            break;
    }
    crc = GetCRC16(buf, len); //���㷵��֡��CRCУ��ֵ
    buf[len++] = crc >> 8;    //CRC���ֽ�
    buf[len++] = crc & 0xFF;  //CRC���ֽ�
    UartWrite(buf, len);      //���ͷ���֡
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
