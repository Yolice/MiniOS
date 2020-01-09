#include "keyboard.h"
#include "string.h"

int8_t NULL_CODE=233;  //������ͨ�������

char recorded_command[100]; 

char* recorded_command_point=recorded_command;

extern uint8_t buffer_flag;

extern uint8_t continue_flag;

/* �����жϴ������ */
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
/* ���̳�ʼ�� */
void init_keyboard() 
{
   register_interrupt_handler(IRQ1, intr_keyboard_handler); //IRQ1�Ǽ��̵��ж�
   put_str("keyboard_init Done\n");
}

int8_t get_char()
{
    int break_code;
    uint16_t scancode = inb(KBD_BUF_PORT);
    break_code = ((scancode & 0x0080) != 0);   // ��ȡbreak_code

    if (break_code) 
    { 
        return NULL_CODE; 
    }
    /* ��Ϊͨ��,ֻ���������ж���ļ��Լ�alt_right��ctrl��,ȫ��make_code */

    uint8_t index = (scancode &= 0x00ff);  // ��ɨ����ĸ��ֽ���0,��Ҫ����Ը��ֽ���e0��ɨ����.
    char cur_char = keymap[index][0];  // ���������ҵ���Ӧ���ַ�

    /* ֻ����ascii�벻Ϊ0�ļ� */
    if (cur_char) 
    {
        return cur_char;
    }
}