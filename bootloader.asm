[bits 16]
[org 0x7c00]

KERNEL_OFFSET equ 0x1000

mov [BOOT_DRIVE], dl

mov bp, 0x9000
mov sp, bp

call load_kernel
call switch_to_32bit

jmp $

%include "gdt.asm"

[bits 16]
load_kernel:
    mov bx, KERNEL_OFFSET

    mov ah, 0x02
    mov al, 15

    mov ch, 0x00
    mov cl, 0x02

    mov dh, 0x00
    mov dl, [BOOT_DRIVE]

    int 0x13

    ret

[bits 16]
switch_to_32bit:
    cli                     
    lgdt [gdt_descriptor]   
    mov eax, cr0
    or eax, 0x1             
    mov cr0, eax
    
    jmp CODE_SEG:init_32bit

[bits 32]
init_32bit:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call BEGIN_32BIT 

[bits 32]
BEGIN_32BIT:
    ;mov ebx,0xb8000
    ;mov al,'T'
    ;mov ah,0x0F
    ;mov [ebx],ax

    call KERNEL_OFFSET

    jmp $

BOOT_DRIVE db 0

times 510 - ($-$$) db 0
dw 0xaa55


