#include "types.h"

uint16_t get_cursor();//��ȡ��ǰ����λ�ã����ö�0x3D4,0x3D5�Ĵ����Ķ���ȡ
void set_cursor(uint16_t cursor_index); //�����ֵ����λ��2000�ַ���ƫ��ֵ
void clear_console(); //�Ե�ǰ��ʾ�����������ǰѵ�ǰ��ʾ��ȫ���ÿո�ռ��
void put_char(uint8_t char_); //�ڵ�ǰ������ӡһ���ַ�
void put_str(uint8_t* str); //��strָ���ַ��ʼһֱ�����ֱ��'\0'Ϊֹ����
void put_int(uint32_t value); //���ַ�����ʽ�ڵ�ǰ��������һ��ʮ��������
void put_hex(uint32_t value); //��ӡʮ�����Ƶ�����
void scroll_screen(uint16_t current_cursor); //�������2000�ַ��ͷ�������
void clear(); //�����Ļ