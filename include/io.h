#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H_
#include "types.h"  //导入数据类型


void outb(uint16_t port, uint8_t value); //往端口写入8位大小的值
uint8_t inb(uint16_t port); //读取8位大小内的端口值
uint16_t inw(uint16_t port); //读取16位大小内的端口值


#endif // !INCLUDE_IO_H_
