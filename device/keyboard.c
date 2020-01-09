#include "keyboard.h"
#include "string.h"

int8_t NULL_CODE=233;  //假如是通码就无视

char recorded_command[100]; 

char* recorded_command_point=recorded_command;

extern uint8_t buffer_flag;

extern uint8_t continue_flag;

/* 键盘中断处理程序 */
void intr_keyboard_handler()
{ 
    if (continue_flag == 1)
    {
        continue_flag = 0;
    }
    memset(recorded_command_point, NULL, 100);
    char chars=get_char();
    if(chars != NULL_CODE && recorded_command_point<recorded_command+100 )
    {
         put_char(chars);
         *recorded_command_point = chars;
         recorded_command_point++;
         if (chars == '\n')
         {
             buffer_flag = 1;
             recorded_command_point = recorded_command;
             
         }

	}
    else if(chars != NULL_CODE)
    {
         put_str("buffer is full\n");
         return;
	}
  
}
/* 键盘初始化 */
void init_keyboard() 
{
   register_interrupt_handler(IRQ1, intr_keyboard_handler); //IRQ1是键盘的中断
   put_str("keyboard_init Done\n");
}

int8_t get_char()
{
    int break_code;
    uint16_t scancode = inb(KBD_BUF_PORT);
    break_code = ((scancode & 0x0080) != 0);   // 获取break_code

    if (break_code) 
    { 
        return NULL_CODE; 
    }
    /* 若为通码,只处理数组中定义的键以及alt_right和ctrl键,全是make_code */

    uint8_t index = (scancode &= 0x00ff);  // 将扫描码的高字节置0,主要是针对高字节是e0的扫描码.
    char cur_char = keymap[index][0];  // 在数组中找到对应的字符

    /* 只处理ascii码不为0的键 */
    if (cur_char) 
    {
        return cur_char;
    }
}