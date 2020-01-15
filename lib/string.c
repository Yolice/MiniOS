#include "string.h"

void memset(void* dst_, uint8_t value, uint32_t size) {
   uint8_t* dst = (uint8_t*)dst_;
   while (size-- > 0)
      *dst++ = value;
}

/* ��src_��ʼ��size���ֽڸ��Ƶ�dst_ */
void memcpy(void* dst_, const void* src_, uint32_t size) {
   uint8_t* dst = dst_;
   const uint8_t* src = src_;
   while (size-- > 0)
      *dst++ = *src++;
}

/* �����Ƚ��Ե�ַa_�͵�ַb_��ͷ��size���ֽ�,������򷵻�0,��a_����b_����+1,���򷵻�-1 */
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

/* ���ַ�����src_���Ƶ�dst_ */
char* strcpy(char* dst_, const char* src_) {
   char* r = dst_;		       // ��������Ŀ���ַ�����ʼ��ַ
   while((*dst_++ = *src_++));
   return r;
}

/* �����ַ������� */
uint32_t strlen(const char* str) {
   const char* p = str;
   while(*p++);
   return (p - str - 1);
}

/* �Ƚ������ַ���,���Ϊ1������Ϊ0. */
int8_t strcmp (char *a, char *b) 
{
    char flags = 0;
    put_char(*b);
}

/* �����Ҳ����ַ���str���״γ����ַ�ch�ĵ�ַ(�����±�,�ǵ�ַ) */
char* strchr(const char* str, const uint8_t ch) {
   while (*str != 0) {
      if (*str == ch) {
	 return (char*)str;	    // ��Ҫǿ��ת���ɺͷ���ֵ����һ��,����������ᱨconst���Զ�ʧ,��ͬ.
      }
      str++;
   }
   return NULL;
}

/* �Ӻ���ǰ�����ַ���str���״γ����ַ�ch�ĵ�ַ(�����±�,�ǵ�ַ) */
char* strrchr(const char* str, const uint8_t ch) {
   const char* last_char = NULL;
   /* ��ͷ��β����һ��,������ch�ַ�,last_char���Ǹ��ַ����һ�γ����ڴ��еĵ�ַ(�����±�,�ǵ�ַ)*/
   while (*str != 0) {
      if (*str == ch) {
	 last_char = str;
      }
      str++;
   }
   return (char*)last_char;
}

/* ���ַ���src_ƴ�ӵ�dst_��,����ƴ�ӵĴ���ַ */
char* strcat(char* dst_, const char* src_) {
   char* str = dst_;
   while (*str++);
   --str;      // �𿴴��ˣ�--str�Ƕ�����һ�䣬������while��ѭ����
   while((*str++ = *src_++));	 // ��*str����ֵΪ0ʱ,��ʱ���ʽ������,����������ַ�����β��0.
   return dst_;
}

/* ���ַ���str�в���ָ���ַ�ch���ֵĴ��� */
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


