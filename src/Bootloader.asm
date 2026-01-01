;NASM语法
; HxOS ISO Bootloader
BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    mov si, msg
    call print
	; ===== 开启 A20 =====
	in al, 0x92
	or al, 00000010b
	out 0x92, al

    ; ---- 进入保护模式 ----
    cli
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp CODE_SEL:pm_entry


print:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret
; ===============================
; 32-bit 保护模式
BITS 32
pm_entry:
    mov ax, DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00
    
    ;内核在1MB处
    mov esi, kernel_start
    mov edi, 0x00100000
    mov ecx, KERNEL_SIZE_DWORDS
    rep movsd

    ;跳转内核
    jmp CODE_SEL:0x00100000
; ===============================
; GDT
BITS 16
gdt_start:
    dq 0

gdt_code:
    dw 0xFFFF
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

gdt_data:
    dw 0xFFFF
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

gdt_end:

gdt_desc:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEL equ 0x08
DATA_SEL equ 0x10
msg db "HxOS By CuSO4isVeryNice(Xianghan.Huang) is booting...",0
; ==============================
align 4
kernel_start:
    incbin "kernel.bin"
kernel_end:
;应保证 kernel.bin 大小 <= 1MB
KERNEL_SIZE        equ kernel_end - kernel_start
KERNEL_SIZE_DWORDS equ (KERNEL_SIZE + 3) / 4

; ===============================
; 填充到 64KB
times 65536 - ($ - $$) db 0