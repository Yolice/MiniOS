#include "keyboard.h"
#include "string.h"

int8_t NULL_CODE=233;  //假如是通码就无视

extern int END_CODE;  //定义此end_code为结尾

char recorded_command[100]; 

int recorded_input[100];

char* recorded_command_point=recorded_command;

int* recorded_input_point=recorded_input;

extern uint8_t buffer_flag;

extern uint8_t input_flag;

extern uint8_t integer_flag;

extern uint8_t continue_flag;

/* 键盘中断处理程序 */
void intr_keyboard_handler()
{ 
    if (continue_flag == 1)
    {
        continue_flag = 0;
    }
    memset(recorded_input_point, NULL, 100);
    memset(recorded_command_point, NULL, 100);
    if(integer_flag)
    {
         int integer = get_int();
         if(integer != NULL_CODE)
         {
               if(integer == 100 )
               {
                   input_flag = 0;
                   *recorded_input_point = END_CODE;
                   recorded_input_point = recorded_input;
		       }
               else if (integer == 120)
               {
                   put_char('\b');
               }
               else if(integer >=0 && integer <=9)
               {
                  put_int(integer);
                  *recorded_input_point = integer;
                  recorded_input_point++;
		       }
		 }
         return;
	}
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

int8_t get_int()
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
    int integer = keymap[index][1];  // 在数组中找到对应的字符
    
    return integer;
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