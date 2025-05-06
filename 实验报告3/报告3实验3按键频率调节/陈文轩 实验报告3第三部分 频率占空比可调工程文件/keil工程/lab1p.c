#include <reg51.h>

// 宏定义
#define FOSC 12000000       // 晶振频率 12MHz
#define TIMER_RELOAD_1KHZ (65536 - 1 ) // 1KHz定时器初值（半周期） 
//中断程序时间占用 影响频率较大 适当调参 但是周期是1.5ms左右
#define TIMER_RELOAD_100HZ (65536 - FOSC / 12 / 2 / 1000 ) // 100Hz定时器初值
#define TIMER_RELOAD_10HZ (65536 - FOSC / 12 / 2 / 100 )   // 10Hz定时器初值
//因为是半周期翻转电平 所以再除以2补偿

sbit P2_0 = P2^0; // 方波输出引脚
unsigned char mode = 0;       // 模式切换变量
unsigned char duty_cycle = 50; // 占空比，初始为50%

// 外部中断0服务函数（切换频率模式）
void INT0_ISR(void) interrupt 0 {
    mode = (mode + 1) % 4; // 模式循环切换
}

// 外部中断1服务函数（切换占空比）
void INT1_ISR(void) interrupt 2 {
    duty_cycle += 10; // 占空比增加10%
    if (duty_cycle > 90) {
        duty_cycle = 10; // 超过90%后回到10%
    }
}

// 定时器0中断服务函数
void Timer0_ISR(void) interrupt 1 {
    static unsigned int high_count = 0; // 高电平计数
    static unsigned int low_count = 0;  // 低电平计数
    static unsigned char state = 0;     // 当前状态：0为高电平，1为低电平
    static int counter_1HZ = 0;         // 基于10Hz的分频计数

    // 根据模式设置定时器初值
    unsigned int timer_reload = (mode == 0) ? TIMER_RELOAD_1KHZ :
                                (mode == 1) ? TIMER_RELOAD_100HZ :
                                (mode == 2) ? TIMER_RELOAD_10HZ :
                                              TIMER_RELOAD_10HZ; // 1Hz基于10Hz分频

    TH0 = timer_reload >> 8;
    TL0 = timer_reload & 0xFF;

    if (mode == 3) { // 1Hz模式
        counter_1HZ++;
        if (counter_1HZ == 10) { // 10Hz分频为1Hz
            counter_1HZ = 0;
            if (state == 0) { // 高电平状态
                high_count++;
                if (high_count >= duty_cycle / 10) { // 高电平持续时间达到占空比
                    high_count = 0;
                    state = 1; // 切换到低电平
                    P2_0 = 0;
                }
            } else { // 低电平状态
                low_count++;
                if (low_count >= (10 - duty_cycle / 10)) { // 低电平持续时间达到占空比
                    low_count = 0;
                    state = 0; // 切换到高电平
                    P2_0 = 1;
                }
            }
        }
    } else { // 其他模式
        if (state == 0) { // 高电平状态
            high_count++;
            if (high_count >= duty_cycle / 10) { // 高电平持续时间达到占空比
                high_count = 0;
                state = 1; // 切换到低电平
                P2_0 = 0;
            }
        } else { // 低电平状态
            low_count++;
            if (low_count >= (10 - duty_cycle / 10)) { // 低电平持续时间达到占空比
                low_count = 0;
                state = 0; // 切换到高电平
                P2_0 = 1;
            }
        }
    }
}

void main() {
    // 初始化外部中断0（切换频率模式）
    IT0 = 1;  // 下降沿触发
    EX0 = 1;  // 使能外部中断0

    // 初始化外部中断1（切换占空比）
    IT1 = 1;  // 下降沿触发
    EX1 = 1;  // 使能外部中断1

    // 初始化总中断
    EA = 1;   // 开启总中断

    // 初始化定时器0
    TMOD = 0x01; // 定时器0，模式1（16位定时）
    TH0 = TIMER_RELOAD_1KHZ >> 8;
    TL0 = TIMER_RELOAD_1KHZ & 0xFF;
    ET0 = 1; // 使能定时器0中断
    TR0 = 1; // 启动定时器0

    P2_0 = 0; // 初始化P2.0为低电平

    while (1) {
        // 主循环，所有逻辑在中断中处理
    }
}