// src/kernel.c
#include "Inout.h"
#include "Keyboard.h"
#include "VGA.h"
#include <stddef.h>
#include <stdint.h>

// 简单实现 memcmp
int memcmp(const void *a, const void *b, size_t n);
void vga_write(const char*);
char keyboard_getchar_poll();
// 声明 linker.ld 中的符号
extern char __bss_start[];
extern char __bss_end[];
// 手动清空 BSS 段的函数
void clean_bss() {
    char* p = __bss_start;
    while (p < __bss_end) {
        *p = 0;
        p++;
    }
}
__attribute__((section(".text.kmain")))    //使kernel.bin[0]是kmain的第一条指令
void kmain() {
	clean_bss();
	clear_screen();
  vga_write("HxOS - welcome!\n");
  vga_write("-------------------\n");
  vga_write("A simple hobby OS kernel\nBy cuso4IsVeryNice(Xianghan.Huang)\n");
  vga_write("Type commands. Builtins: help, echo, clear\n\n");
  char buf[128];
  while (1) {
    vga_write("HxOS=> ");
    size_t pos = 0;
    // 输入
    while (1) {
      char c = keyboard_getchar_poll();
      if (c == '\r' || c == '\n') {
        vga_write("\n");
        buf[pos] = 0;
        break;
      } else if (c == '\b' || c == 127) { // 处理退格
        if (pos) {
          pos--;
          screen_deleteAChar();
        }
      } else {
        if (pos < sizeof(buf) - 1) {
          buf[pos++] = c;
          char s[2] = {c, 0};
          vga_write(s);
        }
      }
    }
    // 简单命令解析
    if (pos == 0)
      continue;
    if (memcmp(buf, "help", 4) == 0) {
      vga_write("Builtins: help, echo <text>, clear\n");
    } else if (memcmp(buf, "clear", 5) == 0) {
      clear_screen();
    } else if (memcmp(buf, "echo ", 5) == 0) {
      vga_write(buf + 5);
      vga_write("\n");
    } else {
      vga_write("Unknown command\n");
    }
  }
  while(1) {
    //使用 hlt 指令让 CPU 暂停，降低功耗，直到下一个中断到来
    __asm__ volatile("hlt");
  }
}

// 简单实现 memcmp
int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *pa = a, *pb = b;
  for (size_t i = 0; i < n; i++)
    if (pa[i] != pb[i])
      return pa[i] - pb[i];
  return 0;
}
