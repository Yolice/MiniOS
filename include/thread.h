#ifndef INCLUDE_THREAD_H_
#define INCLUDE_THREAD_H_

#include "types.h"  //������������
#include "page.h"


// ����״̬����
typedef enum task_state 
{
	TASK_RUNNABLE = 1, 	// ������
} task_state;


struct context  // �ں��̵߳��������л��������Ϣ
{
	uint32_t esp;
	uint32_t ebp;
	uint32_t ebx;
	uint32_t esi;
	uint32_t edi;
	uint32_t eflags;
};

// �����ڴ��ַ�ṹ
struct mm_struct 
{
	uint32_t *pgd_dir; 	// ����ҳ��
};

// ���̿��ƿ� PCB 
struct task_struct {
	volatile task_state state; 	// ���̵�ǰ״̬
	uint32_t 	 pid; 			// ���̱�ʶ��
	void  	*stack; 		// ���̵��ں�ջ��ַ
	struct mm_struct *mm; 		// ��ǰ���̵��ڴ��ַӳ��
	struct context context; 	// �����л���Ҫ����������Ϣ
	struct task_struct *next; 	// ����ָ��
};

extern uint32_t now_pid; // ȫ�� pid ֵ

int32_t create_thread(int (*fn)(void *), void *arg); // �ں��̴߳���

void kthread_exit();  // �߳��˳�����

// �ɵ��Ƚ�������
extern struct task_struct *ready_thread_head;   //�����̶߳���

// ��ǰ���е����� 
extern struct task_struct *current;  //ָ��ǰ���߳�

void init_thread(); // ��ʼ���ʼִ����������һ��PCB��Ϊ���߳�

void schedule(); // ����ʱ���ж��������

void switch_to(struct context *prev, struct context *next);  // �����л�

#endif // !INCLUDE_THREAD_H_
