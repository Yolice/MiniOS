#include "gdt.h"
#include "string.h"

#define GDT_LENGTH 5 //先初始化5个段描述符


gdt_struct global_describe_table[GDT_LENGTH];  //gdt表是一个数组，每个数组的元素都是一个描述符的结构体
gdtr gdt_register; //用于加载gdtr寄存器的48位结构体


//注册描述符在gdt表里的通用函数
static void gdt_initialize(int32_t indexs,uint32_t base,uint32_t limit,uint8_t access_right,uint8_t gran); //第一个是描述符的偏移值，其他四个是根据GDT表定义里的段基址，段界限，访问特权，粒度等等。

extern uint32_t stack;

void init_gdt()
{
    //先初始化gdtr结构体，一会用于load gdt用
    gdt_register.limit=sizeof(gdt_struct)*GDT_LENGTH-1;  //讲段描述符表的大小写入gdtr结构，(单个段描述符大小)*(段描述符个数)=段描述符表界限(大小)
    gdt_register.base=(uint32_t)&global_describe_table; //global_describe_table是一个段描述符结构体类型数组，这个数组地址又c语言分配，他就是gdt表，对其取地址操作得到gdt表的内存地址

    //再初始化段描述表里每个段描述符
    gdt_initialize(0,0,0,0,0); //intel规定在第0个开始必须是个空的段描述符
    gdt_initialize(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);     // 指令段描述符
    gdt_initialize(2, 0, 0xFFFFFFFF, 0x92, 0xCF);     // 数据段描述符
    gdt_initialize(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);     // 用户模式代码段描述符
    gdt_initialize(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);     // 用户模式数据段描述符

    //加载gdt表到gdtr寄存器
    gdt_flush((uint32_t)&gdt_register);  //利用此结构体信息初始化gdtr寄存器 
    put_str("gdt_initial Done\n");
}


static void gdt_initialize(int32_t indexs,uint32_t base,uint32_t limit,uint8_t access_right,uint8_t gran)  //唯一要注意的是granularity参数既有密度，avl等4位参数，也有4位的段界限值储存
{
    global_describe_table[indexs].base_low     = (base & 0xFFFF);   //段基址的低16位逻辑与，其他位全部清零
    global_describe_table[indexs].base_middle  = (base >> 16) & 0xFF; //base先右偏移把低16去掉，再逻辑与8位，得到段基址的中间8位
    global_describe_table[indexs].base_high    = (base >> 24) & 0xFF; //base先右偏移把低24位去掉，再逻辑与8位，得到段基址的高8位，至此完成了32位段基址的初始化
    global_describe_table[indexs].limit_low    = (limit & 0xFFFF);  //段界限低16位保留，其他位全部清零
    global_describe_table[indexs].granularity  = (limit >> 16) & 0x0F; //gran参数里低4位储存是段界限剩下4位，现在段界限把低16位给右偏移去掉，剩下4位储存再granularity的低4位里
    global_describe_table[indexs].granularity |= gran & 0xF0; //只取gran参数里的高4位，因为高4位是granularity要用的值，低4位是刚刚给段界限用的
    global_describe_table[indexs].access       = access_right; //最后把8位的(p,dpl,tpye等字段)赋予给access段，至此gdt表结构赋予规则构造完毕
}
