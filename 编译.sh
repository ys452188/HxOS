clang -target i386-pc-none-elf -m32 -c src/Boot.S -o boot.o
clang -target i386-pc-none-elf -m32 -ffreestanding -O2 -Wall -Wextra -c src/Kernel.c -o kernel.o

ld.lld -m elf_i386 -T linker.ld -o product/boot/kernel.elf boot.o kernel.o

grub-mkrescue -o HxOS.iso product/
