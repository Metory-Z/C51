/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第14章 作业题4 密码锁示例
* 版本号：v1.0.0
* 备  注：
* 1、用0～9的数字键输入密码，允许的密码最大位数由PSW_MAX_LEN宏定义给出
* 2、用户设定的密码由EEPROM保存，并且用“累加和”校验来保证数据的有效性
* 3、程序首次运行时EEPROM中的数据无效，此时会提示“设置密码”
* 4、设置密码时需要用户输入两次相同的密码，以保证设置成功，设置后密码自动保存
* 5、两次输入密码不相同时，将导致密码设置失败，按回车键重新设置
* 6、密码设置成功后，每次开机都会要求用户输入密码，用户键入密码后，按回车键，
     系统将键入的密码与EEPROM中保存的密码做对比，相同则系统解锁，否则继续锁定
* 7、密码输入错误系统锁定时，按回车键可以重新开始输入密码
* 8、密码输入正确系统解锁时，按回车键可以进入修改密码的操作
* 9、当忘记密码时，可通过修改PSW_MAX_LEN或PSW_SAVE_ADDR宏定义的值来重置密码
*******************************************************************************
*/

#include <reg52.h>

#define PSW_MAX_LEN    13    //用宏定义出密码最大长度（最大不超过16）
#define PSW_SAVE_ADDR  0x30  //用宏定义出密码在EEPROM中的保存地址

unsigned char status;  //系统状态字：0-输入密码，1-设置密码，2-设置密码确认，
                       //3-密码设置失败，4-输入的密码正确，5-输入的密码错误
unsigned char PswCnt;  //当前输入密码位数的计数器
unsigned char pdata PswSet[PSW_MAX_LEN];   //已设定的密码数据
unsigned char pdata PswBuf1[PSW_MAX_LEN];  //密码输入缓冲区1
unsigned char pdata PswBuf2[PSW_MAX_LEN];  //密码输入缓冲区2

