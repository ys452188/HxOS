#ifndef HXOS_MEMORY_ALLOCATOR
#define HXOS_MEMORY_ALLOCATOR
#include <stdint.h>
#include "HxStd.h"
typedef struct HeapMemoryHeader {
	uint32_t size;
	char isFree;
	struct HeapMemoryHeader* next;   //指向下一内存块的指针
} HeapMemoryHeader;
#define HEADER_SIZE sizeof(HeapMemoryHeader)

typedef struct {
    uint64_t base_addr;
    uint64_t length;
    uint32_t type;          // 1 = 可用RAM, 2 = 保留
    uint32_t extended;
    uint32_t reserved;      // 填充到 24 字节
    uint32_t padding[2];    // 填充到 32 字节，匹配汇编中的 add di, 32
} __attribute__((packed)) MemoryMapEntry;
typedef struct {
    uint32_t entry_count;
    MemoryMapEntry entries[];
} __attribute__((packed)) MemoryInfo;
//获取可用内存大小
uint64_t getTotalUsableMemorySize(void) {
    //指向约定的物理地址
    MemoryInfo* mem_info = (MemoryInfo*)0x1000;
    uint64_t total = 0;
    //putUInt64(mem_info->entry_count);
    for (uint32_t i = 0; i < mem_info->entry_count; i++) {
        // 手动定位到每一个条目的开头
        // 每个条目在汇编里间隔 32 字节，所以偏移是 i * 32
        MemoryMapEntry* entry = (MemoryMapEntry*)(0x1004 + (i * 32));
        
        if (entry->type == 1) { 
            total += entry->length;
        }
    }
    return total;
}

HeapMemoryHeader* heap_begin = NULL;
//初始化
void initHeapMemory(uint32_t total_size) {
	//vga_write("init heap...\n");
    // 将堆放在内核代码之后
    heap_begin = (HeapMemoryHeader *)__kernel_end;
    // 初始化第一个大块
    heap_begin->size = total_size - HEADER_SIZE;
    heap_begin->isFree = 1;
    heap_begin->next = NULL;
}
void* hxMalloc(uint32_t size) {
	//vga_write("alloc heap memory. size:\n");
	//putUInt64(size);
	//vga_write("\n");
    HeapMemoryHeader *current = heap_begin;
    while (current) {
        // 如果块空闲且大小足够
        if (current->isFree && current->size >= size) {
            // 如果剩余空间比 header 还大，就拆成两个块，避免浪费
            if (current->size > size + HEADER_SIZE + 4) {
                HeapMemoryHeader* new_block = (HeapMemoryHeader*)((uint8_t*)current + HEADER_SIZE + size);
                new_block->size = current->size - size - HEADER_SIZE;
                new_block->isFree = 1;
                new_block->next = current->next;

                current->size = size;
                current->next = new_block;
            }
            current->isFree = 0;
            // 返回数据部分的起始地址 (跳过 header)
            return (void*)((uint8_t*)current + HEADER_SIZE);
        }
        current = current->next;
    }
    return NULL; // 内存不足
}
void hxFree(void *ptr) {
    if (!ptr) return;
    // 找到 header 的位置
    HeapMemoryHeader* header = (HeapMemoryHeader*)((uint8_t*)ptr - HEADER_SIZE);
    header->isFree = 1;
    //vga_write("freed heap memory. size:\n");
	//putUInt64(header->size);
	//vga_write("\n");
    // 简单合并逻辑：遍历一次链表，合并相邻空闲块
    HeapMemoryHeader *current = heap_begin;
    while (current && current->next) {
        if (current->isFree && current->next->isFree) {
            current->size += current->next->size + HEADER_SIZE;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
void* hxRealloc(void* ptr, uint32_t new_size) {
    if (ptr == NULL) {
        return hxMalloc(new_size);
    }
    if (new_size == 0) {
        hxFree(ptr);
        return NULL;
    }
    // 获取当前块的 Header
    HeapMemoryHeader* header = (HeapMemoryHeader*)((uint8_t*)ptr - HEADER_SIZE);
    uint32_t old_size = header->size;

    if (old_size >= new_size) {   //缩容
        // 如果剩余空间大于 Header + 一个最小分配单位（例如4字节），则拆分
        if (old_size - new_size > HEADER_SIZE + 4) {
            // 注意：新块的物理地址 = 原数据区首地址 + new_size
            HeapMemoryHeader* newBlock = (HeapMemoryHeader*)((uint8_t*)ptr + new_size);
            newBlock->size = old_size - new_size - HEADER_SIZE;
            newBlock->isFree = 1;
            newBlock->next = header->next;
            
            header->size = new_size;
            header->next = newBlock;
            HeapMemoryHeader *current = heap_begin;
    		while (current && current->next) {
        		if (current->isFree && current->next->isFree) {
            		current->size += current->next->size + HEADER_SIZE;
            		current->next = current->next->next;
        		} else {
            		current = current->next;
        		}
    		}
            
        }
        return ptr;
    } 
    else {  //扩容
        //尝试就地扩容：检查下一个块是否空闲且空间足够
        if (header->next != NULL && header->next->isFree && 
           (header->size + HEADER_SIZE + header->next->size) >= new_size) {
            uint32_t combined_size = header->size + HEADER_SIZE + header->next->size;
            header->next = header->next->next; // 吞并下一个块
            header->size = combined_size;
            // 合并后再看是否需要再次拆分（如果合并后大太多了）
            if (header->size - new_size > HEADER_SIZE + 4) {
                 HeapMemoryHeader* splitBlock = (HeapMemoryHeader*)((uint8_t*)ptr + new_size);
                 splitBlock->size = header->size - new_size - HEADER_SIZE;
                 splitBlock->isFree = 1;
                 splitBlock->next = header->next; 
                 header->size = new_size;
                 header->next = splitBlock;
            }
            return ptr;
        } 
        //无法就地扩容，申请新地址，拷贝数据，释放旧地址
        void* new_ptr = hxMalloc(new_size);
        if (new_ptr) {
            //拷贝旧数据
            uint8_t* src = (uint8_t*)ptr;
            uint8_t* dst = (uint8_t*)new_ptr;
            for (uint32_t i = 0; i < old_size; i++) {
                dst[i] = src[i];
            }
            hxFree(ptr);
        }
        return new_ptr;
    }
}
#endif