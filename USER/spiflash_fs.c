#include "spiflash_fs.h"

//spi flash读写底层驱动与FATFS文件系统接口函数
void W25_WriteSector(const BYTE *buf, DWORD sector /* Sector address in LBA */,UINT count)
{
	UINT w_addr = sector*SECTOR_SIZE;

	BSP_W25Qx_Erase_Sector(w_addr);
	BSP_W25Qx_Write((uint8_t *)buf, w_addr, SECTOR_SIZE*count);
}

void W25_ReadSector(const BYTE *buf, DWORD sector /* Sector address in LBA */,UINT count)
{
	UINT r_addr = sector*SECTOR_SIZE;
	
	BSP_W25Qx_Read((uint8_t *)buf, r_addr, SECTOR_SIZE*count);
}

