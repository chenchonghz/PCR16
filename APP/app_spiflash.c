#include "app_spiflash.h"
#include "rw_spiflash.h"

//堆栈
__align(4) OS_STK  TASK_SPIFLASH_STK[STK_SIZE_SPIFLASH]; //任务堆栈声?
#define N_MESSAGES		10

_spiflash_t spiflash;
void    *SpiFlashMSG_Q[N_MESSAGES];//消息队列数组
//static  message_pkt_t    msg_pkt_spiflash;
static void TaskSPIFLASH(void * ppdata);

void AppSpiFlashInit(void)
{
	OSTaskCreate(TaskSPIFLASH,  (void * )0, (OS_STK *)&TASK_SPIFLASH_STK[STK_SIZE_SPIFLASH-1], TASK_PRIO_SPIFLASH);
}

static void DataInit(void)
{
	//udisk.Mbox         = OSMboxCreate((void *)0);
	spiflash.MSG_Q 			 = OSQCreate(&SpiFlashMSG_Q[0],N_MESSAGES);//
}
//u8 devid[2];
void CheckSPIFlash(void)
{
	u8 devid[2];
	
	BSP_W25Qx_Read_ID(devid);
	if(devid[0] == W25QXX_MANUFACTURER_ID)	{//devid[1] -- DEVICE_ID; devid[0] -- MANUFACTURER_ID 
		BSP_PRINTF("SPI FLASH OK.");
		SysError.Y2.bits.b7 = DEF_Active;
	}else	{
		BSP_PRINTF("SPI FLASH error.");
		SysError.Y2.bits.b7 = DEF_Inactive;
	}
}
//通过任务写log
void WriteLogByTask(message_pkt_t *Src)
{
	//msg_pkt_spiflash.Src = Src;
	OSQPost(spiflash.MSG_Q, Src);
}

void StartSpiFlashTask(message_pkt_t *Src)
{
	//msg_pkt_spiflash.Src = Src;
	OSQPost(spiflash.MSG_Q, Src);
}
//extern void fs_test();
static void TaskSPIFLASH(void * ppdata)
{
	u8 err;
	message_pkt_t *msg;
	ppdata = ppdata;
	
	DataInit();
	CheckSPIFlash();
	if(SysError.Y2.bits.b7 == DEF_Active)	{
		if(FlashFSInit()==FR_OK)	{
			BSP_PRINTF("filesys init ok");		
			CreateLogFile();//创建系统文件
		}
	}
	OSFlagPost(SysFlagGrp, (OS_FLAGS)FLAG_GRP_2, OS_FLAG_SET, &err);
	
	for(;;)
	{
		msg = (message_pkt_t *)OSQPend(spiflash.MSG_Q, 0, &err);//
		if(err==OS_ERR_NONE)    {
			if(SysError.Y2.bits.b7 == DEF_Active)	{
				if(msg->Src == MSG_WRITELOG)	{//写日志
					write_log((char *)msg->Data);
				}
				else if(msg->Src == MSG_SAVE_PERFUSE_DATA)	{//保存灌注数据
//					WritePerfuseData(PerfuseData.pdata);
				}
			}
		}else if(err==OS_ERR_TIMEOUT)	{
			
		}
	}
}
