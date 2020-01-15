#include "thread.h"
#include "heap.h"
#include "string.h"

uint32_t now_pid = 0; // ȫ�� pid ֵ

struct task_struct *ready_thread_head = NULL;  //����̬�߳�����

struct task_struct *current = NULL;  //��ǰ���е��߳�



int32_t create_thread(int (*fn)(void *), void *arg) // �ں��̴߳���
{
    struct task_struct* new_thread = (struct task_struct*)kmalloc(STACK_SIZE); //mallocһ��PCB�ṹ����ڴ��
	if(new_thread == NULL)
	{
		put_str("kmalloc error\n");
	}
	memset(new_thread,0,sizeof(struct task_struct)); //�Ѹ������PCB�ṹ���ʼ��0


	new_thread->state = TASK_RUNNABLE; //���´������߳�Ϊ����̬
	new_thread->stack = current;  //current�ǵ�ǰ���������PCB��ַ����һ�δ����̺߳�ǰcurrent�����̵߳ĵ�ַ
	new_thread->pid = now_pid++;  //�����´����̵߳�pid

	uint32_t* stack_top=(uint32_t*)((uint32_t)new_thread + STACK_SIZE); //new_threadָ��ջ�ף�����ջ��С��ָ��ջ��
	

	*(--stack_top) = (uint32_t)arg; //arg������ַ����ջ����ջָ���һ
	*(--stack_top) = (uint32_t)kthread_exit; //�߳��˳�������ַ��ջ
	*(--stack_top) = (uint32_t)fn; //���߳���ִ�еĺ�����ַ��ջ

	new_thread->context.esp = (uint32_t)new_thread + STACK_SIZE - sizeof(uint32_t)*3; //����������ջ������esp����

	new_thread->context.eflags = 0x200; // ����������ı�־�Ĵ���δ�����жϣ�����Ҫ

	new_thread->next = ready_thread_head;  //new_thread�����������̬���У���Ϊ���������һ���߳�Ԫ�أ���nextһ����ָ��ready_thread_headͷ���ġ������γ���һ����

	struct task_struct *tail = ready_thread_head;
	while(tail->next != ready_thread_head) //ready_thread_head����̬���߳���������һ���̵߳�next��Զָ��ready_thread_headͷ���������tail��λ�����һ���߳�
    {
		tail=tail->next;
	}
	tail->next = new_thread; //���һ���̣߳�����ָ��ready_thread_headͷ����ַ����ָ���´������̡߳����´������̼߳����˾���̬����

	//put_str("new_ thread pid is:");
	//put_int(new_thread->pid);
	//put_str("\n");

	return new_thread->pid;  //�����´������̵߳�pid
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
	current = (struct task_struct *)(kern_stack_top - STACK_SIZE); // Ϊ��ǰִ����������Ϣ�ṹ�� �ýṹλ�ڵ�ǰִ������ջ��Ͷ�
	current->state = TASK_RUNNABLE; //���߳�Ϊ����̬
	current->pid = now_pid++; //���̵߳�pidΪ0��֮�����д������̹߳��ô�pid����
	current->stack = current; 	// �ó�Աָ��ջ�͵�ַ
	current->mm=NULL; 
	current->next = current; //���߳���һ���̻߳���ָ���Լ���PCB

	ready_thread_head = current; //���߳̽������̬�̵߳�����

	put_str("thread_init Done\n");
}

void schedule()
{
	if (current) 
	{
		if (current != current->next) //�����ǰֻ�����߳�һ���߳̾Ͳ��÷�������
		{
		struct task_struct *prev = current;
		current = current->next;
		switch_to(&(prev->context), &(current->context));
	    }
	}
}

