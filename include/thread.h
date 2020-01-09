#ifndef INCLUDE_THREAD_H_
#define INCLUDE_THREAD_H_

#include "types.h"  //导入数据类型
#include "page.h"


// 进程状态描述
typedef enum task_state 
{
	TASK_RUNNABLE = 1, 	// 可运行
} task_state;


struct context  // 内核线程的上下文切换保存的信息
{
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
};

// 进程内存地址结构
struct mm_struct 
{
	uint32_t *pgd_dir; 	// 进程页表
};

// 进程控制块 PCB 
struct task_struct {
	volatile task_state state; 	// 进程当前状态
	uint32_t 	 pid; 			// 进程标识符
	void  	*stack; 		// 进程的内核栈地址
	struct mm_struct *mm; 		// 当前进程的内存地址映像
	struct context context; 	// 进程切换需要的上下文信息
	struct task_struct *next; 	// 链表指针
};

extern uint32_t now_pid; // 全局 pid 值

int32_t create_thread(int (*fn)(void *), void *arg); // 内核线程创建

void kthread_exit();  // 线程退出函数

// 可调度进程链表
extern struct task_struct *ready_thread_head;   //就绪线程队列

// 当前运行的任务 
extern struct task_struct *current;  //指向当前的线程

void init_thread(); // 初始化最开始执行流，给他一个PCB成为主线程

void schedule(); // 基于时间中断任务调度

void switch_to(struct context *prev, struct context *next);  // 任务切换

#endif // !INCLUDE_THREAD_H_
