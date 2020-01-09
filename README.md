# MiniOS （图一乐OS）
此操作系统内核基于hurlex的教程制作：http://wiki.0xffffff.org/

工具：
gcc/nasm/qemu+gdb/makefile



开发环境：
Ubuntu 16.04 32bit.



-----------------------------------

关于实现：

利用Grub完成bootloader进入保护模式

VGA的字符显示编程

GDT表的初始化

IDT表的初始化

对键盘的初始化

物理内存页分配(不开启段页式管理，也没有虚拟内存用户态等等)

kmalloc和kfreee对一段内存的申请和释放(尽管很多bug)

多任务是基于timer硬件的线程调度(只测试了主线程和create的新线程调度，不考虑死锁或者线程退出后的方案)





---------------------------------

非常雑魚的OS，也就图一乐。



