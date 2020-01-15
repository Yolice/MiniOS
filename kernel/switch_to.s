[global switch_to]

; 具体的线程切换操作，重点在于寄存器的保存与恢复
switch_to:
        mov eax, [esp+4]   ;switch_to函数传入了每个线程的上下文结构体地址，esp+4指向了current的context结构体地址

        mov [eax+0],  esp   ;esp寄存器值存入context结构体第一个成员，即esp成员
        mov [eax+4],  ebp   ;同理保存ebp寄存器
        mov [eax+8],  ebx
        mov [eax+12], esi
        mov [eax+16], edi
        pushf
        pop ecx
        mov [eax+20], ecx   ;ecx值传给eflag成员

        mov eax, [esp+8]    ;esp+8是即将加载线程的context结构体地址

        mov esp, [eax+0]    ;context结构体里esp成员的值传入到esp寄存器中
        mov ebp, [eax+4]
        mov ebx, [eax+8]
        mov esi, [eax+12]
        mov edi, [eax+16]
        mov eax, [eax+20]
        push eax
        popf                ;上下文切换完成
 	
        ret
