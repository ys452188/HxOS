#pragma once
#include "Inout.h"
#include <stdint.h>

#define KBD_DATA_PORT 0x60

// 简单的 scancode -> ascii 映射
static const char scancode_map[128] = {
  0,   27,  '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',
  '-', '=', '\b', '\t', 'q',  'w',  'e',  'r',  't',  'y',  'u',  'i',
  'o', 'p', '[',  ']',  '\n', 0,    'a',  's',  'd',  'f',  'g',  'h',
  'j', 'k', 'l',  ';',  '\'', '`',  0,    '\\', 'z',  'x',  'c',  'v',
  'b', 'n', 'm',  ',',  '.',  '/',  0,    '*',  0,    ' ',  0,    0,
  0,   0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  '7', '8', '9',  '-',  '4',  '5',  '6',  '+',  '1',  '2',  '3',  '0',
  '.', 0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  0,   0,   0,    0,    0,    0,    0,    0,    0,    0,    0,    0,
  0,   0,   0,    0,    0,    0,    0,    0
};

char keyboardGetCharPoll() {
  while (1) {
    uint8_t status = inb(0x64);
    if (status & 1) {
      uint8_t sc = inb(KBD_DATA_PORT);
      if (sc < 128) {
        char c = scancode_map[sc];
        if (c)
          return c;
      }
    }
  }
}
