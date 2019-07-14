/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����eeprom.c
* ��  ����EEPROMоƬ24C02��д����ģ��
* �汾�ţ�v1.0.0
* ��  ע��
*******************************************************************************
*/

#include <reg52.h>

extern void I2CStart();
extern void I2CStop();
extern unsigned char I2CReadACK();
extern unsigned char I2CReadNAK();
extern bit I2CWrite(unsigned char dat);

/* E2��ȡ������buf-���ݽ���ָ�룬addr-E2�е���ʼ��ַ��len-��ȡ���� */
void E2Read(unsigned char *buf, unsigned char addr, unsigned char len)
{
    do {                       //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д����
        I2CStart();
        if (I2CWrite(0x50<<1)) //Ӧ��������ѭ������Ӧ���������һ�β�ѯ
        {
            break;
        }
        I2CStop();
    } while(1);
    I2CWrite(addr);            //д����ʼ��ַ
    I2CStart();                //�����ظ������ź�
    I2CWrite((0x50<<1)|0x01);  //Ѱַ����������Ϊ������
    while (len > 1)            //������ȡlen-1���ֽ�
    {
        *buf++ = I2CReadACK(); //����ֽ�֮ǰΪ��ȡ����+Ӧ��
        len--;
    }
    *buf = I2CReadNAK();       //���һ���ֽ�Ϊ��ȡ����+��Ӧ��
    I2CStop();
}
/* E2д�뺯����buf-Դ����ָ�룬addr-E2�е���ʼ��ַ��len-д�볤�� */
void E2Write(unsigned char *buf, unsigned char addr, unsigned char len)
{
    while (len > 0)
    {
        //�ȴ��ϴ�д��������
        do {                       //��Ѱַ������ѯ��ǰ�Ƿ�ɽ��ж�д����
            I2CStart();
            if (I2CWrite(0x50<<1)) //Ӧ��������ѭ������Ӧ���������һ�β�ѯ
            {
                break;
            }
            I2CStop();
        } while(1);
        //��ҳдģʽ����д���ֽ�
        I2CWrite(addr);           //д����ʼ��ַ
        while (len > 0)
        {
            I2CWrite(*buf++);     //д��һ���ֽ�����
            len--;                //��д�볤�ȼ����ݼ�
            addr++;               //E2��ַ����
            if ((addr&0x07) == 0) //����ַ�Ƿ񵽴�ҳ�߽磬24C02ÿҳ8�ֽڣ�
            {                     //���Լ���3λ�Ƿ�Ϊ�㼴��
                break;            //����ҳ�߽�ʱ������ѭ������������д����
            }
        }
        I2CStop();
    }
}
