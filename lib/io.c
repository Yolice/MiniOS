#include "io.h"

inline void outb(uint16_t port, uint8_t value)
{
	asm volatile("outb %1,%0" : : "dN"(port), "a"(value));  //%1 %0指第几个操作数，这里%1的值写入%0的端口，所以port是第一个值，对应%0，value对应%1
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;       //从大小为short的端口读取一个char大小的数据
	asm volatile ("inb %1,%0":"=a"(ret) : "dN"(port));  //读取port号端口数据，把值返回到ret里
	return ret;
}

inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %1,%0":"=a"(ret) : "dN"(port));
	return ret;
}