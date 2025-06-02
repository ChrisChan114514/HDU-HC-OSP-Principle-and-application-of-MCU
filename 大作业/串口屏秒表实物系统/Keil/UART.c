#include <REGX52.H>

void uart_init(unsigned int baud) //9600:0XFA
{
	TMOD |= 0X20; // 设置计数器工作方式2
	SCON = 0X50; // 设置为工作方式1
	PCON = 0X80; // 波特率加倍
	TH1 = baud; // 计数器初始值设置
	TL1 = baud;

	// 配置中断器
	ES = 1; // 打开接收中断
	EA = 1; // 打开总中断
	TR1 =1 ; // 打开计数器		
}

// 串口通信中断函数
void uart() interrupt 4
{
	unsigned int rec_data; // 接收到的数据
										  
	RI = 0;	// 清除接收中断标志位

	rec_data = SBUF; // 存储接收到的数据
	SBUF = rec_data; // 将接收到的数据放入到发送寄存器
	
	// 存储最近收到的3个字符
	received_chars[0] = received_chars[1];
	received_chars[1] = received_chars[2];
	received_chars[2] = rec_data;
	char_count++;
	
	// 检查是否收到三个'1'
	if (char_count >= 3 && received_chars[0] == '1' && received_chars[1] == '1' && received_chars[2] == '1') {
		// 切换LED状态
		led_state = !led_state;
		LED = !led_state;  // LED低电平点亮
		// 重置计数器
		char_count = 0;
	}

	while(!TI);	// 等待发送数据完成
	TI = 0; // 清除发送完成标志位				
}