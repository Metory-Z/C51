/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������18�� ��ҵ��3 ModbusЭ��ʵ��Уʱ
* �汾�ţ�v1.0.0
* ��  ע��
* 1����lesson15_3�Ļ�����ȥ������Уʱ�����lesson18_2�е�ModbusЭ��֧��
* 2������Modbus���Ծ����д�Ĵ������ܣ����޸�����ʱ���ÿһ���ֽ�
* 3���Ĵ�����ַ0x0000��0x0006�ֱ��Ӧ����/��/��/ʱ/��/��/���ڡ�
* 4��RS485��������ź���ԭ����P1.7��ΪP2.0������ʹ����DS1302��δʹ�ð���
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
bit reqRefresh = 0;  //ʱ��ˢ������
struct sTime bufTime;  //����ʱ�仺����
unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

void ConfigTimer0(unsigned int ms);
void RefreshTimeShow();
extern void InitDS1302();
extern void GetRealTime(struct sTime *time);
extern void SetRealTime(struct sTime *time);
extern void UartDriver();
extern void ConfigUART(unsigned int baud);
extern void UartRxMonitor(unsigned char ms);
extern void UartWrite(unsigned char *buf, unsigned char len);
extern unsigned int GetCRC16(unsigned char *ptr,  unsigned char len);
extern void InitLcd1602();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);

void main()
{
    unsigned char psec=0xAA;  //�뱸�ݣ���ֵAAȷ���״ζ�ȡʱ����ˢ����ʾ

    EA = 1;            //�����ж�
    ConfigTimer0(1);   //����T0��ʱ1ms
    ConfigUART(9600);  //���ò�����Ϊ9600
    InitDS1302();      //��ʼ��ʵʱʱ��
    InitLcd1602();     //��ʼ��Һ��
    
    //��ʼ����Ļ�Ϲ̶����������
    LcdShowStr(3, 0, "20  -  -  ");
    LcdShowStr(4, 1, "  :  :  ");
    
    while (1)
    {
        UartDriver();  //���ô�������
        if (flag200ms)
        {
            flag200ms = 0;
            GetRealTime(&bufTime);    //��ȡ��ǰʱ��
            if (reqRefresh || (psec!=bufTime.sec))
            {   //��⵽ʱ��ˢ�������ʱ���б仯ʱˢ����ʾ
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
/* ���ڶ������������ݽ��յ�������ִ֡����Ӧ�Ķ���
   buf-���յ�������ָ֡�룬len-����֡���� */
void UartAction(unsigned char *buf, unsigned char len)
{
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
        case 0x06:  //д�뵥���Ĵ���
            if ((buf[2]==0x00) && (buf[3]<=0x06)) //��ַ0x0000��0x0006�ֱ��Ӧ
            {                                     // ����/��/��/ʱ/��/��/���ڡ�
                GetRealTime(&bufTime);  //��ȡ��ǰʱ��
                switch (buf[3])  //�ɼĴ�����ַ����Ҫ�޸ĵ�ʱ��λ
                {
                    case 0: bufTime.year = 0x2000 + buf[5]; break;
                    case 1: bufTime.mon  = buf[5]; break;
                    case 2: bufTime.day  = buf[5]; break;
                    case 3: bufTime.hour = buf[5]; break;
                    case 4: bufTime.min  = buf[5]; break;
                    case 5: bufTime.sec  = buf[5]; break;
                    case 6: bufTime.week = buf[5]; break;
                    default: break;
                }
                SetRealTime(&bufTime);  //д�����޸ĺ��ʱ��
                reqRefresh = 1;  //������ʾˢ������
                len -= 2;  //����-2�����¼���CRC������ԭ֡
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
/* T0�жϷ�������ִ�а���ɨ���200ms��ʱ */
void InterruptTimer0() interrupt 1
{
    static unsigned char tmr200ms = 0;
    
    TH0 = T0RH;  //���¼�������ֵ
    TL0 = T0RL;
    UartRxMonitor(1);  //���ڽ��ռ��
    tmr200ms++;
    if (tmr200ms >= 200)  //��ʱ200ms
    {
        tmr200ms = 0;
        flag200ms = 1;
    }
}
