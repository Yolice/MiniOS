#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include "types.h"


// �ڴ�����ṹ
typedef struct memory_blcok_control  //����ṹ�������һ�α�������ڴ棬prev��next��ָ��������������ڴ�顣is_allocated��¼�Ƿ�ռ�ã�length�Ǹ��ڴ���С
{
	struct memory_blcok_control *prev; 	// ǰ���ڴ�����ṹָ��
	struct memory_blcok_control *next;
	uint32_t allocated ;	// ���ڴ���Ƿ��Ѿ�������,1��ʾ�����룬��֮0��ʾû��
	uint32_t length ; 	// ��ǰ�ڴ��ĳ���
	uint32_t page_addr; //�ÿ���׸�ҳ�ĵ�ַ
} header_t;

// ��ʼ����
void init_heap();

// �ڴ�����
void *kmalloc(uint32_t len);

// �ڴ��ͷ�
void kfree(void *p);

// �����ں˶������ͷ�
void test_heap();

void test_heap_again();

#endif 	// INCLUDE_HEAP_H_