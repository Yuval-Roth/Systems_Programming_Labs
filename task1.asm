%define arg(n) dword [ebp+4*(2+n)]

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
    mov esi, 1          ; index in loop
    mov edi, [ebp + 2*4]    ; argc
    mov ecx , arg(1)
startloop:
    cmp esi,edi             ; jump if greater than argc
    jg endloop
    push 0                  ; leave space for return value
    pushad
    push dword [ecx]          ; argument to strlen
    call strlen
    add esp, 4              ; clean up stack
    mov dword [ebp-4] , eax ; store strlen return value
    push eax                ; 4th arg to system_call
    push dword[ecx]                ; 3rd arg to system_call
    push 1                  ; 2nd arg to system_call
    push 4                  ; 1st arg to system_call
    call system_call   ; print string
    add esp, 4*4        ; clean up stack
    popad
    add ecx, 4       ; move to next string
    add esi, 1         ; increment index
    jmp startloop
endloop:
    ret




