#include "types.h"

uint16_t get_cursor();//获取当前光标的位置，利用对0x3D4,0x3D5寄存器的读获取
void set_cursor(uint16_t cursor_index); //输入的值就是位于2000字符的偏移值
void clear_console(); //对当前显示框清屏，就是把当前显示框全部用空格占满
void put_char(uint8_t char_); //在当前光标这打印一个字符
void put_str(uint8_t* str); //从str指向地址开始一直输出，直到'\0'为止结束
void put_int(uint32_t value); //以字符串形式在当前光标这输出一个十进制整数
void put_hex(uint32_t value); //打印十六进制的数组
void scroll_screen(uint16_t current_cursor); //如果超过2000字符就发生滚屏
void clear(); //清除屏幕