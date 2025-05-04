#include <reg51.h>  // 51单片机头文件

sbit LED = P1^0;    // LED连接P1.0

// 定时器初始化（方式1，16位定时器）
void Timer0_Init() {
    TMOD |= 0x01;    // 定时器0，模式1
    TH0 = 0x3C;      // 定时50ms（12MHz晶振）
    TL0 = 0xB0;      //初值 = 65536 - 50000 = 15536 = 0x3CB0 
	                   //即计数50000次，就是50ms=50000*1us
    ET0 = 1;         // 允许定时器0中断
    EA = 1;          // 全局中断使能
    TR0 = 1;         // 启动定时器0
}

// 定时器0中断服务程序
void Timer0_ISR() interrupt 1 {
    static unsigned int count = 0;
    TH0 = 0x3C;      // 重新装载初值 手动赋值装载
    TL0 = 0xB0;
    count++;
    if (count >= 20) {  // 20*50ms = 1s
        count = 0;
        LED = ~LED;     // LED状态取反
    }
}

void main() {
    LED = 0;          // 初始状态（LED灭）
    Timer0_Init();    // 初始化定时器
    while (1);        // 主循环（中断驱动）
}
