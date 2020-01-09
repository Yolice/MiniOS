#include "io.h"

inline void outb(uint16_t port, uint8_t value)
{
	asm volatile("outb %1,%0" : : "dN"(port), "a"(value));  //%1 %0ָ�ڼ���������������%1��ֵд��%0�Ķ˿ڣ�����port�ǵ�һ��ֵ����Ӧ%0��value��Ӧ%1
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;       //�Ӵ�СΪshort�Ķ˿ڶ�ȡһ��char��С������
	asm volatile ("inb %1,%0":"=a"(ret) : "dN"(port));  //��ȡport�Ŷ˿����ݣ���ֵ���ص�ret��
	return ret;
}

inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile ("inw %1,%0":"=a"(ret) : "dN"(port));
	return ret;
}