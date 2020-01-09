#ifndef INCLUDE_INIT_H
#define INCLUDE_INIT_H_
#include "types.h"  //导入数据类型

#include "idt.h"
#include "gdt.h"
#include "timer.h"
#include "page.h"
#include "keyboard.h"
#include "heap.h"
#include "thread.h"
#include "shell.h"

void init_all();

#endif // !INCLUDE_INIT_H_
