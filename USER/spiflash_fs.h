#ifndef __SPIFLASH_FS_H__
#define __SPIFLASH_FS_H__

#include "includes.h"
#include "bsp_w25qxx.h"

#define SECTOR_START_ADDR     0 //第一个扇区（4k）不用fatfs管理 用于存储系统信息
#define PAGE_SIZE       W25Q64FV_PAGE_SIZE
#define SECTOR_SIZE     4096
 
#define SECTOR_COUNT	(W25Q64FV_FLASH_SIZE/W25Q64FV_SUBSECTOR_SIZE)
#define BLOCK_SIZE	65536
#define FLASH_PAGES_PER_SECTOR	(SECTOR_SIZE/PAGE_SIZE)

void W25_WriteSector(const BYTE *buf, DWORD sector /* Sector address in LBA */,UINT count);
void W25_ReadSector(const BYTE *buf, DWORD sector /* Sector address in LBA */,UINT count);

#endif

