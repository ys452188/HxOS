set -e
echo "编译内核 (ELF)"
clang \
  -target i386-pc-none-elf \
  -m32 \
  -ffreestanding \
  -fno-stack-protector \
  -fno-pic \
  -mno-sse -mno-sse2 -mno-mmx \
  -O0 \
  -Wall -Wextra \
  -c src/Kernel.c \
  -o kernel.o
echo "链接内核"
ld.lld \
  -m elf_i386 \
  -T linker.ld \
  kernel.o \
  -o kernel.elf
echo "转换内核为裸二进制"
llvm-objcopy \
  -O binary \
  kernel.elf \
  kernel.bin
echo "汇编 No-Emulation boot image（含内核）"
nasm \
  -f bin \
  src/Bootloader.asm \
  -o boot.img
echo "构建完成：boot.img"
cp -r boot.img ISO/

#生成iso
echo "生成.iso"
xorriso -as mkisofs \
  -no-emul-boot \
  -b boot.img \
  -boot-load-size 128 \
  -o HxOS.iso \
  ISO

rm boot.img kernel.bin kernel.elf kernel.o