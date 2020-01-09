#ifndef INCLUDE_PAGE_H
#define INCLUDE_PAGE_H

#include "multiboot.h"

// �߳�ջ�Ĵ�С
#define STACK_SIZE 8192

// ֧�ֵ���������ڴ�(512MB)
#define PMM_MAX_SIZE 0x20000000  

// �����ڴ�ҳ���С 
#define PMM_PAGE_SIZE 0x1000  //hex(0x1000)=int(4096)

// ���֧�ֵ�����ҳ�����
#define PAGE_MAX_SIZE (PMM_MAX_SIZE/PMM_PAGE_SIZE)

// ҳ���� ���� 4096 �����ַ
#define PHY_PAGE_MASK 0xFFFFF000

#define PAGE_SIZE 	4096

// �ں��ļ����ڴ��е���ʼ�ͽ���λ��
// ���������ű���Ҫ������������
extern uint8_t kern_start[];
extern uint8_t kern_end[];


// �ں�ջ��ջ��
extern uint32_t kern_stack_top;

// ��̬���������ڴ�ҳ������
extern uint32_t phy_page_count;

// ��� BIOS �ṩ�������ڴ沼��
void show_memory_map();

// ��ʼ�������ڴ����
void init_page();

// ����һ���ڴ�ҳ�������ַ
uint32_t page_alloc();

// �ͷ�������ڴ�
void page_free(uint32_t page);

#endif 	// INCLUDE_PAGE_H