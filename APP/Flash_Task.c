#include "Flash_Task.h"
#include "rw_spiflash.h"

static void CheckSPIFlash(void);

void Flash_TaskInit(void)
{
//	DataInit();
	CheckSPIFlash();
//	if(SysError.Y2.bits.b7 == DEF_Active)	{
		if(FlashFSInit()==FR_OK)	{
			BSP_PRINTF("filesys init ok");		
			CreateLogFile();//创建系统文件
		}
//	}
}

static void CheckSPIFlash(void)
{
	u8 devid[2];
	
	BSP_W25Qx_Read_ID(devid);
	if(devid[0] == W25QXX_MANUFACTURER_ID)	{//devid[1] -- DEVICE_ID; devid[0] -- MANUFACTURER_ID 
		BSP_PRINTF("SPI FLASH OK.");
//		SysError.Y2.bits.b7 = DEF_Active;
	}else	{
		BSP_PRINTF("SPI FLASH error.");
//		SysError.Y2.bits.b7 = DEF_Inactive;
	}
}

