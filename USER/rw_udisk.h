#ifndef __rw_udisk_H__
#define __rw_udisk_H__

#include "includes.h"

#define UDISKFLAG_MOUNTED		DEF_BIT00_MASK

typedef struct _udiskfs	{
	FATFS fs;
	FIL	fil;
	u8 flag;
} _udiskfs_t;

extern _udiskfs_t udiskfs;
void MountUDISK(u8 src);
u8 CopyFileToUDISK(char *psrc_name, char *pdest_name);
u8 CopyFileToSpiflash(char *psrc_name, char *pdest_name);
#endif
