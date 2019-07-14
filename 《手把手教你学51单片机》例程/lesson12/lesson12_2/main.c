/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������12�� ָ�������ָ���������ϵ��ʾ��
* �汾�ţ�v1.0.0
* ��  ע���������12��12.2��
*******************************************************************************
*/

#include <reg52.h>

bit cmdArrived = 0;   //������־�������յ���λ���·�������
unsigned char cmdIndex = 0; //����������������λ��Լ���õ�������
unsigned char cntTxd = 0;   //���ڷ��ͼ�����
unsigned char *ptrTxd;      //���ڷ���ָ��

unsigned char array1[1] = {1};
unsigned char array2[2] = {1,2};
unsigned char array3[4] = {1,2,3,4};
unsigned char array4[8] = {1,2,3,4,5,6,7,8};

void ConfigUART(unsigned int baud);

void main()
{
    EA = 1;  //�����ж�
    ConfigUART(9600);  //���ò�����Ϊ9600
    
    while (1)
    {
        if (cmdArrived)
        {
            cmdArrived = 0;
            switch (cmdIndex)
            {
                case 1:
                    ptrTxd = array1;         //����1���׵�ַ��ֵ������ָ��
                    cntTxd = sizeof(array1); //����1�ĳ��ȸ�ֵ�����ͼ�����
                    TI = 1;   //�ֶ���ʽ���������жϣ��������ݷ���
                    break;
                case 2:
                    ptrTxd = array2;
                    cntTxd = sizeof(array2);
                    TI = 1;
                    break;
                case 3:
                    ptrTxd = array3;
                    cntTxd = sizeof(array3);
                    TI = 1;
                    break;
                case 4:
                    ptrTxd = array4;
                    cntTxd = sizeof(array4);
                    TI = 1;
                    break;
                default:
                    break;
            }
        }
    }
}
/* �������ú�����baud-ͨ�Ų����� */
void ConfigUART(unsigned int baud)
{
    SCON  = 0x50;  //���ô���Ϊģʽ1
    TMOD &= 0x0F;  //����T1�Ŀ���λ
    TMOD |= 0x20;  //����T1Ϊģʽ2
    TH1 = 256 - (11059200/12/32)/baud;  //����T1����ֵ
    TL1 = TH1;     //��ֵ��������ֵ
    ET1 = 0;       //��ֹT1�ж�
    ES  = 1;       //ʹ�ܴ����ж�
    TR1 = 1;       //����T1
}
/* UART�жϷ����� */
void InterruptUART() interrupt 4
{
    if (RI)  //���յ��ֽ�
    {
        RI = 0;  //��������жϱ�־λ
        cmdIndex = SBUF;  //���յ������ݱ��浽����������
        cmdArrived = 1;   //����������־
    }
    if (TI)  //�ֽڷ������
    {
        TI = 0;  //���㷢���жϱ�־λ
        if (cntTxd > 0)  //�д���������ʱ���������ͺ����ֽ�
        {
            SBUF = *ptrTxd;  //����ָ��ָ�������
            cntTxd--;        //���ͼ������ݼ�
            ptrTxd++;        //����ָ�����
        }
    }
}
