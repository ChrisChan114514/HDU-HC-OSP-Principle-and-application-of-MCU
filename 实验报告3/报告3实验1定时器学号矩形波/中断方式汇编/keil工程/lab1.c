/**
 * @file square_wave.c
 * @brief 使用51单片机定时器T0查询方式生成67ms方波（20ms+学号47）
 * @details 
 * - 晶振: 12MHz, 12T模式（1MHz机器周期）
 * - 方波周期: 20ms + 学号后两位47ms = 67ms
 * - 占空比: 50%（高低电平各33.5ms）
 * - 输出引脚: P1.0
 */

#include <reg51.h>

/*============== 宏定义（参数集中管理，方便修改） ==============*/
#define FOSC        12000000UL  // 晶振频率12MHz（UL表示无符号长整型）
#define STUDENT_END 47          // 学号后两位数值（用于计算周期）
#define WAVE_MS     (20 + STUDENT_END)/2  // 半周期 = (20+47)/2 = 33.5ms
#define HALF_US     (WAVE_MS * 500)       // 半周期转微秒：33.5ms * 1000/2 = 33500μs
                                          // （500 = 1000μs/ms ÷ 2，因12T模式1μs/计数）
#define T0_LOAD     (65536 - HALF_US)     // 定时器初值 = 65536 - 33500 = 32036 (0x7D24)

/*============== 硬件接口定义 ==============*/
sbit WAVE_OUT = P1^0;  // 方波输出引脚（P1.0）

/*============== 主函数 ==============*/
void main() {
    /*----- 定时器T0初始化 -----*/
    TMOD = 0x01;       // 设置T0为模式1（16位定时器）
    TH0 = T0_LOAD / 256;  // 初值高字节 = 32036/256 = 0x7D
    TL0 = T0_LOAD % 256;  // 初值低字节 = 32036%256 = 0x24
    TR0 = 1;           // 启动定时器T0
    WAVE_OUT = 0;      // 初始输出低电平

    /*----- 主循环（查询方式生成方波） -----*/
    while(1) {
        while(!TF0);   // 阻塞等待定时器溢出（TF0=1时跳出）
        TF0 = 0;       // 必须手动清除溢出标志
        WAVE_OUT = ~WAVE_OUT;  // 电平翻转（33.5ms切换一次）
        
        /* 重装初值（保持周期精确） */
        TH0 = T0_LOAD / 256;  // 重新写入高字节
        TL0 = T0_LOAD % 256;  // 重新写入低字节
    }
}
