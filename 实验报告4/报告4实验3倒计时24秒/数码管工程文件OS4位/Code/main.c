#include <reg51.h> // ��Ƭ��ͷ�ļ�

unsigned char code Tab[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; // �����������α�
unsigned char Dat[] = {0, 0, 2, 4}; // ��ʼʱ��Ϊ24��
int Dat_int=24;
int i; // �����������Ϊѭ��
unsigned char tmp; // ����Ƭѡ����
unsigned char KeyState = 0x0F; // ����״̬��������ʼֵΪ�ߵ�ƽ��δ���£�
bit isRunning = 1; // ��ʱ������״̬��־λ

#define KEY1 (P1 & 0x01) // ����1���ӵ� P1.0
#define KEY2 (P1 & 0x02) // ����2���ӵ� P1.1
#define KEY3 (P1 & 0x04) // ����3���ӵ� P1.2
#define KEY4 (P1 & 0x08) // ����4���ӵ� P1.3
#define TIM_SET 24 //�˳������ü��붨ʱ �������

void Delay() // ��ʱ�ӳ�����Ϊ�������ʾ�ӳ�
{
    unsigned char i;
    for (i = 0; i < 100; i++);
}

void Timer1_Init() // ��ʱ��1��ʼ��
{
    TMOD |= 0x10; // ���ö�ʱ��1Ϊģʽ1��16λ��ʱ����
    TH1 = 0x00;   // ��ʱ����ֵ���ֽڣ�1�붨ʱ��
    TL1 = 0x00;   // ��ʱ����ֵ���ֽ�
    ET1 = 1;      // ʹ�ܶ�ʱ��1�ж�
    EA = 1;       // ʹ�����ж�
    TR1 = 1;      // ������ʱ��1
}

void Timer1_ISR() interrupt 3 // ��ʱ��1�жϷ�����
{
    static int counter=0;
    TH1 = 0x3C;   // ��װ��ʱ����ֵ���ֽ�
    TL1 = 0xAF;   // ��װ��ʱ����ֵ���ֽ� (15535)D=(3CAF)H ��65535-50000=15535 ��ʱ50ms;

    counter++;
    if (counter==20) //50*20=1000ms 1s��ʱ���ж�
    {
        counter=0;
        Dat_int--;
        if (Dat_int<0)
        {
            Dat_int=TIM_SET;
        }
        Dat[2]=Dat_int/10;
        Dat[3]=Dat_int%10;
    }
}

void Ext0_Init() // �ⲿ�ж�0��ʼ��
{
    IT0 = 1; // �����ⲿ�ж�0Ϊ�½��ش���
    EX0 = 1; // ʹ���ⲿ�ж�0
    EA = 1;  // ʹ�����ж�
}

void Ext0_ISR() interrupt 0 // �ⲿ�ж�0������ ��������
{ 
   //�����жϺ�˵���а������£��ٽ���ɨ�裬��������ʡ����������Դ
		Delay(); //��ʱ����
		if (KEY1==0) //����1 p1.0���� ʵ�ָ�λ
		{
			Dat_int=0;
		}
		if (KEY2==0)
		{
			TR1=(TR1==1)?0:1; //�л�TR1״̬ ���ƶ�ʱ��1�������
		}
	 
}

void main()
{
    Timer1_Init(); // ��ʼ����ʱ��1
    Ext0_Init();   // ��ʼ���ⲿ�ж�0
	
    Dat[2]=TIM_SET/10; 
    Dat[3]=TIM_SET%10; //���պ궨���ʼ��Dat(�������ʾֱ�ӿ�������)
    Dat_int=TIM_SET;
	
	
    while (1) // ����ѭ�� �����ɨ��������
    {
        tmp = 0x01; // Ƭѡ��ֵ
        for (i = 0; i < 4; i++) // ѭ��4��
        {
            P2 = tmp; // Ƭѡ��ֵ
            P0 = Tab[Dat[i]]; // ���ĳһλ���ֵ����ֵ
            tmp = tmp << 1; // Ƭѡֵ����һλ
            Delay(); // ������ʱ
        }
    }
}