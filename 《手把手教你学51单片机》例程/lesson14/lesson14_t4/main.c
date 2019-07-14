/*
*******************************************************************************
*                     ���ְ��ֽ���ѧ51��Ƭ��(C���԰�)��
*                    ���� KST-51 ��Ƭ�������� ʾ��Դ����
*
*         (c) ��Ȩ���� 2014 ��ɳ̲������/�廪��ѧ������ ��������Ȩ��
*                 ��ȡ������������ʣ�http://www.kingst.org
*
* �ļ�����main.c
* ��  ������14�� ��ҵ��4 ������ʾ��
* �汾�ţ�v1.0.0
* ��  ע��
* 1����0��9�����ּ��������룬������������λ����PSW_MAX_LEN�궨�����
* 2���û��趨��������EEPROM���棬�����á��ۼӺ͡�У������֤���ݵ���Ч��
* 3�������״�����ʱEEPROM�е�������Ч����ʱ����ʾ���������롱
* 4����������ʱ��Ҫ�û�����������ͬ�����룬�Ա�֤���óɹ������ú������Զ�����
* 5�������������벻��ͬʱ����������������ʧ�ܣ����س�����������
* 6���������óɹ���ÿ�ο�������Ҫ���û��������룬�û���������󣬰��س�����
     ϵͳ�������������EEPROM�б�����������Աȣ���ͬ��ϵͳ�����������������
* 7�������������ϵͳ����ʱ�����س����������¿�ʼ��������
* 8������������ȷϵͳ����ʱ�����س������Խ����޸�����Ĳ���
* 9������������ʱ����ͨ���޸�PSW_MAX_LEN��PSW_SAVE_ADDR�궨���ֵ����������
*******************************************************************************
*/

#include <reg52.h>

#define PSW_MAX_LEN    13    //�ú궨���������󳤶ȣ���󲻳���16��
#define PSW_SAVE_ADDR  0x30  //�ú궨���������EEPROM�еı����ַ

unsigned char status;  //ϵͳ״̬�֣�0-�������룬1-�������룬2-��������ȷ�ϣ�
                       //3-��������ʧ�ܣ�4-�����������ȷ��5-������������
unsigned char PswCnt;  //��ǰ��������λ���ļ�����
unsigned char pdata PswSet[PSW_MAX_LEN];   //���趨����������
unsigned char pdata PswBuf1[PSW_MAX_LEN];  //�������뻺����1
unsigned char pdata PswBuf2[PSW_MAX_LEN];  //�������뻺����2

unsigned char T0RH = 0;  //T0����ֵ�ĸ��ֽ�
unsigned char T0RL = 0;  //T0����ֵ�ĵ��ֽ�

extern void KeyScan();
extern void KeyDriver();
extern void InitLcd1602();
extern void LcdClearScreen();
extern void LcdShowStr(unsigned char x, unsigned char y, unsigned char *str);
extern void E2Read(unsigned char *buf, unsigned char addr, unsigned char len);
extern void E2Write(unsigned char *buf, unsigned char addr, unsigned char len);

void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len);
void ConfigTimer0(unsigned int ms);
bit  ReadPassword();

