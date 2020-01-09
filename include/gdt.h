#ifndef INCLUDE_GDT_H_
#define INCLUDE_GDT_H_
#include "types.h"

// ȫ������������
typedef struct gdt_struct { //����GDT�Ķ��壬��32λ�ֶν��޺͵Ͷλ�ַ��ɣ���32λ������4��8λ����ɡ�����GDT��ʱ�򣬰���GDT����ƴ�Ӽ���
    uint16_t limit_low;     // �ν���   15 �� 0
    uint16_t base_low;      // �λ���ַ 15 �� 0
    uint8_t  base_middle;   // �λ���ַ 23 �� 16
    uint8_t  access;        // �δ���λ����������Ȩ�������������͡������������
    uint8_t  granularity;   // ������־���ν��� 19 �� 16
    uint8_t  base_high;     // �λ���ַ 31 �� 24
} __attribute__((packed)) gdt_struct;

// GDTR
typedef struct gdtr {
    uint16_t limit;     // ȫ�����������޳�,2^16=65535�ֽڣ�ÿ��������8�ֽڣ�����һ��gdt�������Է�8192����������
    uint32_t base;      // ȫ���������� 32 λ����ַ
} __attribute__((packed)) gdtr;

// ��ʼ��ȫ����������
void init_gdt();

// GDT ���ص� GDTR �ĺ���[���ʵ��]
extern void gdt_flush(uint32_t);

#endif  // INCLUDE_GDT_H_