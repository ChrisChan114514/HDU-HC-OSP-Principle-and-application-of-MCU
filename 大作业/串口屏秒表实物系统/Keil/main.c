#include <REGX52.H>
#include "Timer0.h"
#include "Key.h"
#include "Nixie.h"
#include "Delay.h"
#include "AT24C02.h"

unsigned char KeyNum;
unsigned char Min,Sec,MiniSec;
unsigned char RunFlag;

// 模式控制变量
#define MODE_TIMER 0       // 计时器模式
#define MODE_COUNTDOWN 1   // 倒计时模式
unsigned char CurrentMode = MODE_TIMER;  // 默认为计时器模式

// 倒计时设置值
unsigned char CDMin = 1, CDSec = 0, CDMiniSec = 0; // 倒计时默认值：1分钟

// LED灯的定义
#define LED P2_0
#define CD_LED1 P2_5      // 倒计时结束指示灯1
#define CD_LED2 P2_6      // 倒计时结束指示灯2
#define CD_LED3 P2_7      // 倒计时结束指示灯3
unsigned char led_state = 1;    
unsigned char DEAL_LED = 0; //正确处理指令后，再闪灯
// 定义通信协议
#define FRAME_HEADER 0x05  // 帧头
#define FRAME_FOOTER 0x26  // 帧尾

//计时器指令
#define CMD_STOP_OR_CONTINUE 0x30 // 命令：暂停/继续计时器 
#define CMD_RESET 0x31 //命令：RESET计时器
#define CMD_WRITE_AT 0x32 //命令：将当前时间放入AT存储器
#define CMD_READ_AT 0x33 //命令：读取AT存储器，覆盖当前时间

// 模式切换指令
#define CMD_MODE_TIMER 0x40      // 切换到计时器模式
#define CMD_MODE_COUNTDOWN 0x41  // 切换到倒计时模式

// 倒计时控制指令
#define CMD_CD_PAUSE_CONTINUE 0x50  // 倒计时暂停/继续
#define CMD_CD_RESET 0x51          // 倒计时复位到设定值
#define CMD_CD_SET_TIME 0x52       // 设置倒计时时间

// 通信协议变量
unsigned char rx_state = 0;     // 接收状态：0-等待帧头，1-等待操作数，2-等待数据低字节，3-等待数据高字节，4-等待帧尾
unsigned char rx_command = 0;   // 接收到的操作数
unsigned int rx_time_data = 0;  // 接收到的时间数据（秒数）
unsigned char rx_time_low = 0;  // 接收到的时间数据低字节
unsigned char rx_time_high = 0; // 接收到的时间数据高字节

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

// 将所有倒计时指示LED设置为指定状态
void SetCountdownLEDs(bit state)
{
    CD_LED1 = state;
    CD_LED2 = state;
    CD_LED3 = state;
}

