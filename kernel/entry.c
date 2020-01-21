#include "init.h"

char kern_stack[STACK_SIZE];  //主线程的内核栈

uint32_t kern_stack_top;  //内核栈顶的地址

uint8_t flag = 0;





int kern_entry()
{
	clear();
	kern_stack_top = ((uint32_t)kern_stack + STACK_SIZE); // 切换内核栈
	asm volatile ("mov %0, %%esp\n\t" : : "r" (kern_stack_top)); // esp加载kern_stack_top地址
	asm volatile ("xor %ebp, %ebp"); //ebp清零
	init_all();
	enable_interrupt();
	login_shell();
	while (1);
	return 0;
}
