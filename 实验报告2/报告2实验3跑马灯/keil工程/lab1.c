#include <reg51.h>
#include <intrins.h> // 包含_crol_函数

#define LED_PORT P1
#define DELAY_MS 150 // 控制跑马灯速度

void delay(unsigned int ms) {
    unsigned int i, j;
    for(i=0; i<ms; i++)
        for(j=0; j<114; j++); //阻塞式延时一段时间
}

void main() {
    unsigned char led_pattern = 0x01; // 初始模式：0000_0001（第一个LED亮）
    
    while(1) {
        LED_PORT = led_pattern;
        delay(DELAY_MS); //软件阻塞式延时
        
        // 左移跑马灯（低电平有效）
        led_pattern = _crol_(led_pattern, 1);
        
    }
}