void main()
{
	Timer0_Init();
	uart_init(0XFA); // 波特率为9600
	// 初始化LED状态
	LED = 0;  // 初始状态为点亮(低电平)
	led_state = 0;
	
	// 初始化倒计时LED为熄灭状态
	SetCountdownLEDs(1);  // 高电平熄灭
	
	while(1)
	{
		// 处理按键和命令
		if(KeyNum==1)     // K1按键按下
		{
			if(CurrentMode == MODE_TIMER)
			{
				RunFlag=!RunFlag;    // 计时器模式：启动标志位翻转
			}
			else // 倒计时模式
			{
				RunFlag=!RunFlag;    // 倒计时模式也使用相同的运行标志
			}
			KeyNum = 0; //处理完成，状态复位
			DEAL_LED=1;
		}
		if(KeyNum==2)     // K2按键按下
		{
			if(CurrentMode == MODE_TIMER)
			{
				Min=0;              // 计时器模式：分秒清0
				Sec=0;
				MiniSec=0;
			}
			else // 倒计时模式
			{
				Min=CDMin;          // 倒计时模式：复位到设定值
				Sec=CDSec;
				MiniSec=CDMiniSec;
				SetCountdownLEDs(1); // 复位指示灯
			}
			KeyNum = 0; //处理完成，状态复位

		}
		if(KeyNum==3)			//K3按键按下
		{
			AT24C02_WriteByte(0,Min);	//将分秒写入AT24C02
			Delay(5);
			AT24C02_WriteByte(1,Sec);
			Delay(5);
			AT24C02_WriteByte(2,MiniSec);
			Delay(5);
			KeyNum = 0; //处理完成，状态复位

		}
		if(KeyNum==4)			//K4按键按下
		{
			Min=AT24C02_ReadByte(0);	//读出AT24C02数据
			Sec=AT24C02_ReadByte(1);
			MiniSec=AT24C02_ReadByte(2);
			KeyNum = 0; //处理完成，状态复位

		}
		// 显示当前时间
		Nixie_SetBuf(1,Min/10);    //设置显示缓存，显示数据
		Nixie_SetBuf(2,Min%10);
		Nixie_SetBuf(3,11);
		Nixie_SetBuf(4,Sec/10);
		Nixie_SetBuf(5,Sec%10);
		Nixie_SetBuf(6,11);
		Nixie_SetBuf(7,MiniSec/10);
		Nixie_SetBuf(8,MiniSec%10);
	}
}

/**
  * @brief  时间驱动函数，在中断中调用，根据当前模式执行不同操作
  * @param  无
  * @retval 无
  */
void Sec_Loop(void)
{
	if(RunFlag)
	{
		if(CurrentMode == MODE_TIMER) // 计时器模式
		{
			MiniSec++;
			if(MiniSec>=100)
			{
				MiniSec=0;
				Sec++;
				if(Sec>=60)
				{
					Sec=0;
					Min++;
					if(Min>=60)
					{
						Min=0;
					}
				}
			}
		}
		else // 倒计时模式
		{
			if(Min==0 && Sec==0 && MiniSec==0)
			{
				// 倒计时已经结束，停止运行
				RunFlag = 0;
				
				// 点亮指示灯
				SetCountdownLEDs(0);  // 低电平点亮
				Delay(1000);         // 延时1秒
				SetCountdownLEDs(1);  // 熄灭指示灯
				
				return;
			}
			
			// 倒计时逻辑
			if(MiniSec==0)
			{
				if(Sec==0)
				{
					if(Min!=0)
					{
						Min--;
						Sec=59;
						MiniSec=99;
					}
				}
				else
				{
					Sec--;
					MiniSec=99;
				}
			}
			else
			{
				MiniSec--;
			}
		}
	}
}

