/*
*******************************************************************************
*                     《手把手教你学51单片机(C语言版)》
*                    配套 KST-51 单片机开发板 示例源代码
*
*         (c) 版权所有 2014 金沙滩工作室/清华大学出版社 保留所有权利
*                 获取更多资料请访问：http://www.kingst.org
*
* 文件名：main.c
* 描  述：第9章 按键控制步进电机转动的示例
* 版本号：v1.0.0
* 备  注：详情见第9章9.3.6节
*******************************************************************************
*/

#include <reg52.h>

sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code KeyCodeMap[4][4] = { //矩阵按键编号到标准键盘键码的映射表
    { 0x31, 0x32, 0x33, 0x26 }, //数字键1、数字键2、数字键3、向上键
    { 0x34, 0x35, 0x36, 0x25 }, //数字键4、数字键5、数字键6、向左键
    { 0x37, 0x38, 0x39, 0x28 }, //数字键7、数字键8、数字键9、向下键
    { 0x30, 0x1B, 0x0D, 0x27 }  //数字键0、ESC键、  回车键、 向右键
};
unsigned char KeySta[4][4] = {  //全部矩阵按键的当前状态
    {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
};
signed long beats = 0;  //电机转动节拍总数

void KeyDriver();

void main()
{
    EA = 1;       //使能总中断
    TMOD = 0x01;  //设置T0为模式1
    TH0  = 0xFC;  //为T0赋初值0xFC67，定时1ms
    TL0  = 0x67;
    ET0  = 1;     //使能T0中断
    TR0  = 1;     //启动T0
	
    while (1)
    {
        KeyDriver();   //调用按键驱动函数
    }
}
/* 步进电机启动函数，angle-需转过的角度 */
void StartMotor(signed long angle)
{
    //在计算前关闭中断，完成后再打开，以避免中断打断计算过程而造成错误
    EA = 0;
    beats = (angle * 4076) / 360; //实测为4076拍转动一圈
    EA = 1;
}
/* 步进电机停止函数 */
void StopMotor()
{
    EA = 0;
    beats = 0;
    EA = 1;
}
/* 按键动作函数，根据键码执行相应的操作，keycode-按键键码 */
void KeyAction(unsigned char keycode)
{
    static bit dirMotor = 0;  //电机转动方向
    
    if ((keycode>=0x30) && (keycode<=0x39))  //控制电机转动1-9圈
    {
        if (dirMotor == 0)
            StartMotor(360*(keycode-0x30));
        else
            StartMotor(-360*(keycode-0x30));
    }
    else if (keycode == 0x26)  //向上键，控制转动方向为正转
    {
        dirMotor = 0;
    }
    else if (keycode == 0x28)  //向下键，控制转动方向为反转
    {
        dirMotor = 1;
    }
    else if (keycode == 0x25)  //向左键，固定正转90度
    {
        StartMotor(90);
    }
    else if (keycode == 0x27)  //向右键，固定反转90度
    {
        StartMotor(-90);
    }
    else if (keycode == 0x1B)  //Esc键，停止转动
    {
        StopMotor();
    }
}
/* 按键驱动函数，检测按键动作，调度相应动作函数，需在主循环中调用 */
void KeyDriver()
{
    unsigned char i, j;
    static unsigned char backup[4][4] = {  //按键值备份，保存前一次的值
        {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1},  {1, 1, 1, 1}
    };
    
    for (i=0; i<4; i++)  //循环检测4*4的矩阵按键
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])    //检测按键动作
            {
                if (backup[i][j] != 0)           //按键按下时执行动作
                {
                    KeyAction(KeyCodeMap[i][j]); //调用按键动作函数
                }
                backup[i][j] = KeySta[i][j];     //刷新前一次的备份值
            }
        }
    }
}
/* 按键扫描函数，需在定时中断中调用，推荐调用间隔1ms */
void KeyScan()
{
    unsigned char i;
    static unsigned char keyout = 0;   //矩阵按键扫描输出索引
    static unsigned char keybuf[4][4] = {  //矩阵按键扫描缓冲区
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF},
        {0xFF, 0xFF, 0xFF, 0xFF},  {0xFF, 0xFF, 0xFF, 0xFF}
    };

    //将一行的4个按键值移入缓冲区
    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4;
    //消抖后更新按键状态
    for (i=0; i<4; i++)  //每行4个按键，所以循环4次
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
        {   //连续4次扫描值为0，即4*4ms内都是按下状态时，可认为按键已稳定的按下
            KeySta[keyout][i] = 0;
        }
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
        {   //连续4次扫描值为1，即4*4ms内都是弹起状态时，可认为按键已稳定的弹起
            KeySta[keyout][i] = 1;
        }
    }
    //执行下一次的扫描输出
    keyout++;                //输出索引递增
    keyout = keyout & 0x03;  //索引值加到4即归零
    switch (keyout)          //根据索引，释放当前输出引脚，拉低下次的输出引脚
    {
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0; break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0; break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0; break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0; break;
        default: break;
    }
}
/* 电机转动控制函数 */
void TurnMotor()
{
    unsigned char tmp;  //临时变量
    static unsigned char index = 0;  //节拍输出索引
    unsigned char code BeatCode[8] = {  //步进电机节拍对应的IO控制代码
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };
    
    if (beats != 0)  //节拍数不为0则产生一个驱动节拍
    {
        if (beats > 0)  //节拍数大于0时正转
        {
            index++;               //正转时节拍输出索引递增
            index = index & 0x07;  //用&操作实现到8归零
            beats--;               //正转时节拍计数递减
        }
        else            //节拍数小于0时反转
        {
            index--;               //反转时节拍输出索引递减
            index = index & 0x07;  //用&操作同样可以实现到-1时归7
            beats++;               //反转时节拍计数递增
        }
        tmp = P1;                    //用tmp把P1口当前值暂存
        tmp = tmp & 0xF0;            //用&操作清零低4位
        tmp = tmp | BeatCode[index]; //用|操作把节拍代码写到低4位
        P1  = tmp;                   //把低4位的节拍代码和高4位的原值送回P1
    }
    else  //节拍数为0则关闭电机所有的相
    {
        P1 = P1 | 0x0F;
    }
}
/* T0中断服务函数，用于按键扫描与电机转动控制 */
void InterruptTimer0() interrupt 1
{
    static bit div = 0;
    
    TH0 = 0xFC;  //重新加载初值
    TL0 = 0x67;
    KeyScan();   //执行按键扫描
    //用一个静态bit变量实现二分频，即2ms定时，用于控制电机
    div = ~div;
    if (div == 1)
    {
        TurnMotor();
    }
}
