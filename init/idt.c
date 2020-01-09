#include "print.h"
#include "idt.h"
#include "io.h"

#define INTERRUPT_NUM 256

idt_entry_t idt_entries[INTERRUPT_NUM]; // 中断描述符表

idt_ptr_t idt_ptr; // IDTR
 
interrupt_handler_t interrupt_handlers[INTERRUPT_NUM]; // 中断处理函数的指针数组

static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags); // 设置中断描述符

extern void idt_flush(uint32_t); // 声明加载 IDTR 的函数




// 初始化中断描述符表
void init_idt()
{	
    // 重新映射 IRQ 表
	// 两片级联的 Intel 8259A 芯片
	// 主片端口 0x20 0x21
	// 从片端口 0xA0 0xA1
	
	// 初始化主片、从片
	// 0001 0001
	outb(0x20, 0x11);
	outb(0xA0, 0x11);

	// 设置主片 IRQ 从 0x20(32) 号中断开始
	outb(0x21, 0x20);

	// 设置从片 IRQ 从 0x28(40) 号中断开始
	outb(0xA1, 0x28);
	
	// 设置主片 IR2 引脚连接从片
	outb(0x21, 0x04);

	// 告诉从片输出引脚和主片 IR2 号相连
	outb(0xA1, 0x02);
	
	// 设置主片和从片按照 8086 的方式工作
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	
	// 设置主从片允许中断
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
    
	//初始化内存区域
	memset(idt_entries,0,INTERRUPT_NUM);
	memset(interrupt_handlers,0,INTERRUPT_NUM);

	idt_ptr.limit = sizeof(idt_entry_t) * INTERRUPT_NUM - 1;  //idt表大小=中断描述符大小*中断描述符个数
	idt_ptr.base  = (uint32_t)&idt_entries; //取idt表数组地址为idt表在内存中的地址
	

	// idt_set_gate第二个参数是偏移地址，这里以函数的地址为参数，就能在函数所储存的段里通过偏移地址找到对应的isr(irq)
	//ISR的处理过程是 (isr0 - isr31) -> isr_common_stub -> isr_handler -> 具体的ISR处理函数。
    //IRQ的处理过程是 (irq0 - irq15) -> irq_common_stub -> irq_hanlder -> 具体的IRQ处理函数。
	idt_set_gate( 0, (uint32_t)isr0,  0x08, 0x8E);
	idt_set_gate( 1, (uint32_t)isr1,  0x08, 0x8E);
	idt_set_gate( 2, (uint32_t)isr2,  0x08, 0x8E);
	idt_set_gate( 3, (uint32_t)isr3,  0x08, 0x8E);
	idt_set_gate( 4, (uint32_t)isr4,  0x08, 0x8E);
	idt_set_gate( 5, (uint32_t)isr5,  0x08, 0x8E);
	idt_set_gate( 6, (uint32_t)isr6,  0x08, 0x8E);
	idt_set_gate( 7, (uint32_t)isr7,  0x08, 0x8E);
	idt_set_gate( 8, (uint32_t)isr8,  0x08, 0x8E);
	idt_set_gate( 9, (uint32_t)isr9,  0x08, 0x8E);
	idt_set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
	idt_set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
	idt_set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
	idt_set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
	idt_set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
	idt_set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
	idt_set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
	idt_set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
	idt_set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
	idt_set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
	idt_set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
	idt_set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
	idt_set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
	idt_set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
	idt_set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
	idt_set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
	idt_set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
	idt_set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
	idt_set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
	idt_set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
	idt_set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
	idt_set_gate(31, (uint32_t)isr31, 0x08, 0x8E);

	idt_set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	idt_set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	idt_set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	idt_set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	idt_set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	idt_set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	idt_set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	idt_set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	idt_set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	idt_set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	idt_set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt_set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt_set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt_set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt_set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt_set_gate(47, (uint32_t)irq15, 0x08, 0x8E);
	// 255 将来用于实现系统调用
	idt_set_gate(255, (uint32_t)isr255, 0x08, 0x8E);

	// 更新设置中断描述符表
	idt_flush((uint32_t)&idt_ptr);
	put_str("idt_initial Done\n");
}

// 设置中断描述符
static void idt_set_gate(uint8_t num, uint32_t base, uint16_t selector, uint8_t flags)
{
	idt_entries[num].base_lo = base & 0xFFFF; //传进来32位的偏移地址只取低16位
	idt_entries[num].base_hi = (base >> 16) & 0xFFFF; //去掉低16后再取16位作为高16位的偏移地址
	idt_entries[num].selector= selector;  //传入段选择子
	idt_entries[num].always0 = 0; //默认为0即可
	idt_entries[num].flags = flags;  //传入type dpl等字段
}

// 调用CPU内部的中断处理函数
void isr_handler(pt_regs *regs)
{
	if (interrupt_handlers[regs->int_no])  
	{  
	     interrupt_handlers[regs->int_no](regs); //如果中断函数指针指向的函数存在，那么就利用函数指针来调用被指向的函数
	}
	else 
	{
         put_str("!Interrput number: ");
	     put_int(regs->int_no);
		 put_str("\n");

	}
}

// 调用用户外部的中断处理函数
void irq_handler(pt_regs *regs)
{
	// 发送中断结束信号给 PICs
	// 按照我们的设置，从 32 号中断起为用户自定义中断
	// 因为单片的 Intel 8259A 芯片只能处理 8 级中断
	// 故大于等于 40 的中断号是由从片处理的
	if (regs->int_no >= 40) {
		// 发送重设信号给从片
		outb(0xA0, 0x20);
	}
	// 发送重设信号给主片
	outb(0x20, 0x20);

	if (interrupt_handlers[regs->int_no])  //如果中断函数指针指向的函数存在，那么就利用函数指针来调用被指向的函数
	{
		interrupt_handlers[regs->int_no](regs);
	}
}


// 注册一个中断处理函数
void register_interrupt_handler(uint8_t num, interrupt_handler_t function)
{
	interrupt_handlers[num] = function; //把函数的地址赋给中断函数指针数组
}

void enable_interrupt()
{
	asm volatile ("sti");
}

void disable_interrupt()
{
	asm volatile ("cli");
}