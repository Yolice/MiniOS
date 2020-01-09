#ifndef INCLUDE_HEAP_H_
#define INCLUDE_HEAP_H_

#include "types.h"


// 内存块管理结构
typedef struct memory_blcok_control  //这个结构体控制着一段被分配的内存，prev和next是指向其他被分配的内存块。is_allocated记录是否被占用，length是该内存块大小
{
	struct memory_blcok_control *prev; 	// 前后内存块管理结构指针
	struct memory_blcok_control *next;
	uint32_t allocated ;	// 该内存块是否已经被申请,1表示被申请，反之0表示没有
	uint32_t length ; 	// 当前内存块的长度
	uint32_t page_addr; //该块的首个页的地址
} header_t;

// 初始化堆
void init_heap();

// 内存申请
void *kmalloc(uint32_t len);

// 内存释放
void kfree(void *p);

// 测试内核堆申请释放
void test_heap();

void test_heap_again();

#endif 	// INCLUDE_HEAP_H_