#include <reg51.h> // 单片机头文件

unsigned char code Tab[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; // 共阳数码管码段表
unsigned char Dat[] = {0, 0, 0, 0}; // 存放4位数字数组

int i; // 定义变量，作为循环
unsigned char tmp; // 定义片选变量

unsigned char KeyState = 0x0F; // 按键状态变量，初始值为高电平（未按下）

#define KEY1 (P1 & 0x01) // 按键1连接到 P1.0
#define KEY2 (P1 & 0x02) // 按键2连接到 P1.1
#define KEY3 (P1 & 0x04) // 按键3连接到 P1.2
#define KEY4 (P1 & 0x08) // 按键4连接到 P1.3

void Delay() // 延时子程序，作为数码管显示延迟
{
    unsigned char i;
    for (i = 0; i < 250; i++);
}

void ScanKeys() // 按键扫描函数
{
    // 检测按键1（P1.0）
    if ((KeyState & 0x01) && !KEY1) // 上一次状态为高电平，当前为低电平
    {
        Delay(); // 消抖
        if (!KEY1) // 再次确认按键按下
        {
            Dat[0] = (Dat[0] + 1) % 10; // 第一位数字加1
        }
    }
    KeyState = (KeyState & ~0x01) | (KEY1 ? 0x01 : 0x00); // 更新按键1状态

    // 检测按键2（P1.1）
    if ((KeyState & 0x02) && !KEY2) // 上一次状态为高电平，当前为低电平
    {
        Delay(); // 消抖
        if (!KEY2) // 再次确认按键按下
        {
            Dat[1] = (Dat[1] + 1) % 10; // 第二位数字加1
        }
    }
    KeyState = (KeyState & ~0x02) | (KEY2 ? 0x02 : 0x00); // 更新按键2状态

    // 检测按键3（P1.2）
    if ((KeyState & 0x04) && !KEY3) // 上一次状态为高电平，当前为低电平
    {
        Delay(); // 消抖
        if (!KEY3) // 再次确认按键按下
        {
            Dat[2] = (Dat[2] + 1) % 10; // 第三位数字加1
        }
    }
    KeyState = (KeyState & ~0x04) | (KEY3 ? 0x04 : 0x00); // 更新按键3状态

    // 检测按键4（P1.3）
    if ((KeyState & 0x08) && !KEY4) // 上一次状态为高电平，当前为低电平
    {
        Delay(); // 消抖
        if (!KEY4) // 再次确认按键按下
        {
            Dat[3] = (Dat[3] + 1) % 10; // 第四位数字加1
        }
    }
    KeyState = (KeyState & ~0x08) | (KEY4 ? 0x08 : 0x00); // 更新按键4状态
}

void main()
{
    while (1) // 无限循环
    {
        ScanKeys(); // 调用按键扫描函数

        tmp = 0x01; // 片选初值
        for (i = 0; i < 4; i++) // 循环4次
        {
            P2 = tmp; // 片选初值
            P0 = Tab[Dat[i]]; // 输出某一位数字的码段值
            tmp = tmp << 1; // 片选值左移一位
            Delay(); // 调用延时
        }
    }
}