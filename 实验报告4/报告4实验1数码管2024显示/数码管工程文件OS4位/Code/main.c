#include <reg51.h>//单片机头文件
unsigned char code Tab[]={0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90};//共阳数码管码段表
unsigned char Dat[]={2,0,2,4};//存放4位数字数组
 
int i ,t;//定义变量，作为循环，定时计数
unsigned char tmp;//定义片选变量
unsigned char second=0,min=0;//定义秒、分变量
void Delay()//延时子程序，作为数码管显示延迟
{
	unsigned char i;
	for(i=0;i<250;i++);
}
 
void main()
{
//	IT0=1;EX0=1;//开外部中断0和外部中断0允许
//	IT1=1;EX1=1;//开外部中断1和外部中断1允许
//	EA=1;//允许所有中断
//	ET0=1;//允许T0中断
//	TMOD=0x01;//T0方式1计时0.05S
//	TH0=-50000/256;//定时器T0的高八位
//	TL0=-50000%256;//定时器T0的低八位
//	TR0=1;//启动定时器0
	while(1)//无限循环
	{
		tmp=0x01;//片选初值
		for(i=0;i<4;i++)//循环4次
		{
			P2=tmp;//片选初值
			P0=Tab[Dat[i]];//输出某一位数字的码段值
			tmp=tmp<<1;//片选值左移一位
			Delay();//调用延时
		}
	}
}
 
//void intserv1 (void) interrupt 1 using 1//定时器0中断服务子程序
//{
//	TH0=-50000/256;//定时器高八位赋值
//	TL0=-50000%256;//定时器低八位赋值
//	t++;//变量t加1
//	if(t==20)//20次到，即1秒到
//	{
//		t=0;//变量t清零
//		second++;//秒加1
//		if(second>=60)//60秒到
//		{
//			second=0;//秒变量清零
//			min++;//分加1
//			if(min>=60)//60分到
//				min=0;//分变量清零
//		}
//		Dat[0]=min/10;//计算分高位
//		Dat[1]=min%10;//计算分低位
//		Dat[2]=second/10;//计算秒高位
//		Dat[3]=second%10;//计算秒低位
//	}
//}
// 
//void intersvr0 (void) interrupt 0 using 1//外部中断0子程序
//{
//	min++;//分加1
//	if(min>=60)//如果分到60
//		min=0;//分清零
//}
// 
//void intersvr1 (void) interrupt 2 using 1//外部中断1子程序
//{
//	second++;//秒加1
//	if(second>=60)//如果秒到60
//		second=0;//秒清零
//}