#include "print.h"


//��80x25����ʾ�����y����25�У�80����ÿ�п������80���ַ���һ���������2000���ַ�����Ϊһ���ַ�ռ��2���ֽڣ�ǰ1���ֽ���ASCII�룬��1���ֽ���������ԣ�����һ������ַ�ռ��4000�ֽ�
static uint8_t* video_memory = (uint8_t*)0xB8000;  //char* ���ͣ�����ָ����Դ��ÿһ���ֽڣ����Զ�һ���ַ����Ҫ���������ֽڡ��ܹ���40000���ֽ�
static uint8_t color = (0 << 4) | (15 & 0x0F); //�����ַ�����ʾ���ԣ�Ĭ�ϰ�����ʾ��Ҳ����˸

/*��ӡ�߼��ǣ��Ȼ�ȡ��ǰ���꣬�ڵ�ǰ����λ�ô�ӡһ���ַ���Ȼ��ѹ�����õ���һ������*/

void scroll_screen(uint16_t current_cursor)
{
	for (int i = 0 * 160; i < 24 * 160; i++) 
	{
		video_memory[i] = video_memory[i + 160];
	}
	for (int i = 24 * 160; i < 25 * 160; i=i+2)
	{
		video_memory[i] = 0x20;
		video_memory[i + 1] = 0x07;
	}
	current_cursor = current_cursor - 160;
	current_cursor = current_cursor >> 1; 
	set_cursor(current_cursor);
}

void clear()
{
	set_cursor(0);
	for (int i = 0; i < 4000; i = i + 2)  //����4000���ֽڣ�ȫ���ÿո����
	{
		video_memory[i] = 0x20;
		video_memory[i + 1] = 0x07;
	}
}

uint16_t get_cursor()  //���üĴ�����ȡ�õ���ǰ���ֵ��Ȼ�����2�����ع���ƫ��ֵ
{
	outb(0x3d4, 0x0e); //0x3d4�˿�д��0x0e��ʾҪȡ�߹��ĸ�8λ
	uint8_t high_bytes = inb(0x3d5); //ȡ��8λ��������high_bytes����
	outb(0x3d4, 0x0f); //0x3d4�˿�д��0x0f��ʾȡ�߹��ĵ�8λ
	uint8_t low_bytes = inb(0x3d5); //ȡ��8λ��������low_bytes����
	uint16_t cursor = (high_bytes << 8) | low_bytes;
	cursor = cursor << 1;  //1�����ֵռ2���ֽڣ����������ǹ��ֵ������һλ�����2�������õ����ǹ���Ӧ���ڴ�ƫ��ֵ
	return cursor;
}

void set_cursor(uint16_t cursor_index)
{
	outb(0x3d4, 0x0e); //���߼Ĵ���Ҫ���ø�8λ
	outb(0x3d5, cursor_index >> 8);
	outb(0x3d4, 0x0f); //���߼Ĵ���Ҫ���õ�8λ
	outb(0x3d5, cursor_index);
}

void put_char(uint8_t char_)  //��һ����ȡ��꣬�ڶ����ڹ���ϴ�ӡֵ��������������һ�����
{
	uint16_t current_cursor = get_cursor();
	uint8_t* video_current = video_memory + current_cursor;  //��ȡ������Դ��µ�ƫ��
	if (char_ == '\n') 
	{ 
		current_cursor = (current_cursor / 160) * 160 + 160; //�Ȱѹ���ƶ������п�ͷ[/160*160]����ͷ�����ƶ�һ��[+160]
		if (current_cursor>=160*25)  //���\nʱ��Ҫ�ж������ǲ��ǳ���2000�ַ�
		{
		     scroll_screen(current_cursor);
			 return;
		}
		current_cursor = current_cursor >> 1; //�����ù��ǰ�Ȱ�ƫ�Ƶ�ַת���ɹ����߼���ַ
		set_cursor(current_cursor); 
	}
    else if(char_ == '\b') //����ǵ��ˣ��Ͱѹ����ǰ�ƶ�һλ��Ȼ���ӡһ���ո�
	{
		if (*(video_current - 2) == ':')
		{
			
			return;
		}
	    current_cursor = current_cursor >> 1;
		current_cursor = current_cursor - 1;
		set_cursor(current_cursor);
		current_cursor = get_cursor();
		video_current = video_memory + current_cursor;
		*video_current++=' ';
		*video_current++ = color;
	}
	else
	{
	    if (current_cursor>=160*25)    //���ֻ����ͨ���ַ�ֻҪ�жϵ�ǰ�ַ��᲻�ᳬ��2000�ַ�
		{
		     scroll_screen(current_cursor);
			 return;
		}
		*video_current++ = char_; //�ڹ�괦���ַ�
		*video_current++ = color; //��ӡ�ַ�������
		current_cursor = video_current - video_memory;  //�õ�����ƫ�Ƶ�ַ
		current_cursor = current_cursor >> 1; //ת�����߼���ַ
		set_cursor(current_cursor); 
	}
}

void put_str(uint8_t* str)
{
	while (*str)   //�ַ����������������ڴ��У�������ڴ����ַ��������ѭ��
	{
		put_char(*str++);
	}
}

void put_int(uint32_t value)  //���Χunsigned int�����������
{
    unsigned char i,j=0;
    unsigned char backstr[11];  //int���ֽ�10λ��
    unsigned char str[32];
	while(value)
	{
		backstr[i]=(value%10)+'0';  //�ֽ����һλ����'0'�ǰ�int���͵��ַ�ת���ɴ��ַ���char����
		value=value/10; //�ֽ⵹���ڶ�λ���Դ�����
		i++; //ֱ���ֽ�����λ
	}
	while(i) //��Ϊ�ո��ǵ���ֽ⣬��ʱ�����������
	{
	    str[j++]=backstr[--i];  //��--i����Ϊ��ʱiָ���˿յ����֣�iǰһ������value�����һλ����
	}
	str[j]='\0'; //c����ַ����Ľ�β
	put_str(str);  //ÿ�����ֽ�����ֶ���char����
}



void put_hex(uint32_t value)
{
    char hex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	char result[32];
	char str[32];
	char i,j=0;
	int hex_index=0;
	while(value)
	{
		hex_index=value%16;
		result[i++]=hex[hex_index];
		value=value/16;
	}
	while(i)
	{
	    str[j++]=result[--i];  //��Ϊʮ��������������ӣ���ʱӦ�õ���д��
	}
	str[j]='\0';
	put_str("0x0");
	put_str(str);
}