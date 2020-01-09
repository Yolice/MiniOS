# MiniOS
此操作系统内核基于hurlex的教程制作：http://wiki.0xffffff.org/

工具：
gcc/nasm/qemu+gdb/makefile

系统环境：
Ubuntu 16.04 32bit.

关于实现：

利用Grub完成bootloader进入保护模式
VGA的字符显示编程
GDT表的初始化
IDT表的初始化
物理内存页分配(不适用段页式管理，所以没有虚拟内存)
kmalloc对一段内存的分配(尽管很多bug)
多任务是基于timer硬件的线程调度(只测试了主线程和create的新线程调度，不考虑死锁或者线程退出后的方案)


非常雑魚的OS，也就图一乐。



