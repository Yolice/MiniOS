#include "string.h"

void memset(void* dst_, uint8_t value, uint32_t size) {
   uint8_t* dst = (uint8_t*)dst_;
   while (size-- > 0)
      *dst++ = value;
}

/* 将src_起始的size个字节复制到dst_ */
void memcpy(void* dst_, const void* src_, uint32_t size) {
   uint8_t* dst = dst_;
   const uint8_t* src = src_;
   while (size-- > 0)
      *dst++ = *src++;
}

/* 连续比较以地址a_和地址b_开头的size个字节,若相等则返回0,若a_大于b_返回+1,否则返回-1 */
int memcmp(const void* a_, const void* b_, uint32_t size) {
   const char* a = a_;
   const char* b = b_;
   while (size-- > 0) 
   {
      if(*a != *b) 
      {
	      return *a > *b ? 1 : -1; 
      }
      a++;
      b++;
   }
   return 0;
}

/* 将字符串从src_复制到dst_ */
char* strcpy(char* dst_, const char* src_) {
   char* r = dst_;		       // 用来返回目的字符串起始地址
   while((*dst_++ = *src_++));
   return r;
}

/* 返回字符串长度 */
uint32_t strlen(const char* str) {
   const char* p = str;
   while(*p++);
   return (p - str - 1);
}

/* 比较两个字符串,相等为1，不等为0. */
int8_t strcmp (char *a, char *b) 
{
    char flags = 0;
    put_char(*b);
}

/* 从左到右查找字符串str中首次出现字符ch的地址(不是下标,是地址) */
char* strchr(const char* str, const uint8_t ch) {
   while (*str != 0) {
      if (*str == ch) {
	 return (char*)str;	    // 需要强制转化成和返回值类型一样,否则编译器会报const属性丢失,下同.
      }
      str++;
   }
   return NULL;
}

/* 从后往前查找字符串str中首次出现字符ch的地址(不是下标,是地址) */
char* strrchr(const char* str, const uint8_t ch) {
   const char* last_char = NULL;
   /* 从头到尾遍历一次,若存在ch字符,last_char总是该字符最后一次出现在串中的地址(不是下标,是地址)*/
   while (*str != 0) {
      if (*str == ch) {
	 last_char = str;
      }
      str++;
   }
   return (char*)last_char;
}

/* 将字符串src_拼接到dst_后,将回拼接的串地址 */
char* strcat(char* dst_, const char* src_) {
   char* str = dst_;
   while (*str++);
   --str;      // 别看错了，--str是独立的一句，并不是while的循环体
   while((*str++ = *src_++));	 // 当*str被赋值为0时,此时表达式不成立,正好添加了字符串结尾的0.
   return dst_;
}

/* 在字符串str中查找指定字符ch出现的次数 */
uint32_t strchrs(const char* str, uint8_t ch) {
   uint32_t ch_cnt = 0;
   const char* p = str;
   while(*p != 0) {
      if (*p == ch) {
	 ch_cnt++;
      }
      p++;
   }
   return ch_cnt;
}


