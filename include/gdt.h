#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_
#include "types.h"

// 全局描述符类型
typedef struct gdt_struct { //按照GDT的定义，低32位又段界限和低段基址组成，高32位又其他4个8位段组成。生成GDT表时候，按照GDT定义拼接即可
    uint16_t limit_low;     // 段界限   15 ～ 0
    uint16_t base_low;      // 段基地址 15 ～ 0
    uint8_t  base_middle;   // 段基地址 23 ～ 16
    uint8_t  access;        // 段存在位、描述符特权级、描述符类型、描述符子类别
    uint8_t  granularity;   // 其他标志、段界限 19 ～ 16
    uint8_t  base_high;     // 段基地址 31 ～ 24
} __attribute__((packed)) gdt_struct;

// GDTR
typedef struct gdtr {
    uint16_t limit;     // 全局描述符表限长,2^16=65535字节，每个描述符8字节，所以一个gdt表最多可以放8192个段描述符
    uint32_t base;      // 全局描述符表 32 位基地址
} __attribute__((packed)) gdtr;

// 初始化全局描述符表
void init_gdt();

// GDT 加载到 GDTR 的函数[汇编实现]
extern void gdt_flush(uint32_t);

#endif  // INCLUDE_GDT_H_