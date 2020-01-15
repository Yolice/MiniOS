#include "timer.h"
#include "print.h"
#include "idt.h"
#include "io.h"

void timer_callback()  //������ڻ���ʱ���жϵĵ���
{
    schedule();
	
}

void init_timer()
{
    
    register_interrupt_handler(IRQ0, timer_callback); // ע��ʱ����صĴ�����

    // Intel 8253/8254 PITоƬ I/O�˿ڵ�ַ��Χ��40h~43h
    uint32_t divisor = 1193180 / 10; // ����Ƶ��Ϊ 1193180��frequency ��ÿ���жϴ���

    // D7 D6 D5 D4 D3 D2 D1 D0
    // 0  0  1  1  0  1  1  0
    // ������ 36 H
    // ���� 8253/8254 оƬ������ģʽ 3 ��
    outb(0x43, 0x36);

    // ��ֵ��ֽں͸��ֽ�
    uint8_t low = (uint8_t)(divisor & 0xFF);
    uint8_t hign = (uint8_t)((divisor >> 8) & 0xFF);
    
    // �ֱ�д����ֽں͸��ֽ�
    outb(0x40, low);
    outb(0x40, hign);
    put_str("timer_inital Done\n");
}