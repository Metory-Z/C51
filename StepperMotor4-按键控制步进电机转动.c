#include<reg52.h>

sbit KEY_IN_1  = P2^4;
sbit KEY_IN_2  = P2^5;
sbit KEY_IN_3  = P2^6;
sbit KEY_IN_4  = P2^7;
sbit KEY_OUT_1 = P2^3;
sbit KEY_OUT_2 = P2^2;
sbit KEY_OUT_3 = P2^1;
sbit KEY_OUT_4 = P2^0;

unsigned char code KeyCodeMap[4][4] =
    {
        {0x31,0x32,0x33,0x26}, // 1,2,3,left
        {0x34,0x35,0x36,0x25}, // 4,5,6,up
        {0x37,0x38,0x39,0x28}, // 7,8,9,right
        {0x30,0x1B,0x0D,0x27}  // 0,Esc,Enter,down
    };

unsigned char KeySta[4][4] = 
    {
        {1,1,1,1}, {1,1,1,1}, {1,1,1,1}, {1,1,1,1}
    };

signed long beats = 0;// 有符号数

void KeyDriver();

void main()
{
    EA = 1;
    TMOD = 0x01;
    TH0 = 0xFC;
    TL0 = 0x67;// 1ms
    ET0 = 1;
    TR0 = 1;// 启动T0

    while (1)
    {
        KeyDriver();
    } 
}

void StartMotor(signed long angle)
{
    EA = 0;
    beats = (angle * 4076) / 360;
    EA = 1;
}

void StopMotor()
{
    EA = 0;
    beats = 0;
    EA = 1;
}

void KeyAction(unsigned char keycode)
{
    static bit dirMotor = 0;// motor direction

    if ((keycode>=0x30) && (keycode<=0x39))
    {
        if (dirMotor == 0)
            StartMotor(360*(keycode-0x30));
        else
            StartMotor(-360*(keycode-0x30));
    }
    else if (keycode == 0x26)
    {
        dirMotor = 0;// up -> clockwise
    }
    else if (keycode == 0x28)
    {
        dirMotor = 1;// down -> anticlockwise
    }
    else if (keycode == 0x25)
    {
        StartMotor(90);// left -> clockwise 90 degree
    }
    else if (keycode == 0x27)
    {
        StartMotor(-90);// right -> anticlockwise 90 degree
    }
    else if (keycode == 0x1B)
    {
        StopMotor();
    }
}

void KeyDriver()
{
    // 判断哪个按键有动作,执行KeyAction()
    unsigned char i, j;
    static unsigned char backup[4][4] = 
    {
        {1,1,1,1}, {1,1,1,1}, {1,1,1,1}, {1,1,1,1}
    };

    for (i=0; i<4; i++)
    {
        for (j=0; j<4; j++)
        {
            if (backup[i][j] != KeySta[i][j])
            {
                if (backup[i][j] != 0)
                {
                    KeyAction(KeyCodeMap[i][j]);
                }
                backup[i][j] = KeySta[i][j];
            }
        }
    }
}

void KeyScan()
{
    
    // 获取4次扫描值，执行KeyDriver()
    unsigned char i;
    static unsigned char keyout = 0;
    static unsigned char keybuf[4][4] = 
    {
        {0xFF, 0xFF, 0xFF, 0xFF}, 
        {0xFF, 0xFF, 0xFF, 0xFF}, 
        {0xFF, 0xFF, 0xFF, 0xFF}, 
        {0xFF, 0xFF, 0xFF, 0xFF}
    };

    keybuf[keyout][0] = (keybuf[keyout][0] << 1) | KEY_IN_1;
    keybuf[keyout][1] = (keybuf[keyout][1] << 1) | KEY_IN_2;
    keybuf[keyout][2] = (keybuf[keyout][2] << 1) | KEY_IN_3;
    keybuf[keyout][3] = (keybuf[keyout][3] << 1) | KEY_IN_4; 
    
    for (i=0; i<4; i++)
    {
        if ((keybuf[keyout][i] & 0x0F) == 0x00)
            KeySta[keyout][i] = 0;
        else if ((keybuf[keyout][i] & 0x0F) == 0x0F)
            KeySta[keyout][i] = 1;
    }
    keyout++;
    keyout = keyout & 0x03;
    // 当keyout 等于4(100) 时，与0x03(011) 做与运算后，归零
    switch(keyout)
    {
        // 先扫描KeyIn，后切换KeyOut
        case 0: KEY_OUT_4 = 1; KEY_OUT_1 = 0;break;
        case 1: KEY_OUT_1 = 1; KEY_OUT_2 = 0;break;
        case 2: KEY_OUT_2 = 1; KEY_OUT_3 = 0;break;
        case 3: KEY_OUT_3 = 1; KEY_OUT_4 = 0;break;
        default: break;
    }
}

void TurnMotor()
{
    unsigned char tmp;
    static unsigned char index = 0;
    unsigned char code BeatCode[8] = {
        0xE, 0xC, 0xD, 0x9, 0xB, 0x3, 0x7, 0x6
    };

    if (beats != 0)
    {
        if (beats > 0)
        {
            index++;
            index = index & 0x07;
            beats--;
        }
        else
        {
            index--;
            index = index & 0x07;
            beats++;
        }
        tmp = P1;
        tmp = tmp & 0xF0;
        tmp = tmp | BeatCode[index];
        P1 = tmp;
    }
    else
        P1 = P1 | 0x0F;
}

void InterruptTimer0() interrupt 1
{
    static bit div = 0;

    TH0 = 0xFC;
    TL0 = 0x67;
    KeyScan();
    div = ~div;
    if (div == 1)
        TurnMotor();// 定时1ms, 步进电机需要2ms
}