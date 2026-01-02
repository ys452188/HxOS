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
    call get_memory_map
	; ===== 开启 A20 =====
	in al, 0x92
	or al, 00000010b
	out 0x92, al
	

    ; ---- 进入保护模式 ----
    cli
    ; 重新强制设置一次 ds，确保 lgdt 取值正确
    xor ax, ax
    mov ds, ax
    lgdt [gdt_desc]

    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:pm_entry

; ===============================
print:
    lodsb
    or al, al
    jz .print_done
    mov ah, 0x0E
    int 0x10
    jmp print
.print_done:
    ret
;---------------------
get_memory_map:
    mov di, 0x1004          ; 内存条目保存的起始地址 (留出前4字节存条目数量)
    xor ebx, ebx            ; 第一次调用 ebx 必须为 0
    xor bp, bp              ; 用于统计条目数量
    mov edx, 0x534D4150     ; 魔数 'SMAP'

.next_entry:
    mov eax, 0xE820
    mov ecx, 24            ; 每个条目期望的大小
    int 0x15
    jc .get_mem_map_done         ; 如果 CF 置位，表示出错或结束
    
    cmp eax, 0x534D4150     ; 确认 eax 是否依然是 'SMAP'
    jne .get_mem_map_done
    
    add di, 32             ; 指向下一个条目存储位置
    inc bp                  ; 条目数 +1
    test ebx, ebx           ; 如果 ebx 为 0，说明获取完毕
    jnz .next_entry

.get_mem_map_done:
    mov [0x1000], bp        ; 保存总条目数
    ret
; 32-bit 保护模式
BITS 32
pm_entry:
    mov ax, 0x10    ; DATA_SEL
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x7C00

    ; 如果屏幕左上角出现红色 'P'，说明成功进入了 32 位保护模式
    mov dword [0xB8000], 0x0C500C50 ; 'P'
    
    ;内核在1MB处
    ; 拷贝内核
    mov esi, kernel_start
    mov edi, 0x00100000
    mov ecx, 16384
    rep movsd
    ;如果出现蓝色 'K'，说明拷贝完成，准备跳转
    mov dword [0xB8004], 0x094B094B ; 'K'

    ; 跳转内核
    jmp 0x08:0x00100000
; ===============================
; GDT 部分 (确保 0x08 是代码段，0x10 是数据段)
align 4
gdt_start:
    dq 0x0                ; 空描述符
gdt_code:
    dw 0xFFFF, 0x0000
    db 0x00, 0x9A, 0xCF, 0x00 ; 代码段：Base=0, Limit=4GB
gdt_data:
    dw 0xFFFF, 0x0000
    db 0x00, 0x92, 0xCF, 0x00 ; 数据段：Base=0, Limit=4GB
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
KERNEL_SIZE_BYTES equ (kernel_end - kernel_start)
KERNEL_SIZE_DWORDS equ (KERNEL_SIZE_BYTES + 3) / 4  ; 确保向上取整并按 4 字节对齐

; ===============================
; 填充到 64KB
times 65536 - ($ - $$) db 0