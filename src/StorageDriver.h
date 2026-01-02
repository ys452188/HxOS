//ATA PIO
#ifndef HXOS_STORAGE_DRIVER_H
#define HXOS_STORAGE_DRIVER_H
#include "Inout.h"
#include "HxStd.h"
#pragma pack(push, 1)
typedef struct {
    uint8_t  jmpBoot[3];      // EB ?? 90
    uint8_t  OEMName[8];

    uint16_t bytesPerSector;  // 通常 512
    uint8_t  sectorsPerCluster;
    uint16_t reservedSectors; // FAT32 通常 32
    uint8_t  numFATs;         // 通常 2
    uint16_t rootEntryCount;  // FAT32 = 0
    uint16_t totalSectors16;  // FAT32 = 0
    uint8_t  media;
    uint16_t fatSize16;       // FAT32 = 0
    uint16_t sectorsPerTrack;
    uint16_t numHeads;
    uint32_t hiddenSectors;
    uint32_t totalSectors32;

    // FAT32 扩展
    uint32_t fatSize32;
    uint16_t extFlags;
    uint16_t fsVersion;
    uint32_t rootCluster;     // 通常 2
    uint16_t fsInfo;
    uint16_t backupBootSector;
    uint8_t  reserved[12];

    uint8_t  driveNumber;
    uint8_t  reserved1;
    uint8_t  bootSignature;   // 0x29
    uint32_t volumeID;
    uint8_t  volumeLabel[11];
    uint8_t  fsType[8];       // "FAT32   "
} FAT32_BPB;
#pragma pack(pop)

static void ata_wait_ready(void) {
    uint8_t status;
    do {
        status = inb(0x1F7);
    } while (status & 0x80);   // BSY
}
static int ata_wait_drq(void) {
    uint8_t status;
    while (1) {
        status = inb(0x1F7);
        if (status & 0x01) return -1;   // ERR
        if (status & 0x08) return 0;    // DRQ
    }
}
int ata_read_sector(uint32_t lba, uint16_t* buf) {
    ata_wait_ready();

    outb(0x1F2, 1);                  // 扇区数
    outb(0x1F3, (uint8_t)(lba));
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F7, 0x20);               // READ SECTOR

    if (ata_wait_drq() != 0)
        return -1;
    for (int i = 0; i < 256; i++)
        buf[i] = inw(0x1F0);
    return 0;
}
uint32_t fat32_find_partition_lba(void) {
    static uint8_t sector[512];
    ata_read_sector(0, sector);

    /* 第一个分区表项 */
    uint8_t* entry = sector + 0x1BE;

    /* 分区起始 LBA 在偏移 8 */
    return *(uint32_t*)(entry + 8);
}
int fat32_read_bpb(uint32_t lba, FAT32_BPB* out) {
    if (!out) return -1;

    /* 512 字节扇区缓冲 */
    static uint8_t sector[512];

    ata_read_sector(lba, sector);

    /* 基本合法性检查 */
    if (sector[510] != 0x55 || sector[511] != 0xAA)
        return -2;

    FAT32_BPB* bpb = (FAT32_BPB*)sector;

    /* FAT32 特征判断 */
    if (bpb->fatSize16 != 0)
        return -3;   // 不是 FAT32

    /* 拷贝 BPB */
    *out = *bpb;

    return 0;
}
#endif