#include "print.h"


//在80x25的显示窗口里，y代表25行，80代表每行可以输出80个字符，一共可以输出2000个字符。因为一个字符占用2个字节，前1个字节是ASCII码，后1个字节是码的属性，所以一共输出字符占用4000字节
static uint8_t* video_memory = (uint8_t*)0xB8000;  //char* 类型，所以指向该显存的每一个字节，所以对一个字符输出要操作两个字节。总共有40000个字节
static uint8_t color = (0 << 4) | (15 & 0x0F); //这是字符的显示属性，默认白字显示，也不闪烁

/*打印逻辑是，先获取当前坐标，在当前坐标位置打印一个字符，然后把光标设置到下一个坐标*/

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
	for (int i = 0; i < 4000; i = i + 2)  //遍历4000个字节，全部用空格代替
	{
		video_memory[i] = 0x20;
		video_memory[i + 1] = 0x07;
	}
}

uint16_t get_cursor()  //利用寄存器读取得到当前光标值，然后乘以2，返回光标的偏移值
{
	outb(0x3d4, 0x0e); //0x3d4端口写入0x0e表示要取走光标的高8位
	uint8_t high_bytes = inb(0x3d5); //取高8位，返回由high_bytes保存
	outb(0x3d4, 0x0f); //0x3d4端口写入0x0f表示取走光标的低8位
	uint8_t low_bytes = inb(0x3d5); //取低8位，返回由low_bytes保存
	uint16_t cursor = (high_bytes << 8) | low_bytes;
	cursor = cursor << 1;  //1个光标值占2个字节，所以现在是光标值，左移一位代表乘2，乘完后得到的是光标对应的内存偏移值
	return cursor;
}

void set_cursor(uint16_t cursor_index)
{
	outb(0x3d4, 0x0e); //告诉寄存器要设置高8位
	outb(0x3d5, cursor_index >> 8);
	outb(0x3d4, 0x0f); //告诉寄存器要设置低8位
	outb(0x3d5, cursor_index);
}

void put_char(uint8_t char_)  //第一步获取光标，第二布在光标上打印值，第三步更新下一个光标
{
	uint16_t current_cursor = get_cursor();
	uint8_t* video_current = video_memory + current_cursor;  //获取光标在显存下的偏移
	if (char_ == '\n') 
	{ 
		current_cursor = (current_cursor / 160) * 160 + 160; //先把光标移动到本行开头[/160*160]，开头向下移动一格[+160]
		if (current_cursor>=160*25)  //如果\n时候要判断下行是不是超过2000字符
		{
		     scroll_screen(current_cursor);
			 return;
		}
		current_cursor = current_cursor >> 1; //在设置光标前先把偏移地址转换成光标的逻辑地址
		set_cursor(current_cursor); 
	}
    else if(char_ == '\b') //如果是倒退，就把光标往前移动一位，然后打印一个空格
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
	    if (current_cursor>=160*25)    //如果只是普通的字符只要判断当前字符会不会超过2000字符
		{
		     scroll_screen(current_cursor);
			 return;
		}
		*video_current++ = char_; //在光标处的字符
		*video_current++ = color; //打印字符的属性
		current_cursor = video_current - video_memory;  //得到光标的偏移地址
		current_cursor = current_cursor >> 1; //转换成逻辑地址
		set_cursor(current_cursor); 
	}
}

void put_str(uint8_t* str)
{
	while (*str)   //字符串是连续被放在内存中，如果该内存有字符，则继续循环
	{
		put_char(*str++);
	}
}

void put_int(uint32_t value)  //最大范围unsigned int，超过会出错
{
    unsigned char i,j=0;
    unsigned char backstr[11];  //int最多分解10位数
    unsigned char str[32];
	while(value)
	{
		backstr[i]=(value%10)+'0';  //分解最后一位，加'0'是吧int类型的字符转换成此字符的char类型
		value=value/10; //分解倒数第二位，以此类推
		i++; //直到分解所有位
	}
	while(i) //因为刚刚是倒叙分解，此时倒叙添加数字
	{
	    str[j++]=backstr[--i];  //用--i是因为此时i指向了空的数字，i前一个才是value的最后一位数字
	}
	str[j]='\0'; //c风格字符串的结尾
	put_str(str);  //每个被分解的数字都是char类型
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
	    str[j++]=result[--i];  //因为十六进制是正序添加，这时应该倒叙写回
	}
	str[j]='\0';
	put_str("0x0");
	put_str(str);
}