.set ALIGN, 1<<0   /*内存对齐*/
.set MEMINFO, 1<<1 /*内存信息*/
.set FLAGS, ALIGN | MEMINFO /*标志位*/
.set MAGIC, 0x1BADB002 /*多重引导魔数*/
.set CHECKSUM, -(MAGIC + FLAGS) /*校验和*/

.section .multiboot
.align 4
.long MAGIC          /*多重引导魔数*/
.long FLAGS          /*标志位*/
.long CHECKSUM      /*校验和*/

.section .text
.global _start
.type _start, @function

_start:
    movl %ebx, multiboot_info_ptr /*保存多重引导信息指针*/
    /*设置栈指针 */
    movl $stack_top, %esp
    call kmain                   /*调用内核主函数*/
1:
    hlt                         /*停止CPU*/
    jmp 1b                      /*无限循环*/

    .section .bss
    .align 4
multiboot_info_ptr:
    .long 0                     /*多重引导信息指针*/
stack_bottom:
    /*分配约16MB的栈空间*/
    .skip 16384000
stack_top: