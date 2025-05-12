#include <reg51.h> // ��Ƭ��ͷ�ļ�

unsigned char code Tab[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; // �����������α�
unsigned char Dat[] = {0, 0, 0, 0}; // ���4λ��������

int i; // �����������Ϊѭ��
unsigned char tmp; // ����Ƭѡ����

unsigned char KeyState = 0x0F; // ����״̬��������ʼֵΪ�ߵ�ƽ��δ���£�

#define KEY1 (P1 & 0x01) // ����1���ӵ� P1.0
#define KEY2 (P1 & 0x02) // ����2���ӵ� P1.1
#define KEY3 (P1 & 0x04) // ����3���ӵ� P1.2
#define KEY4 (P1 & 0x08) // ����4���ӵ� P1.3

void Delay() // ��ʱ�ӳ�����Ϊ�������ʾ�ӳ�
{
    unsigned char i;
    for (i = 0; i < 250; i++);
}

void ScanKeys() // ����ɨ�躯��
{
    // ��ⰴ��1��P1.0��
    if ((KeyState & 0x01) && !KEY1) // ��һ��״̬Ϊ�ߵ�ƽ����ǰΪ�͵�ƽ
    {
        Delay(); // ����
        if (!KEY1) // �ٴ�ȷ�ϰ�������
        {
            Dat[0] = (Dat[0] + 1) % 10; // ��һλ���ּ�1
        }
    }
    KeyState = (KeyState & ~0x01) | (KEY1 ? 0x01 : 0x00); // ���°���1״̬

    // ��ⰴ��2��P1.1��
    if ((KeyState & 0x02) && !KEY2) // ��һ��״̬Ϊ�ߵ�ƽ����ǰΪ�͵�ƽ
    {
        Delay(); // ����
        if (!KEY2) // �ٴ�ȷ�ϰ�������
        {
            Dat[1] = (Dat[1] + 1) % 10; // �ڶ�λ���ּ�1
        }
    }
    KeyState = (KeyState & ~0x02) | (KEY2 ? 0x02 : 0x00); // ���°���2״̬

    // ��ⰴ��3��P1.2��
    if ((KeyState & 0x04) && !KEY3) // ��һ��״̬Ϊ�ߵ�ƽ����ǰΪ�͵�ƽ
    {
        Delay(); // ����
        if (!KEY3) // �ٴ�ȷ�ϰ�������
        {
            Dat[2] = (Dat[2] + 1) % 10; // ����λ���ּ�1
        }
    }
    KeyState = (KeyState & ~0x04) | (KEY3 ? 0x04 : 0x00); // ���°���3״̬

    // ��ⰴ��4��P1.3��
    if ((KeyState & 0x08) && !KEY4) // ��һ��״̬Ϊ�ߵ�ƽ����ǰΪ�͵�ƽ
    {
        Delay(); // ����
        if (!KEY4) // �ٴ�ȷ�ϰ�������
        {
            Dat[3] = (Dat[3] + 1) % 10; // ����λ���ּ�1
        }
    }
    KeyState = (KeyState & ~0x08) | (KEY4 ? 0x08 : 0x00); // ���°���4״̬
}

void main()
{
    while (1) // ����ѭ��
    {
        ScanKeys(); // ���ð���ɨ�躯��

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