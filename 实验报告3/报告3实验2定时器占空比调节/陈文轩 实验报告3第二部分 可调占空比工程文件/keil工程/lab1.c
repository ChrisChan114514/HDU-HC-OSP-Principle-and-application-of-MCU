#include <reg51.h>

// 宏定义
#define FOSC 6000000       // 振荡频率 6MHz
#define STUDENT_ID 47      // 学号后两位
#define DUTY_CYCLE (10 + STUDENT_ID) // 占空比 = 10 + 学号后两位
#define PERIOD_MS 120*2      // 输出波形周期 120ms 但是0.5ms定时，所以乘2

// 定时器初值计算
#define TIMER_RELOAD (256 - (FOSC / 12 / 2000)) // 定时器重装值，0.5ms定时

sbit P2_0 = P2^0; // 定义P2.0口

unsigned int high_time = 0; // 高电平时间
unsigned int low_time = 0;  // 低电平时间

void Timer1_ISR(void) interrupt 3 {
    static unsigned int counter = 0; // 定时器计数器
    counter++;

    if (P2_0 == 1 && counter >= high_time) {
        P2_0 = 0; // 切换为低电平
        counter = 0;
        TH1 = TIMER_RELOAD; // 重装定时器初值
    } else if (P2_0 == 0 && counter >= low_time) {
        P2_0 = 1; // 切换为高电平
        counter = 0;
        TH1 = TIMER_RELOAD; // 重装定时器初值
    }
}

void main() {
    unsigned int total_time = PERIOD_MS; // 总周期时间
    high_time = (total_time * DUTY_CYCLE) / 100; // 高电平时间
    low_time = total_time - high_time;          // 低电平时间

    TMOD = 0x20; // 定时器1，方式2（8位自动重装）
    TH1 = TIMER_RELOAD; // 初始化定时器初值
    TL1 = TIMER_RELOAD;
    ET1 = 1; // 使能定时器1中断
    EA = 1;  // 开启总中断
    TR1 = 1; // 启动定时器1

    P2_0 = 1; // 初始状态为高电平

    while (1) {
        // 主循环，所有逻辑在中断中处理
    }
}