#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H_
#include "types.h"  //������������


void outb(uint16_t port, uint8_t value); //���˿�д��8λ��С��ֵ
uint8_t inb(uint16_t port); //��ȡ8λ��С�ڵĶ˿�ֵ
uint16_t inw(uint16_t port); //��ȡ16λ��С�ڵĶ˿�ֵ


#endif // !INCLUDE_IO_H_
