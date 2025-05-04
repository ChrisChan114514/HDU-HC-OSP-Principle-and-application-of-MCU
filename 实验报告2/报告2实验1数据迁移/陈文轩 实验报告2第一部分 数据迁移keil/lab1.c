#include <reg51.h>

#define EXTERNAL_START  0x1000
#define INTERNAL_START  0x30
#define DATA_LENGTH     49  // 从0x1000到0x1030，共49字节

void main() {
    unsigned char i;
    
    // 1. 初始化片外RAM：写入测试数据
    for (i = 0; i < DATA_LENGTH; i++) {
        *((unsigned char xdata *)(EXTERNAL_START + i)) = i+1;  // 存入0xFF
    }

    // 2. 从片外RAM读取，写入片内RAM
    for (i = 0; i < DATA_LENGTH; i++) {
        *((unsigned char idata *)(INTERNAL_START + i)) = *((unsigned char xdata *)(EXTERNAL_START + i));
    }

    // 3. 清零片外RAM
    for (i = 0; i < DATA_LENGTH; i++) {
        *((unsigned char xdata *)(EXTERNAL_START + i)) = 0x00;
    }

    while (1);  // 停止程序运行，便于调试观察
}