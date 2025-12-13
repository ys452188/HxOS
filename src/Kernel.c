// src/kernel.c
#include "Inout.h"
#include "Keyboard.h"
#include "VGA.h"
#include <stddef.h>
#include <stdint.h>

// 简单实现 memcmp
int memcmp(const void *a, const void *b, size_t n);
void vga_write(const char *);
char keyboard_getchar_poll();

void kmain() {
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
          vga_write(" ");
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
}

// 简单实现 memcmp
int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *pa = a, *pb = b;
  for (size_t i = 0; i < n; i++)
    if (pa[i] != pb[i])
      return pa[i] - pb[i];
  return 0;
}
