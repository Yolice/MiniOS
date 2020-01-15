#include "thread.h"
#include "heap.h"
#include "string.h"

uint32_t now_pid = 0; // 全局 pid 值

struct task_struct *ready_thread_head = NULL;  //就绪态线程链表

struct task_struct *current = NULL;  //当前运行的线程



int32_t create_thread(int (*fn)(void *), void *arg) // 内核线程创建
{
    struct task_struct* new_thread = (struct task_struct*)kmalloc(STACK_SIZE); //malloc一个PCB结构体的内存块
	if(new_thread == NULL)
	{
		put_str("kmalloc error\n");
	}
	memset(new_thread,0,sizeof(struct task_struct)); //把刚申请的PCB结构体初始化0


	new_thread->state = TASK_RUNNABLE; //即新创建的线程为就绪态
	new_thread->stack = current;  //current是当前运行任务的PCB地址，第一次创建线程后当前current是主线程的地址
	new_thread->pid = now_pid++;  //赋予新创建线程的pid

	uint32_t* stack_top=(uint32_t*)((uint32_t)new_thread + STACK_SIZE); //new_thread指向栈底，加上栈大小，指向栈顶
	

	*(--stack_top) = (uint32_t)arg; //arg参数地址放入栈顶，栈指针减一
	*(--stack_top) = (uint32_t)kthread_exit; //线程退出函数地址入栈
	*(--stack_top) = (uint32_t)fn; //在线程内执行的函数地址入栈

	new_thread->context.esp = (uint32_t)new_thread + STACK_SIZE - sizeof(uint32_t)*3; //上面三个入栈操作后，esp减三

	new_thread->context.eflags = 0x200; // 设置新任务的标志寄存器未屏蔽中断，很重要

	new_thread->next = ready_thread_head;  //new_thread即将加入就绪态队列，最为加入后的最后一个线程元素，其next一定是指向ready_thread_head头部的。这样形成了一个环

	struct task_struct *tail = ready_thread_head;
	while(tail->next != ready_thread_head) //ready_thread_head就绪态的线程链表里，最后一个线程的next永远指向ready_thread_head头部。这里把tail定位到最后一个线程
    {
		tail=tail->next;
	}
	tail->next = new_thread; //最后一个线程，不再指向ready_thread_head头部地址而是指向新创建的线程。即新创建的线程加入了就绪态链表

	//put_str("new_ thread pid is:");
	//put_int(new_thread->pid);
	//put_str("\n");

	return new_thread->pid;  //返回新创建的线程的pid
}

void kthread_exit()
{
	register uint32_t val asm ("eax");
    
	put_str("thread_exit with value:");
	put_hex(val);

	while (1);
}

void init_thread()
{
	current = (struct task_struct *)(kern_stack_top - STACK_SIZE); // 为当前执行流创建信息结构体 该结构位于当前执行流的栈最低端
	current->state = TASK_RUNNABLE; //主线程为就绪态
	current->pid = now_pid++; //主线程的pid为0，之后所有创建的线程共用此pid变量
	current->stack = current; 	// 该成员指向栈低地址
	current->mm=NULL; 
	current->next = current; //主线程下一个线程还是指向自己的PCB

	ready_thread_head = current; //此线程进入就绪态线程的链表

	put_str("thread_init Done\n");
}

void schedule()
{
	if (current) 
	{
		if (current != current->next) //如果当前只有主线程一个线程就不用发生调度
		{
		struct task_struct *prev = current;
		current = current->next;
		switch_to(&(prev->context), &(current->context));
	    }
	}
}

