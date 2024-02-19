%define arg(n) dword [ebp+4*(2+n)]
%define clean_stack(n) add esp, 4*n
%define stmalloc(n) sub esp, n
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
    space db 0x20,0x0
    newline db 0x0A,0x0
    dash_i db "-i",0x0
    dash_i_fail db "Failed to open infile",0x0A,0x0
    dash_i_fail_len equ $-dash_i_fail - 1
    dash_o db "-o",0x0
    dash_o_fail db "Failed to open outfile",0x0A,0x0
    dash_o_fail_len equ $-dash_o_fail - 1

section .bss
    fileout resd 1
    filein resd 1

section .text
global _start
global system_call
extern strlen
extern strncmp
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

encode:
    push ebp
    mov ebp, esp
    stmalloc(4)         ; input size
    stmalloc(256 + 1)   ; buffer allocation + null char

    ; call readline
    mov esi, esp        ; esi = buffer
    push 256            ; buffer size
    push esi
    call readline
    ; store length of input
    lea esi, [ebp-4]
    mov dword [esi], eax
    clean_stack(2)

    ; null terminate the input
    mov esi, esp            ; esi = buffer[0]
    add esi, eax            ; esi = buffer[strlen]
    mov byte [esi], 0       ; null terminate

    ; increment each character (if A to z) by 1
    mov edi, esp            ; edi = buffer
    L_while2:
    cmp byte [edi], 0       ; if null char, then end
    je L_while2_end
    cmp byte [edi], "A"
    jl L_while2_next
    cmp byte [edi], "Z"
    jg L_while2_next

    ; if Z
    cmp byte [edi], "Z"
    jne L_while2_not_Z
    ; then A
    mov byte [edi], "A"
    jmp L_while2_next
    ; else increment
    L_while2_not_Z:
    add byte [edi], 1
    L_while2_next:
    add edi, 1
    jmp L_while2
    L_while2_end:
    ; print to fileout
    lea esi, [ebp-4]
    mov edi, esp
    push dword [esi]            ; strlen
    push edi                    ; str
    call print_to_fileout
    clean_stack(2)
    ; if filein is not stdin, then print newline
    cmp dword [filein], stdin
    je L_encode_end
    call print_newline_to_fileout
    L_encode_end:
    mov esp, ebp
    pop ebp
    ret

main:
    push ebp
    mov ebp, esp
    ; set default values for filein and fileout
    mov dword [filein], stdin
    mov dword [fileout], stdout

    mov ebx, 1                  ; index in loop
    mov edx, arg(0)             ; argc
    mov ecx, arg(1)             ; ecx = argv[0]
    add ecx, 4                  ; skip program name
    L_while1:
    cmp ebx,edx                 ; jump if greater than argc
    jge L_while1_end

    ; check -i
    pushad
    push 2                      ; args for strncmp
    push dword [ecx]
    push dash_i
    call strncmp
    clean_stack(3)
    cmp eax, 0                  ; is arg -i?
    popad
    jne L_check_dash_o
    mov esi, [ecx]              ; get arg string
    lea esi, [esi+2]            ; skip -i
    pushad
    push esi                    ; filename
    call open_infile
    clean_stack(1)
    cmp eax, 0                 ; if error, then continue to print to stdout
    popad
    jl end_program
    jmp L_continue_to_print
    L_check_dash_o:
    pushad
    push 2                      ; args for strncmp
    push dword [ecx]
    push dash_o
    call strncmp
    clean_stack(3)
    cmp eax, 0                  ; is arg -o ?
    popad
    jl L_continue_to_print
    mov esi, [ecx]              ; get arg string
    lea esi, [esi+2]            ; skip -o
    pushad
    push esi                    ; filename
    call open_outfile
    clean_stack(1)
    cmp eax, 0                 ; if error, then continue to print to stdout
    popad
    jl end_program

    L_continue_to_print:
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
    add ebx, 1                  ; increment index
    jmp L_while1
    L_while1_end:

    ; call encode
    call encode

    ; close file descriptors
    push dword [filein]
    call close_file
    push dword [fileout]
    call close_file
    end_program:
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
    push dword [fileout]
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
    call print_to_fileout
    mov esp, ebp
    pop ebp
    ret

readline:
    push ebp
    mov ebp, esp
    push arg(1)
    push arg(0)
    push dword [filein]
    push sys_read
    call system_call
    mov esp, ebp
    pop ebp
    ret

open_outfile:
    push ebp
    mov ebp, esp
    push 0777o
    push O_CREAT | O_WRONLY  | O_TRUNC
    push arg(0)
    push sys_open
    call system_call
    cmp eax, 0
    jl L_open_outfile_fail
    mov dword [fileout], eax
    jmp L_open_outfile_end
    L_open_outfile_fail:
    push dash_o_fail_len
    push dash_o_fail
    call print_err
    L_open_outfile_end:
    mov esp, ebp
    pop ebp
    ret

open_infile:
    push ebp
    mov ebp, esp
    push 0
    push O_RDONLY
    push arg(0)
    push sys_open
    call system_call
    cmp eax, 0
    jl L_open_infile_fail
    mov dword [filein], eax
    jmp L_open_infile_end
    L_open_infile_fail:
    push dash_i_fail_len
    push dash_i_fail
    call print_err
    L_open_infile_end:
    mov esp, ebp
    pop ebp
    ret

close_file:
    push ebp
    mov ebp, esp


    push 0
    push 0
    push arg(0)
    push sys_close
    call system_call
    mov esp, ebp
    pop ebp
    ret