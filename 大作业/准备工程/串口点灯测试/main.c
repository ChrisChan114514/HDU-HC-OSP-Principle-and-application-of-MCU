#include <REGX52.H>

// LED�ƵĶ���
#define LED P2_0
unsigned char led_state = 1;    // LED��״̬��0Ϊ��1Ϊ��
unsigned char received_chars[3] = {0}; // �洢������յ�3���ַ�
unsigned char char_count = 0;   // ���յ����ַ�����

// ���ô���ͨ�ų�ʼ��
void uart_init(unsigned int baud)
{
	TMOD |= 0X20; // ���ü�����������ʽ2
	SCON = 0X50; // ����Ϊ������ʽ1
	PCON = 0X80; // �����ʼӱ�
	TH1 = baud; // ��������ʼֵ����
	TL1 = baud;

	// �����ж���
	ES = 1; // �򿪽����ж�
	EA = 1; // �����ж�
	TR1 =1 ; // �򿪼�����		
}
// ������������ã���ʵ����ʹ�õ�ʱ�������������Ǹ���ճ����Ȼ���޸�һЩ���������粨���ʣ�����λ��У��λ�ȵȡ�

// ����ͨ���жϺ���
void uart() interrupt 4
{
	unsigned int rec_data; // ���յ�������
										  
	RI = 0;	// ��������жϱ�־λ

	rec_data = SBUF; // �洢���յ�������
	SBUF = rec_data; // �����յ������ݷ��뵽���ͼĴ���
	
	// �洢����յ���3���ַ�
	received_chars[0] = received_chars[1];
	received_chars[1] = received_chars[2];
	received_chars[2] = rec_data;
	char_count++;
	
	// ����Ƿ��յ�����'1'
	if (char_count >= 3 && received_chars[0] == '1' && received_chars[1] == '1' && received_chars[2] == '1') {
		// �л�LED״̬
		led_state = !led_state;
		LED = !led_state;  // LED�͵�ƽ����
		// ���ü�����
		char_count = 0;
	}

	while(!TI);	// �ȴ������������
	TI = 0; // ���������ɱ�־λ				
}

void main()
{	
	uart_init(0XFA); // ������Ϊ9600
	
	// ��ʼ��LED״̬
	LED = 0;  // ��ʼ״̬ΪϨ��(�ߵ�ƽ)
	led_state = 0;

	while(1)
	{			
		// ��ѭ�����գ�ͨ���жϴ���LED����					
	}		
}
