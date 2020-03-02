#include "app_display.h"

//堆栈
__align(4) OS_STK  TASK_DISPLAY_STK[STK_SIZE_DISPLAY]; //任务堆栈声?
#define N_MESSAGES		5

_appdisplay_t appdis;

void    *AppDisMSG_Q[N_MESSAGES];//消息队列数组
static  message_pkt_t    msg_pkt_appdis[2];
static void TaskDisplay(void * ppdata);


void AppDisplayInit(void)
{
	OSTaskCreate(TaskDisplay,  (void * )0, (OS_STK *)&TASK_DISPLAY_STK[STK_SIZE_DISPLAY-1], TASK_PRIO_DISPLAY);
}

void StartAppDisplay(message_pkt_t *msg)
{
	OSQPost(appdis.MSG_Q, msg);
}

static void DisDatInit(void)
{
	appdis.sem         = OSSemCreate(0);
	appdis.MSG_Q 			 = OSQCreate(&AppDisMSG_Q[0],N_MESSAGES);//

	appdis.pDaCai = &dacai;
	appdis.pUI = &UI;
}

static  void  UsartCmdParsePkt (_dacai_usart_t *pUsart)
{
	u8 cmd;
	
	cmd = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
	switch(cmd)	{
		case HANDSHAKE_OK://握手成功
			appdis.pDaCai->state = DEF_ONLINE;
			break;
		case RESTART_OK://设备复位 重新连接
			appdis.pDaCai->state = DEF_OFFLINE;
			appdis.pUI->screen_id = Invalid_UIID;
			break;
		case 0xb1:	{//画面相关指令
			if(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx)==0x01)	{//回读screen id
				appdis.pUI->screen_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
			}
			break;
		}
		case 0xf7:	{//时间格式是BCD码
			SysTime.tm_year = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_mon = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_wday = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_mday = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_hour = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_min = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_sec = (UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			break;
		}
	}
}

static void TaskDisplay(void * ppdata)
{
	u8 err;
	message_pkt_t *msg;
	ppdata = ppdata;
	
	DisDatInit();
	DaCaiProto_init();
	DaCaiAPI_Init();

	for(;;)
	{
		msg = (message_pkt_t *)OSQPend(appdis.MSG_Q, 500, &err);//
		if(err==OS_ERR_NONE)    {
			if(msg->Src==USART_MSG_RX_TASK)	{//屏串口回复事件
				UsartCmdParsePkt(appdis.pDaCai->puart_t);//解析串口数据
			}
		}
		else if(err==OS_ERR_TIMEOUT)	{
			if(appdis.pDaCai->state==DEF_OFFLINE)	{
				DaCai_CheckDevice();//check online
			}else if(appdis.pDaCai->state==DEF_ONLINE)	{
				if(appdis.pUI->screen_id == Invalid_UIID)	{
					DaCai_GetScreenID();
				}
				else if(appdis.pUI->screen_id == Welcome_UIID)	{
					OSTimeDly(2000);
					appdis.pUI->screen_id = Main_UIID;							
					DaCai_SwitchUI(appdis.pUI);//显示主界面
//					OSFlagPost(SysFlagGrp, (OS_FLAGS)FLAG_GRP_3, OS_FLAG_SET, &err);
				}
				else {
					DaCai_TimeGet();
				}
			}
		}
	}
}
