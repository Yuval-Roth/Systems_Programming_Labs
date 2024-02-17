%define arg(n) dword [ebp+4*(2+n)]
%define clean_stack(n) add esp, 4*n
%define sys_write 4
%define sys_read 3
%define stdin 0
%define stdout 1
%define stderr 2

section .globals
    global filein
    global fileout

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
    call    encode

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

encode:
    push ebp
    mov ebp, esp
    sub esp, 4 + 256  ; to access this buffer,

    ; call readline
    push 256
    push ebp-4
    call readline
    clean_stack(2)

    ; increment each character (if A to z) by 1
    mov edi, ebp-4      ; edi = buffer
    L_while2:
    cmp byte [edi], 0
    je L_while2_end
    cmp byte [edi], "A"
    jl L_while2_next
    cmp byte [edi], "Z"
    jg L_while2_next

    ; if Z, set to A
    cmp byte [edi], "Z"
    je yes_Z
    mov byte [edi], "A"
    jmp L_while2_next
    ; not Z, increment
    add byte [edi], 1
    L_while2_next:
    add edi, 1
    jmp L_while2
    L_while2_end:
    pushad
    push 256
    push ebp-4
    call print_to_fileout
    clean_stack(2)
    call print_newline_to_fileout
    popad
    mov esp, ebp
    pop ebp
    ret

main:
    push ebp
    mov ebp, esp
    ; set default values for filein and fileout
    mov dword [filein], stdin
    mov dword [fileout], stdout

    mov esi, 1                  ; index in loop
    mov edi, arg(0)             ; argc
    mov ecx, arg(1)             ; ecx = argv[0]
    add ecx, 4                  ; skip program name
    L_while1:
    cmp esi,edi                 ; jump if greater than argc
    jge L_while1_end
    pushad

    ; get length of string
    push dword [ecx]            ; argument to strlen
    call strlen
    clean_stack(1)

    ; print string (eax = strlen, *ecx = string)
    push eax                    ; strlen
    push dword [ecx]            ; str
    call print_err
    clean_stack(2)
    call print_newline_err

    popad
    add ecx, 4                  ; move to next string
    add esi, 1                  ; increment index
    jmp L_while1
    L_while1_end:
    mov esp, ebp
    pop ebp
    ret

print_err:
    push ebp
    mov ebp, esp
    push arg(1)
    push arg(0)
    push stderr
    push sys_write
    call system_call
    mov esp, ebp
    pop ebp
    ret

print_to_fileout:
    push ebp
    mov ebp, esp
    push arg(1)
    push arg(0)
    push fileout
    push sys_write
    call system_call
    mov esp, ebp
    pop ebp
    ret

print_newline_err:
    push ebp
    mov ebp, esp
    push 1
    push newline
    call print_err
    mov esp, ebp
    pop ebp
    ret

print_newline_to_fileout:
    push ebp
    mov ebp, esp
    push 1
    push newline
    call print
    mov esp, ebp
    pop ebp
    ret

readline:
    push ebp
    mov ebp, esp
    push arg(1)
    push arg(0)
    push filein
    push sys_read
    call system_call
    mov esp, ebp
    pop ebp
    ret