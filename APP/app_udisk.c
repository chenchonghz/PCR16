#include "app_udisk.h"
#include "rw_udisk.h"
#include "bsp.h"
//堆栈
__align(4) OS_STK  TASK_UDISK_STK[STK_SIZE_UDISK]; //任务堆栈声?
#define N_MESSAGES		5
static u8 data_buf[30];
_appudisk_t appudisk;
static void    *MyArrayOfMsg[N_MESSAGES];//消息队列数组
static  message_pkt_t    msg_pkt_udisk;
static void TaskUDISK(void * ppdata);
extern USBH_HandleTypeDef  hUSB_Host;
//void CheckUpdateFWName(void);
//#define	UPDATE_FW_NAME	"PerfuserFirmware.bin"
//#define	UPDATE_FW_ATTR_SIZE		0X10
//#define	UPDATE_FW_FILE_MINSIZE		0X400

void AppUSBInit(void)
{
	OSTaskCreate(TaskUDISK,  (void * )0, (OS_STK *)&TASK_UDISK_STK[STK_SIZE_UDISK-1], TASK_PRIO_UDISK);
}

static void DataInit(void)
{
	appudisk.MSG_Q 			 = OSQCreate(&MyArrayOfMsg[0],N_MESSAGES);//
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
		msg = (message_pkt_t *)OSQPend(appudisk.MSG_Q, 0, &err);
		if(err==OS_ERR_NONE)    {
			if(msg->Src == MSG_USB_START || msg->Src == MSG_USB_READY || msg->Src == MSG_USB_DISCONNECT)	{
				USBH_Process(&hUsbHostFS);
				if(hUsbHostFS.gState != HOST_IDLE)	{
					userMSC_Handle =  (MSC_HandleTypeDef *) hUsbHostFS.pActiveClass->pData;
					if(userMSC_Handle->state != MSC_IDLE)	{
						OSTimeDly(10);
						msg_pkt_udisk.Src = MSG_USB_START;
						OSQPost(appudisk.MSG_Q, &msg_pkt_udisk);		
					}						
				}
				if(msg->Src == MSG_USB_READY||msg->Src == MSG_USB_DISCONNECT)	{
					MountUDISK(msg->Src);
					if(udiskfs.flag & UDISKFLAG_MOUNTED && \
						(sys.devstate == DevState_IDLE||sys.devstate == DevState_Error))	{//设备空闲或故障情况下 检查是否有升级固件
//						CheckUpdateFWName();
					}
				}
			}
			else if(msg->Src == MSG_COPY_LOGFILE)	{//执行log拷贝

			}
			else if(msg->Src == MSG_COPY_PERFUSEFILE)	{//执行灌注数据拷贝

			}
			else if(msg->Src == MSG_COPY_UPDATE_FW)	{//执行升级固件拷贝
//				msg_pkt_udisk.Src = MSG_MESSAGE_DSIPLAY;
//				if(sys.state & SysState_UdiskConnect)	{
//					msg_pkt_udisk.Data = (char *)&Code_Message[2][0];
//					OSQPost(appdis.MSG_Q,&msg_pkt_udisk);//通知app_display显示
//					if(CopyFileToSpiflash(UPDATE_FW_NAME, UPDATE_FW_NAME)==1)	{
//						msg_pkt_udisk.Data = (char *)&Code_Message[0][0];
//						OSQPost(appdis.MSG_Q,&msg_pkt_udisk);//通知app_display显示
//						FWUpdate_reboot();
//					}
//				}else	{
//					msg_pkt_udisk.Data = (char *)&Code_Message[1][0];
//				}
//				OSQPost(appdis.MSG_Q,&msg_pkt_udisk);//通知app_display显示
			}
		}
  }
}
#define		ApplicationAddress			0x0800F000
#define		ApplicationJump				0x200
//void CheckUpdateFWName(void)
//{
//	FRESULT res;
//	char file_name[FILE_NAME_LEN];
//	char FW_attr[UPDATE_FW_ATTR_SIZE];
//	char FW_ver[UPDATE_FW_ATTR_SIZE];
//	u32 rsize;
//	
//	sprintf(file_name, "%s%s", USBHPath, UPDATE_FW_NAME);//固件路径
//	res = f_open(&udiskfs.fil, file_name, FA_READ);
//	if(res==FR_OK)	{
//		if(f_size(&udiskfs.fil)<UPDATE_FW_FILE_MINSIZE)	{//固件太小 判定为有问题固件
//			f_close(&udiskfs.fil);
//			return;
//		}
//		f_read(&udiskfs.fil, FW_attr, UPDATE_FW_ATTR_SIZE, &rsize);
//		if(0==strcmp(FW_attr, firmware_check_string))  {//校验公司名称
//			f_read(&udiskfs.fil, FW_attr, UPDATE_FW_ATTR_SIZE, &rsize);
//			if(0==strcmp(FW_attr, firmware_attr))        {//校验设备名称
//				f_lseek(&udiskfs.fil, ApplicationJump);
//				f_read(&udiskfs.fil, FW_attr, UPDATE_FW_ATTR_SIZE, &rsize);
//				if (((*(volatile INT32U *)(FW_attr)) & 0x2FFE0000) == 0x20000000)	{//校验固件启动向量					
//					msg_pkt_udisk.Src = MSG_WARNING_DSIPLAY;
//					msg_pkt_udisk.Data = data_buf;
//					f_lseek(&udiskfs.fil, 0x20);
//					f_read(&udiskfs.fil, FW_attr, UPDATE_FW_ATTR_SIZE, &rsize);//读板类型
//					f_read(&udiskfs.fil, FW_ver, UPDATE_FW_ATTR_SIZE, &rsize);//读版本号
//					if(0==strcmp(FW_attr, "Mainboard"))        {//校验板名称：主板or传感器板	
//						if(0!=strcmp(FW_ver, firmware_ver))	{//版本号一致不升级
//							rsize = sprintf((char *)data_buf, "检测到主板固件 Ver:%s\r\n",FW_ver);					
//							sys.state |= SYSSTATE_UPDATE_TBC;
//						}
//					}
////					else if(0==strcmp(FW_attr, "Slaveboard"))        {
////						rsize = sprintf((char *)data_buf, "检测到从板固件 Ver:%s\r\n",FW_ver);						
////						sys.state |= SYSSTATE_UPDATE_TBC;
////					}
//					if(sys.state & SYSSTATE_UPDATE_TBC)	{
//						if(sys_data.PowerType == POWERTYPE_DC)	//市电连接 运行升级					
//							sprintf((char *)data_buf + rsize, "是否升级？");	
//						else	{
//							sprintf((char *)data_buf + rsize, "请连接市电.");
//							sys.state &= ~SYSSTATE_UPDATE_TBC;
//						}
//						OSQPost(appdis.MSG_Q,&msg_pkt_udisk);//通知app_display显示
//					}
//				}
//			}
//		}
//		f_close(&udiskfs.fil);
//	}
//}
