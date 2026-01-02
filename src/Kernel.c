// 声明 linker.ld 中的符号
extern char __bss_start[];
extern char __bss_end[];
extern char __kernel_end[]; // 链接脚本提供的内核结束地址，堆的起始地址
#include "Inout.h"
#include "Keyboard.h"
#include "VGA.h"
#include "HxStd.h"
#include "Icon.h"
#include "MemoryAllocator.h"
#include "StorageDriver.h"
#include "Hxsh.h"
#include <stddef.h>
#include <stdint.h>
// 手动清空 BSS 段的函数
void cleanBSS() {
    char* p = __bss_start;
    while (p < __bss_end) {
        *p = 0;
        p++;
    }
}
__attribute__((used, noreturn))
__attribute__((section(".text.kmain")))    //使kernel.bin[0]是kmain的第一条指令
void kmain() {
  cleanBSS();
  clearScreen();
  displayHXOSIcon();
  uint64_t usableMemorySize = getTotalUsableMemorySize();   //获取可用内存大小
  initHeapMemory(usableMemorySize);
  sleep_busyWait(2333);
  clearScreen();
  sleep_busyWait(1145);
  displayHXOSIcon();
  vga_write("HxOS - welcome!\n");
  vga_write("usableMemorySize:");
  putUInt64(usableMemorySize);
  vga_write(" byte\n");
  vga_write("-------------------\n");
  vga_write("A simple hobby OS kernel\nBy cuso4IsVeryNice(Xianghan.Huang)\n");
  vga_write("Type commands. Builtins: help, echo, clear, icon\n\n");
  /*
  FAT32_BPB bpb;
uint32_t lba = fat32_find_partition_lba();
if (fat32_read_bpb(lba, &bpb) == 0) {
    vga_write("FAT32 detected\n");
    putUInt64(bpb.bytesPerSector);
    vga_putc('\n');
    putUInt64(bpb.sectorsPerCluster);
    vga_putc('\n');

    for (int i = 0; i < 8; i++)
        vga_putc(bpb.fsType[i]);
    vga_write("Bytes/Sector: ");
	putUInt64(bpb.bytesPerSector);
}*/ 
  while (1) {
    hxsh();
  }
  while(1) {
    //使用 hlt 指令让 CPU 暂停，降低功耗，直到下一个中断到来
    __asm__ volatile("hlt");
  }
}