// 串口通信中断函数
void uart() interrupt 4
{
	unsigned char rec_data; // 接收到的数据
										  
	RI = 0;	// 清除接收中断标志位

	rec_data = SBUF; // 存储接收到的数据
	SBUF = rec_data; // 将接收到的数据放入到发送寄存器
	
	// 根据当前状态处理接收到的数据
	switch (rx_state) {
		case 0: // 等待帧头
			if (rec_data == FRAME_HEADER) {
				rx_state = 1; // 进入等待操作数状态
			}
			break;
			
		case 1: // 等待操作数
			rx_command = rec_data; // 保存操作数
			if (rx_command == CMD_CD_SET_TIME) { 
				rx_state = 2; // 如果是设置时间命令，进入等待数据低字节状态
			} else {
				rx_state = 4; // 否则直接进入等待帧尾状态
			}
			break;
			
		case 2: // 等待数据低字节
			rx_time_low = rec_data;  // 保存时间数据低字节
			rx_state = 3;  // 进入等待数据高字节状态
			break;
			
		case 3: // 等待数据高字节
			rx_time_high = rec_data;  // 保存时间数据高字节
			rx_state = 4;  // 进入等待帧尾状态
			break;
			
		case 4: // 等待帧尾
			if (rec_data == FRAME_FOOTER) 
			{
				// 完整接收到一帧数据，根据操作数执行相应操作
				
				// 计时器模式命令
				if (rx_command == CMD_STOP_OR_CONTINUE && CurrentMode == MODE_TIMER)
				{
					KeyNum = 1; //计时器暂停/继续
					DEAL_LED=1;
				}
				else if (rx_command == CMD_RESET && CurrentMode == MODE_TIMER)
				{
					KeyNum = 2; //计时器复位
					DEAL_LED=1;
				}
				else if (rx_command == CMD_WRITE_AT)
				{
					KeyNum = 3; //将当前时间写入AT24C02
					DEAL_LED=1;
				}
				else if (rx_command == CMD_READ_AT)
				{
					KeyNum = 4; //从AT24C02读取时间
					DEAL_LED=1;
				}
				// 模式切换命令
				else if (rx_command == CMD_MODE_TIMER)
				{
					CurrentMode = MODE_TIMER;
					RunFlag = 0;  // 切换模式时停止计时
					Min = 0;
					Sec = 0;
					MiniSec = 0;
					DEAL_LED=1;
				}
				else if (rx_command == CMD_MODE_COUNTDOWN)
				{
					CurrentMode = MODE_COUNTDOWN;
					RunFlag = 0;  // 切换模式时停止计时
					Min = CDMin;  // 设置为默认倒计时值
					Sec = CDSec;
					MiniSec = CDMiniSec;
					DEAL_LED=1;
				}
				// 倒计时控制命令
				else if (rx_command == CMD_CD_PAUSE_CONTINUE && CurrentMode == MODE_COUNTDOWN)
				{
					if (CurrentMode == MODE_COUNTDOWN)
					{
						KeyNum = 1; // 倒计时暂停/继续
					}
					DEAL_LED=1;
				}
				else if (rx_command == CMD_CD_RESET && CurrentMode == MODE_COUNTDOWN)
				{
					if (CurrentMode == MODE_COUNTDOWN)
					{
						KeyNum = 2; // 倒计时复位到设定值
					}
					DEAL_LED=1;
				}
				// 设置倒计时时间
				else if (rx_command == CMD_CD_SET_TIME && CurrentMode == MODE_COUNTDOWN) 
				{
	
					// 合并两个字节得到总秒数
					rx_time_data = (unsigned int)rx_time_high << 8 | rx_time_low;
					if (rx_time_data > 3599) // 最大支持59分59秒
					{
						rx_time_data = 3599; // 限制最大值为59分59秒
					}
					else if (rx_time_data <= 1) // 最小值为1秒
					{
						rx_time_data = 1; // 限制最小值为1秒
					}
					// 计算分和秒
					CDMin = rx_time_data / 60;
					CDSec = rx_time_data % 60;
					CDMiniSec = 0;
					
					// 如果当前是倒计时模式并且没有在运行，则立即更新显示
					if (CurrentMode == MODE_COUNTDOWN && RunFlag == 0)
					{
						Min = CDMin;
						Sec = CDSec;
						MiniSec = CDMiniSec;
					}
					DEAL_LED=1;
				}
				if (DEAL_LED) // 如果需要闪烁LED指示
				{
					// 执行LED闪烁 指示收到了UART数据包 并处理了
					LED = 0;  // 点亮LED (低电平点亮)
					Delay(50);  // 延时50ms
					LED = 1;  // 熄灭LED
				}

			}
			// 不管帧尾是否正确，都回到等待帧头状态
			rx_state = 0;
			break;
	}

	while(!TI);	// 等待发送数据完成
	TI = 0; // 清除发送完成标志位				
}

void Timer0_Routine() interrupt 1
{
	static unsigned int T0Count1,T0Count2,T0Count3;
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	T0Count1++;
	if(T0Count1>=20)
	{
		T0Count1=0;
		Key_Loop();	//20ms调用一次按键驱动函数
	}
	T0Count2++;
	if(T0Count2>=2)
	{
		T0Count2=0;
		Nixie_Loop();//2ms调用一次数码管驱动函数
	}
	T0Count3++;
	if(T0Count3>=10)
	{
		T0Count3=0;
		Sec_Loop();	//10ms调用一次数秒表驱动函数
	}
}
