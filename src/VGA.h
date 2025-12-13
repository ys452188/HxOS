#include <stddef.h>
#include <stdint.h>

#define VGA_BASE 0xB8000
static uint16_t *vga_buffer = (uint16_t *)VGA_BASE;
static size_t vga_row = 0;
static size_t vga_col = 0;
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 25;
static uint8_t current_color = 0x07; // 灰底黑字

static uint16_t make_vga_entry(char c, uint8_t color) {
  return (uint16_t)c | ((uint16_t)color << 8);
}

void vga_putc(char c) {
  if (c == '\n') {
    vga_col = 0;
    vga_row++;
    if (vga_row >= VGA_HEIGHT)
      vga_row = 0; // 简单滚屏处理：回到顶部
    return;
  }
  size_t idx = vga_row * VGA_WIDTH + vga_col;
  vga_buffer[idx] = make_vga_entry(c, current_color);
  vga_col++;
  if (vga_col >= VGA_WIDTH) {
    vga_col = 0;
    vga_row++;
    if (vga_row >= VGA_HEIGHT)
      vga_row = 0;
  }
}

void clear_screen() {
  // 重启屏幕
  for (int i = 0; i < 80 * 25; i++) {
    ((uint16_t *)VGA_BASE)[i] = 0;
  }
  vga_row = 0;
  vga_col = 0;
}

void vga_write(const char *s) {
  for (const char *p = s; *p; ++p)
    vga_putc(*p);
}
