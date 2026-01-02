#pragma once
#include "Inout.h"
#include "Keyboard.h"
#include "VGA.h"
#include "HxStd.h"
#include "Icon.h"
#include "MemoryAllocator.h"
#include "StorageDriver.h"
#include <stddef.h>
#include <stdint.h>
int memcmp(const void *a, const void *b, size_t n);
void hxsh(void) {
	vga_write("HxOS=> ");
    char* buf = hxMalloc(128);
  	if(!buf) {
  		vga_write("[ERR] Memory alloction fault!\n");
  	}
  	memset(buf, 0, 128);
  	size_t buf_len = 128;
    size_t pos = 0;
    // 输入
    while (1) {
      char c = keyboardGetCharPoll();
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
        if (pos >= buf_len ) {
        	size_t old = buf_len;
        	buf_len+=128;
        	buf = hxRealloc(buf, buf_len);
        	if(!buf) {
  				vga_write("[ERR] Memory alloction fault!\n");
  			}
  			memset(buf+old, 0, buf_len-old);
        }
        buf[pos++] = c;
        char s[2] = {c, 0};
        vga_write(s);
      }
    }
    // 简单命令解析
    if (pos == 0) {
      hxFree(buf);
      buf_len = 0;
      return;
    }
    if (memcmp(buf, "help", 4) == 0) {
      vga_write("Builtins:\n help\n echo <text>\n clear\n icon\n");
    } else if (memcmp(buf, "clear", 5) == 0) {
      clearScreen();
    } else if (memcmp(buf, "echo ", 5) == 0) {
      vga_write(buf + 5);
      vga_write("\n");
    } else if (memcmp(buf, "icon", 4) == 0) {
      displayHXOSIcon();
    }else {
      vga_write("Unknown command\n");
    }
    
    hxFree(buf);
    buf_len = 0;
}