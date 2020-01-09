#ifndef INCLUDE_PAGE_H
#define INCLUDE_PAGE_H

#include "multiboot.h"

// 线程栈的大小
#define STACK_SIZE 8192

// 支持的最大物理内存(512MB)
#define PMM_MAX_SIZE 0x20000000  

// 物理内存页框大小 
#define PMM_PAGE_SIZE 0x1000  //hex(0x1000)=int(4096)

// 最多支持的物理页面个数
#define PAGE_MAX_SIZE (PMM_MAX_SIZE/PMM_PAGE_SIZE)

// 页掩码 按照 4096 对齐地址
#define PHY_PAGE_MASK 0xFFFFF000

#define PAGE_SIZE 	4096

// 内核文件在内存中的起始和结束位置
// 在链接器脚本中要求链接器定义
extern uint8_t kern_start[];
extern uint8_t kern_end[];


// 内核栈的栈顶
extern uint32_t kern_stack_top;

// 动态分配物理内存页的总数
extern uint32_t phy_page_count;

// 输出 BIOS 提供的物理内存布局
void show_memory_map();

// 初始化物理内存管理
void init_page();

// 返回一个内存页的物理地址
uint32_t page_alloc();

// 释放申请的内存
void page_free(uint32_t page);

#endif 	// INCLUDE_PAGE_H