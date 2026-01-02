#ifndef HXOS_HXSTD_H
#define HXOS_HXSTD_H
#include <stdint.h>
void* memset(void* mem, int n, size_t size) {
	size_t index = 0;
	while(index < size) {
		*((char*)mem + index) = (char)n;
		index++;
	}
	return mem;
}
int memcmp(const void *a, const void *b, size_t n) {
  const unsigned char *pa = a, *pb = b;
  for (size_t i = 0; i < n; i++)
    if (pa[i] != pb[i])
      return pa[i] - pb[i];
  return 0;
}
void* memcpy(void* dest, const void* src, size_t n) {
    char* d = (char*)dest;
    const char* s = (const char*)src;
    //按字复制
    size_t word_size = sizeof(unsigned long); //8字节
    size_t chunks = n / word_size;
    size_t remainder = n % word_size;
    unsigned long* dl = (unsigned long*)dest;
    const unsigned long* sl = (const unsigned long*)src;
    // 批量复制
    while (chunks--) {
        *dl++ = *sl++;
    }
    //复制剩余的字节
    while (remainder--) {
        *((char*)dl++) = *((char*)sl++);
    }
    return dest;
}

uint64_t div64(uint64_t dividend, uint32_t divisor, uint32_t *remainder) {
    uint32_t high = dividend >> 32;
    uint32_t low = (uint32_t)dividend;
    uint32_t q_high, q_low, rem;
    // 先除高32位
    __asm__ (
        "divl %4"
        : "=a"(q_high), "=d"(rem)
        : "a"(high), "d"(0), "rm"(divisor)
    );
    // 再除低32位（接续余数）
    __asm__ (
        "divl %4"
        : "=a"(q_low), "=d"(rem)
        : "a"(low), "d"(rem), "rm"(divisor)
    );
    if (remainder) *remainder = rem;
    return ((uint64_t)q_high << 32) | q_low;
}
//-----------------------------基于PIT定时器的sleep----------
#define PIT_COMMAND 0x43
#define PIT_CHANNEL0 0x40
#define PIT_FREQ 1193182
#include "VGA.h"
void initPit(uint32_t frequency) {
    uint32_t divisor = PIT_FREQ / frequency;
    // 设置命令字：通道0，访问模式LOBYTE/HIBYTE，模式3（方波），二进制计数
    outb(PIT_COMMAND, 0x36);
    // 写入分频值
    outb(PIT_CHANNEL0, (uint8_t)(divisor & 0xFF));
    outb(PIT_CHANNEL0, (uint8_t)((divisor >> 8) & 0xFF));
}
static volatile uint32_t timer_ticks = 0;
void timer_callback() {
    timer_ticks++;
}
void sleep_PIT(uint32_t ms) {
    uint32_t end = timer_ticks + ms; // 假设 PIT 频率设为 1000Hz (1ms/tick)
    while (timer_ticks < end) {
        // 在等待时停止 CPU 执行，节省能耗，直到下一次中断唤醒
        __asm__ volatile("hlt"); 
    }
}
//-----------------------------------------------
//忙等
void sleep_busyWait(uint32_t count) {
    for (volatile uint32_t i = 0; i < count * 1000; i++) {
        __asm__ volatile("nop");
    }
}
//------------------------------------------------
//输出数字
void putUInt64(uint64_t n) {
    if (n == 0) {
        vga_putc('0');
        return;
    }
    char buf[21];
    int i = 20;
    buf[i--] = '\0';
    
    while (n > 0) {
        uint32_t rem;
        n = div64(n, 10, &rem); // 使用自定义函数代替 n / 10 和 n % 10
        buf[i--] = rem + '0';
    }
    vga_write(&buf[i + 1]);
}
#endif