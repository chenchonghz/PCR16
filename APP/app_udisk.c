#include "app_udisk.h"
#include "rw_udisk.h"
#include "bsp.h"
#include  "app_system_update.h"
//堆栈
__align(4) OS_STK  TASK_UDISK_STK[STK_SIZE_UDISK]; //任务堆栈声?
#define N_MESSAGES		5
static u8 data_buf[30];
_appudisk_t appudisk;
//static void    *MyArrayOfMsg[N_MESSAGES];//消息队列数组
static  message_pkt_t    msg_pkt_udisk;
static void TaskUDISK(void * ppdata);
extern USBH_HandleTypeDef  hUSB_Host;

void AppUSBInit(void)
{
	OSTaskCreate(TaskUDISK,  (void * )0, (OS_STK *)&TASK_UDISK_STK[STK_SIZE_UDISK-1], TASK_PRIO_UDISK);
}

static void DataInit(void)
{
	appudisk.mbox        = OSMboxCreate((void *)0);
}
MSC_HandleTypeDef *userMSC_Handle;
//UDISK任务
static void TaskUDISK(void * ppdata)
{
	u8 err;
	message_pkt_t *msg;
	ppdata = ppdata;
	
	DataInit();
//	OSFlagPend(SysFlagGrp, (OS_FLAGS)FLAG_GRP_3, OS_FLAG_WAIT_SET_ALL, 8000, &err);	
	MX_USB_HOST_Init();//udisk 任务起来后才可以初始化u盘，否员U盘插上开机时检测不到
	
	for(;;)
	{
		msg = (message_pkt_t *)OSMboxPend(appudisk.mbox, 0, &err);
		if(err==OS_ERR_NONE)    {
			if(msg->Src == MSG_USB_START || msg->Src == MSG_USB_READY || msg->Src == MSG_USB_DISCONNECT)	{
				USBH_Process(&hUsbHostFS);
				if(hUsbHostFS.gState != HOST_IDLE)	{
					userMSC_Handle =  (MSC_HandleTypeDef *) hUsbHostFS.pActiveClass->pData;
					if(userMSC_Handle->state != MSC_IDLE)	{
						OSTimeDly(10);
						msg_pkt_udisk.Src = MSG_USB_START;
						OSMboxPost(appudisk.mbox, &msg_pkt_udisk);		
					}						
				}
				if(msg->Src == MSG_USB_READY||msg->Src == MSG_USB_DISCONNECT)	{
					MountUDISK(msg->Src);
					if(udiskfs.flag & UDISKFLAG_MOUNTED && \
						(Sys.devstate == DevState_IDLE||Sys.devstate == DevState_Error))	{//设备空闲或故障情况下 启动升级固件任务
						msg_pkt_udisk.Src = MSG_UPDATE_FROM_UDISK;
						OSMboxPost(app_system_update.mbox, &msg_pkt_udisk);	
					}
				}
			}
		}
  }
}
