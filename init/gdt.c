#include "gdt.h"
#include "string.h"

#define GDT_LENGTH 5 //�ȳ�ʼ��5����������


gdt_struct global_describe_table[GDT_LENGTH];  //gdt����һ�����飬ÿ�������Ԫ�ض���һ���������Ľṹ��
gdtr gdt_register; //���ڼ���gdtr�Ĵ�����48λ�ṹ��


//ע����������gdt�����ͨ�ú���
static void gdt_initialize(int32_t indexs,uint32_t base,uint32_t limit,uint8_t access_right,uint8_t gran); //��һ������������ƫ��ֵ�������ĸ��Ǹ���GDT������Ķλ�ַ���ν��ޣ�������Ȩ�����ȵȵȡ�

extern uint32_t stack;

void init_gdt()
{
    //�ȳ�ʼ��gdtr�ṹ�壬һ������load gdt��
    gdt_register.limit=sizeof(gdt_struct)*GDT_LENGTH-1;  //������������Ĵ�Сд��gdtr�ṹ��(��������������С)*(������������)=�������������(��С)
    gdt_register.base=(uint32_t)&global_describe_table; //global_describe_table��һ�����������ṹ���������飬��������ַ��c���Է��䣬������gdt������ȡ��ַ�����õ�gdt����ڴ��ַ

    //�ٳ�ʼ������������ÿ����������
    gdt_initialize(0,0,0,0,0); //intel�涨�ڵ�0����ʼ�����Ǹ��յĶ�������
    gdt_initialize(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);     // ָ���������
    gdt_initialize(2, 0, 0xFFFFFFFF, 0x92, 0xCF);     // ���ݶ�������
    gdt_initialize(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);     // �û�ģʽ�����������
    gdt_initialize(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);     // �û�ģʽ���ݶ�������

    //����gdt��gdtr�Ĵ���
    gdt_flush((uint32_t)&gdt_register);  //���ô˽ṹ����Ϣ��ʼ��gdtr�Ĵ��� 
    put_str("gdt_initial Done\n");
}


static void gdt_initialize(int32_t indexs,uint32_t base,uint32_t limit,uint8_t access_right,uint8_t gran)  //ΨһҪע�����granularity���������ܶȣ�avl��4λ������Ҳ��4λ�Ķν���ֵ����
{
	global_describe_table[indexs].base_low     = (base & 0xFFFF);   //�λ�ַ�ĵ�16λ�߼��룬����λȫ������
    global_describe_table[indexs].base_middle  = (base >> 16) & 0xFF; //base����ƫ�ưѵ�16ȥ�������߼���8λ���õ��λ�ַ���м�8λ
    global_describe_table[indexs].base_high    = (base >> 24) & 0xFF; //base����ƫ�ưѵ�24λȥ�������߼���8λ���õ��λ�ַ�ĸ�8λ�����������32λ�λ�ַ�ĳ�ʼ��
    global_describe_table[indexs].limit_low    = (limit & 0xFFFF);  //�ν��޵�16λ����������λȫ������
    global_describe_table[indexs].granularity  = (limit >> 16) & 0x0F; //gran�������4λ�����Ƕν���ʣ��4λ�����ڶν��ްѵ�16λ����ƫ��ȥ����ʣ��4λ������granularity�ĵ�4λ��
    global_describe_table[indexs].granularity |= gran & 0xF0; //ֻȡgran������ĸ�4λ����Ϊ��4λ��granularityҪ�õ�ֵ����4λ�Ǹոո��ν����õ�
    global_describe_table[indexs].access       = access_right; //����8λ��(p,dpl,tpye���ֶ�)�����access�Σ�����gdt��ṹ������������
}
