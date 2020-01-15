#include "init.h"

char kern_stack[STACK_SIZE];  //���̵߳��ں�ջ

uint32_t kern_stack_top;  //�ں�ջ���ĵ�ַ

uint8_t flag = 0;

void thread_A()
{
	while (1)
	{
		if (flag == 1)
		{
			put_str("A");
			flag = 0;
		}
	}
}




int kern_entry()
{
	clear();
	kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE); // �л��ں�ջ
	asm volatile ("mov %0, %%esp\n\t" : : "r" (kern_stack_top)); // esp����kern_stack_top��ַ
	asm volatile ("xor %ebp, %ebp"); //ebp����
	init_all();
	enable_interrupt();
	login_shell();
	while (1);
	return 0;
}
