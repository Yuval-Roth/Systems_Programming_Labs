%define arg(n) dword [ebp+4*(2+n)]
%define clean_stack(n) add esp, 4*n

section .data
    space db 0x20,0x0
    newline db 0x0A,0x0

section .text
global _start
global system_call
extern strlen
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
    ret                     ; Back to caller

main:
    push ebp
    mov ebp, esp
    mov esi, 1                  ; index in loop
    mov edi, arg(0)             ; argc
    mov ecx, arg(1)             ; ecx = argv[0]
    add ecx, 4                  ; skip program name
startloop:
    cmp esi,edi                 ; jump if greater than argc
    jge endloop
    pushad

    ; get length of string
    push dword [ecx]            ; argument to strlen
    call strlen
    clean_stack(1)

    ; print string (eax = strlen, *ecx = string)
    push eax                    ; strlen
    push dword [ecx]            ; str
    call print
    clean_stack(2)

    popad
    add ecx, 4                  ; move to next string
    add esi, 1                  ; increment index
if:
    cmp esi, edi                ; if index < argc print space
    jge end_if
then:
    pushad
    call print_space
    popad
end_if:
    jmp startloop
endloop:
    cmp edi, 1                  ; if argc > 1 print newline
    jle no_newline
    pushad
    call print_newline
    popad
no_newline:
    mov esp, ebp
    pop ebp
    ret

print:
    push ebp
    mov ebp, esp
    push arg(1)
    push arg(0)
    push 1
    push 4
    call system_call
    mov esp, ebp
    pop ebp
    ret

print_space:
    push ebp
    mov ebp, esp
    push 1
    push space
    call print
    mov esp, ebp
    pop ebp
    ret

print_newline:
    push ebp
    mov ebp, esp
    push 1
    push newline
    call print
    mov esp, ebp
    pop ebp
    ret
