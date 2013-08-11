SECTION .text


UPDT_CODE_START:

execl_addr equ 0x804a46c
rename_addr equ 0x804a41c
arg_execFilePath equ 8
arg_execArguments equ 12
arg_n equ 16
arg_data equ 20

UPDT_SwapAndExec: ; arguments: execFilePath, execArguments, int n, {char *str1a, char *str1b,char *str2a, char *str2b, ... }
    push ebp
    mov ebp, esp

    push ebx
    push ecx
    push edx

    ;real code starts here

    mov ecx, [ebp + arg_n]
    mov ebx, [ebp + arg_data]
    mov edx, rename_addr
  updt_loopStart:
    mov eax, [ebx + 4]
    push eax
    
    mov eax, [ebx]
    push eax
    add ebx, 8
    
    call edx
    
    add esp, 8
    
    dec ecx
    jnz updt_loopStart
    
    ;all files are copied by now, all we need to do is calling execl
    
    
    ;mov ebx, [ebp + arg_execArguments]
    ;push ebx
    
    ;mov ebx, [ebp + arg_execFilePath]
    ;push ebx
    
    ;push all commandline arguments
    mov ebx, [ebp + arg_execArguments]
  updt_secondLoop:
    push ebx
    cmp ebx, 0
    
    je updt_secondLoopEnd
    add ebx, 4
    jmp updt_secondLoop
  updt_secondLoopEnd:
    mov eax, execl_addr
    call eax
    
    add esp, 8
    
    pop edx
    pop ecx
    pop ebx
    
    pop ebp
    
    ret
    nop
    
    
    
    
UPDT_CODE_END: