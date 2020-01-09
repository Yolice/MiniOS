#include "init.h"

void init_all()
{
    put_str("Initial Start\n");
    init_gdt();
    init_idt();
    init_timer();
    init_page();
    init_keyboard();
    init_heap();
    init_thread();
    put_str("Initial Done\n");
}