void main()
{
    EA = 1;           //�����ж�
    ConfigTimer0(1);  //����T0��ʱ1ms
    InitLcd1602();    //��ʼ��Һ��
    if (ReadPassword())  //��ȡ���벢�ж��Ƿ���Ч
    {
        LcdShowStr(0, 0, "Enter Password");  //��Чʱ��ʾ��������
        status = 0;
    }
    else
    {
        LcdShowStr(0, 0, "Set Password");  //��Чʱ��ʾ��������
        status = 1;
    }
    FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
    FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
    PswCnt = 0;
    
    while (1)
    {
        KeyDriver();
    }
}
/* ��һ���ڴ����ݿ�������һ����dest-Ŀ�ĵ�ַ��src-Դ��ַ��len-���ݳ��� */
void CopyMemory(unsigned char *dest, unsigned char *src, unsigned char len)
{
    while (len--)
    {
        *dest++ = *src++;
    }
}
/* ��һ��ָ���ֽ����һ���ڴ棬dest-Ŀ�ĵ�ַ��byt-������ֽڣ�len-���ݳ��� */
void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len)
{
    while (len--)
    {
        *dest++ = byt;
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
/* ��һ�����ݰ��ֽ��ۼӣ����ػ�õ��ۼӺͣ�dat-����ָ�룬len-���ݳ��� */
unsigned char GetChkSum(unsigned char *dat, unsigned char len)
{
    unsigned char sum = 0;
    
    while (len--)
    {
        sum += *dat++;
    }
    return sum;
}
/* ��EEPROM�ж�ȡ������������ݣ�����������С��ۼӺ͡�У�飬
   ����ֵ-У��ͨ������1�����򷵻�0 */
bit ReadPassword()
{
    unsigned char sum;
    unsigned char buf[PSW_MAX_LEN+1];
    
    E2Read(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //��ȡ�������ݺ�У���ֽ�
    sum = GetChkSum(buf, PSW_MAX_LEN); //�����������ݵ��ۼӺ�
    if (buf[PSW_MAX_LEN] == ~sum)      //У��ͨ������������Чʱ����������
    {
        CopyMemory(PswSet, buf, PSW_MAX_LEN);
        return 1;
    }
    else  //����������Чʱ��������Ĭ�ϳ�ȫ0
    {
        FillMemory(PswSet, '\0', PSW_MAX_LEN);
        return 0;
    }
}
/* ����ǰ���������ݱ��浽EEPROM�У�ͬʱ���У���ֽ� */
void SavePassword()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    CopyMemory(buf, PswSet, PSW_MAX_LEN);  //�����������ݵ���������
    buf[PSW_MAX_LEN] = ~GetChkSum(buf, PSW_MAX_LEN);  //��������ۼӺ�У���ֽ�
    E2Write(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //���������ݱ��浽EEPROM��
}
/* ��Һ������ʾ�뵱ǰ��������λ����ͬ��*����ָʾ��ǰ���������λ�� */
void ShowPswCnt()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    FillMemory(buf, '*', PswCnt);
    FillMemory(buf+PswCnt, '\0', sizeof(buf)-PswCnt);
    LcdShowStr(0, 1, buf);
}
/* ���ּ�����������keycode-���ְ�����ASCIIֵ */
void KeyNumAction(unsigned char keycode)
{
    switch (status)
    {
        case 0: //��������״̬ʱ���������ַ���䵽PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 1: //��������״̬ʱ���������ַ���䵽PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 2: //��������ȷ��״̬ʱ���������ַ���䵽PswBuf2
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf2[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        default: //����״̬������Ӧ
            break;
    }
}
/* �س����������� */
void KeyEnterAction()
{
    switch (status)
    {
        case 0: //�������������Ƿ���ȷ
            LcdClearScreen();
            if (CmpMemory(PswSet, PswBuf1, PSW_MAX_LEN))
            {   //��ȷʱ��ʾ��������ʾ
                LcdShowStr(0, 0, "Password Correct");
                LcdShowStr(0, 1, "system unlock");
                status = 4;  //�������״̬
            }
            else
            {   //����ʱ��ʾ����������ʾ
                LcdShowStr(0, 0, "Password Wrong");
                LcdShowStr(0, 1, "system locked");
                status = 5;  //��������״̬
            }
            break;
        case 1: //�����������õ�ȷ�ϲ��裬��������һ������
            LcdClearScreen();
            LcdShowStr(0, 0, "Confirm Password");
            PswCnt = 0;
            status = 2;
            break;
        case 2: //�����������������Ƿ�һ��
            LcdClearScreen();
            if (CmpMemory(PswBuf1, PswBuf2, PSW_MAX_LEN))
            {   //һ��ʱ�������õ�����
                LcdShowStr(0, 0, "Enter Password");
                CopyMemory(PswSet, PswBuf1, PSW_MAX_LEN);
                SavePassword();
                FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
                PswCnt = 0;
                status = 0;  //������������״̬
            }
            else
            {   //��һ��ʱ��ʾ��������ʧ��
                LcdShowStr(0, 0, "Confirm Password");
                LcdShowStr(0, 1, "Failed");
                status = 3;  //������������ʧ��״̬
            }
            break;
        case 3: //������ʾ�������룬������������״̬
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        case 4: //�������ü��޸�����
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        case 5: //������������
            LcdClearScreen();
            LcdShowStr(0, 0, "Enter Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 0;
            break;
        default: //������Ӧ
            break;
    }
}
/* Esc���������� */
void KeyEscAction()
{
    switch (status)
    {
        case 0: //��������״̬ʱ����յ�ǰ���������¿�ʼ
            LcdClearScreen();
            LcdShowStr(0, 0, "Enter Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            break;
        case 1: //��������״̬ʱ����յ�ǰ���������¿�ʼ
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            break;
        case 2: //��������ȷ��״̬ʱ��������벢������������״̬
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        default: //����״̬������Ӧ
            break;
    }
}
/* �����������������ݼ���ִ����Ӧ�Ĳ�����keycode-�������� */
void KeyAction(unsigned char keycode)
{
    if  ((keycode>='0') && (keycode<='9'))  //�����ַ�
    {
        KeyNumAction(keycode);
    }
    else if (keycode == 0x0D)  //�س���
    {
        KeyEnterAction();
    }
    else if (keycode == 0x1B)  //Esc��
    {
        KeyEscAction();
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