unsigned char T0RH = 0;  //T0重载值的高字节
unsigned char T0RL = 0;  //T0重载值的低字节

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
    EA = 1;           //开总中断
    ConfigTimer0(1);  //配置T0定时1ms
    InitLcd1602();    //初始化液晶
    if (ReadPassword())  //读取密码并判断是否有效
    {
        LcdShowStr(0, 0, "Enter Password");  //有效时提示输入密码
        status = 0;
    }
    else
    {
        LcdShowStr(0, 0, "Set Password");  //无效时提示设置密码
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
/* 将一段内存数据拷贝到另一处，dest-目的地址，src-源地址，len-数据长度 */
void CopyMemory(unsigned char *dest, unsigned char *src, unsigned char len)
{
    while (len--)
    {
        *dest++ = *src++;
    }
}
/* 用一个指定字节填充一段内存，dest-目的地址，byt-填充用字节，len-数据长度 */
void FillMemory(unsigned char *dest, unsigned char byt, unsigned char len)
{
    while (len--)
    {
        *dest++ = byt;
    }
}
/* 内存比较函数，比较两个指针所指向的内存数据是否相同，
   ptr1-待比较指针1，ptr2-待比较指针2，len-待比较长度
   返回值-两段内存数据完全相同时返回1，不同返回0 */
bit CmpMemory(unsigned char *ptr1, unsigned char *ptr2, unsigned char len)
{
    while (len--)
    {
        if (*ptr1++ != *ptr2++)  //遇到不相等数据时即刻返回0
        {
            return 0;
        }
    }
    return 1;  //比较完全部长度数据都相等则返回1
}
/* 将一段数据按字节累加，返回获得的累加和，dat-数据指针，len-数据长度 */
unsigned char GetChkSum(unsigned char *dat, unsigned char len)
{
    unsigned char sum = 0;
    
    while (len--)
    {
        sum += *dat++;
    }
    return sum;
}
/* 从EEPROM中读取保存的密码数据，并对密码进行“累加和”校验，
   返回值-校验通过返回1，否则返回0 */
bit ReadPassword()
{
    unsigned char sum;
    unsigned char buf[PSW_MAX_LEN+1];
    
    E2Read(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //读取密码数据和校验字节
    sum = GetChkSum(buf, PSW_MAX_LEN); //计算密码数据的累加和
    if (buf[PSW_MAX_LEN] == ~sum)      //校验通过，即密码有效时，拷贝密码
    {
        CopyMemory(PswSet, buf, PSW_MAX_LEN);
        return 1;
    }
    else  //密码数据无效时，缓冲区默认成全0
    {
        FillMemory(PswSet, '\0', PSW_MAX_LEN);
        return 0;
    }
}
/* 将当前的密码数据保存到EEPROM中，同时添加校验字节 */
void SavePassword()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    CopyMemory(buf, PswSet, PSW_MAX_LEN);  //复制密码数据到缓冲区中
    buf[PSW_MAX_LEN] = ~GetChkSum(buf, PSW_MAX_LEN);  //填充最后的累加和校验字节
    E2Write(buf, PSW_SAVE_ADDR, PSW_MAX_LEN+1);  //将密码数据保存到EEPROM中
}
/* 在液晶上显示与当前输入密码位数相同的*，以指示当前输入的密码位数 */
void ShowPswCnt()
{
    unsigned char buf[PSW_MAX_LEN+1];
    
    FillMemory(buf, '*', PswCnt);
    FillMemory(buf+PswCnt, '\0', sizeof(buf)-PswCnt);
    LcdShowStr(0, 1, buf);
}
/* 数字键动作函数，keycode-数字按键的ASCII值 */
void KeyNumAction(unsigned char keycode)
{
    switch (status)
    {
        case 0: //输入密码状态时，将输入字符填充到PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 1: //设置密码状态时，将输入字符填充到PswBuf1
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf1[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        case 2: //设置密码确认状态时，将输入字符填充到PswBuf2
            if (PswCnt < PSW_MAX_LEN)
            {
                PswBuf2[PswCnt] = keycode;
                PswCnt++;
                ShowPswCnt();
            }
            break;
        default: //其它状态不予响应
            break;
    }
}
/* 回车键动作函数 */
void KeyEnterAction()
{
    switch (status)
    {
        case 0: //检查输入的密码是否正确
            LcdClearScreen();
            if (CmpMemory(PswSet, PswBuf1, PSW_MAX_LEN))
            {   //正确时显示解锁的提示
                LcdShowStr(0, 0, "Password Correct");
                LcdShowStr(0, 1, "system unlock");
                status = 4;  //进入解锁状态
            }
            else
            {   //错误时显示已锁定的提示
                LcdShowStr(0, 0, "Password Wrong");
                LcdShowStr(0, 1, "system locked");
                status = 5;  //进入锁定状态
            }
            break;
        case 1: //进入密码设置的确认步骤，即再输入一次密码
            LcdClearScreen();
            LcdShowStr(0, 0, "Confirm Password");
            PswCnt = 0;
            status = 2;
            break;
        case 2: //检查两次输入的密码是否一致
            LcdClearScreen();
            if (CmpMemory(PswBuf1, PswBuf2, PSW_MAX_LEN))
            {   //一致时保存设置的密码
                LcdShowStr(0, 0, "Enter Password");
                CopyMemory(PswSet, PswBuf1, PSW_MAX_LEN);
                SavePassword();
                FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
                PswCnt = 0;
                status = 0;  //进入密码输入状态
            }
            else
            {   //不一致时提示密码设置失败
                LcdShowStr(0, 0, "Confirm Password");
                LcdShowStr(0, 1, "Failed");
                status = 3;  //进入密码设置失败状态
            }
            break;
        case 3: //重新提示设置密码，进入密码设置状态
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        case 4: //重新设置即修改密码
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        case 5: //重新输入密码
            LcdClearScreen();
            LcdShowStr(0, 0, "Enter Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 0;
            break;
        default: //不予响应
            break;
    }
}
/* Esc键动作函数 */
void KeyEscAction()
{
    switch (status)
    {
        case 0: //输入密码状态时，清空当前输入以重新开始
            LcdClearScreen();
            LcdShowStr(0, 0, "Enter Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            break;
        case 1: //设置密码状态时，清空当前输入以重新开始
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            break;
        case 2: //设置密码确认状态时，清空输入并返回密码设置状态
            LcdClearScreen();
            LcdShowStr(0, 0, "Set Password");
            FillMemory(PswBuf1, '\0', PSW_MAX_LEN);
            FillMemory(PswBuf2, '\0', PSW_MAX_LEN);
            PswCnt = 0;
            status = 1;
            break;
        default: //其它状态不予响应
            break;
    }
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    if  ((keycode>='0') && (keycode<='9'))  //输入字符
    {
        KeyNumAction(keycode);
    }
    else if (keycode == 0x0D)  //回车键
    {
        KeyEnterAction();
    }
    else if (keycode == 0x1B)  //Esc键
    {
        KeyEscAction();
    }
}
/* 配置并启动T0，ms-T0定时时间 */
void ConfigTimer0(unsigned int ms)
{
    unsigned long tmp;  //临时变量
    
    tmp = 11059200 / 12;      //定时器计数频率
    tmp = (tmp * ms) / 1000;  //计算所需的计数值
    tmp = 65536 - tmp;        //计算定时器重载值
    tmp = tmp + 28;           //补偿中断响应延时造成的误差
    T0RH = (unsigned char)(tmp>>8);  //定时器重载值拆分为高低字节
    T0RL = (unsigned char)tmp;
    TMOD &= 0xF0;   //清零T0的控制位
    TMOD |= 0x01;   //配置T0为模式1
    TH0 = T0RH;     //加载T0重载值
    TL0 = T0RL;
    ET0 = 1;        //使能T0中断
    TR0 = 1;        //启动T0
}
/* T0中断服务函数，执行按键扫描 */
void InterruptTimer0() interrupt 1
{
    TH0 = T0RH;  //重新加载重载值
    TL0 = T0RL;
    KeyScan();   //按键扫描
}
