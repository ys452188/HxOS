#ifndef HXOS_VGA_H
#define HXOS_VGA_H
#include <stddef.h>
#include <stdint.h>

#define VGA_BASE 0xB8000
#define VGA_BUFFER ((volatile uint16_t*)VGA_BASE)
static size_t vga_row = 0;
static size_t vga_col = 0;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint8_t current_color = 0x0F; // 黑底白字
void clearScreen(void);

static uint16_t makeVGAEntry(char c, uint8_t color) {
    return ((uint16_t)color << 8) | (uint8_t)c;
}
void setColor(uint8_t color) {
	current_color = color;
	return;
}
__attribute__((noinline))
void vga_putc(char c) {
  if (c == '\n') {
    vga_col = 0;
    vga_row++;
    if (vga_row >= VGA_HEIGHT)
      clearScreen();
    return;
  }
  size_t idx = vga_row * VGA_WIDTH + vga_col;
  VGA_BUFFER[idx] = makeVGAEntry(c, current_color);
  vga_col++;
  if (vga_col >= VGA_WIDTH) {
    vga_col = 0;
    vga_row++;
    if (vga_row >= VGA_HEIGHT)
      vga_row = VGA_HEIGHT - 1;
  }
}

void clearScreen(void) {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        VGA_BUFFER[i] = makeVGAEntry(' ', current_color);
    }
    vga_row = 0;
    vga_col = 0;
}

//取消一个字符的显示
void screen_deleteAChar() {
  if (vga_col == 0 && vga_row == 0)
    return; // 已经在最开始位置，不能删除
  if (vga_col == 0) {
    vga_row--;
    vga_col = VGA_WIDTH - 1;
  } else {
    vga_col--;
  }
  size_t idx = vga_row * VGA_WIDTH + vga_col;
  VGA_BUFFER[idx] = makeVGAEntry(' ', current_color); // 用空格覆盖
}

void vga_write(const char* msg) {
    if (!msg) return;
    for (const char* p = msg; *p; ++p) {
        vga_putc(*p);
    }
}
#endif