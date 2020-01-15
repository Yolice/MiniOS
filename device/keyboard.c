#include "keyboard.h"
#include "string.h"

int8_t NULL_CODE=233;  //������ͨ�������

extern int END_CODE;  //�����end_codeΪ��β

char recorded_command[100]; 

int recorded_input[100];

char* recorded_command_point=recorded_command;

int* recorded_input_point=recorded_input;

extern uint8_t buffer_flag;

extern uint8_t input_flag;

extern uint8_t integer_flag;

extern uint8_t continue_flag;

/* �����жϴ������ */
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

/* ���̳�ʼ�� */
void init_keyboard() 
{
   register_interrupt_handler(IRQ1, intr_keyboard_handler); //IRQ1�Ǽ��̵��ж�
   put_str("keyboard_init Done\n");
}

int8_t get_int()
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
    int integer = keymap[index][1];  // ���������ҵ���Ӧ���ַ�
    
    return integer;
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