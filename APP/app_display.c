#include "app_display.h"
#include "display_code.h"

//堆栈
__align(4) OS_STK  TASK_DISPLAY_STK[STK_SIZE_DISPLAY]; //任务堆栈声?
#define N_MESSAGES		5
//LIFOBUFF_T ScreenIDLIFO;
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
//	appdis.sem         = OSSemCreate(0);
	appdis.MSG_Q 			 = OSQCreate(&AppDisMSG_Q[0],N_MESSAGES);//

	appdis.pDaCai = &dacai;
	appdis.pUI = &UI;
//	LIFOBuffer_Init(&ScreenIDLIFO,(u8 *)appdis.pUI->screen_idlifo, 1 ,SCREEN_BUFSIZE);
}

static void ScreenDataProcess(_dacai_usart_t *pUsart);
static  void  UsartCmdParsePkt (_dacai_usart_t *pUsart)
{
	u8 cmd;
	u16 iPara;
	
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
			iPara = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
			if(iPara==0x01)	{//回读screen id
				appdis.pUI->screen_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
			}
			else if(iPara==0x11)	{//按钮状态
				appdis.pUI->screen_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
				appdis.pUI->ctrl_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
//				UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
//				appdis.pUI->ButtonState = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
				ScreenDataProcess(pUsart);
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
}u8 ctrl_type,subtype,status;
//屏幕相关数据处理
static void ScreenDataProcess(_dacai_usart_t *pUsart)
{
	s32 temp;
	
	
	ctrl_type = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);//0x10-表示按钮 0x11-表示文本	
	if(ctrl_type==0x10)	{		
		subtype = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
		status = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);//获取按钮状态 按下/弹起
	}
	if(appdis.pUI->screen_id==Main_UIID)	{//主界面按钮响应		
		if(status == DEF_Releass)	{
			if(appdis.pUI->ctrl_id == 1)	{
				appdis.pUI->screen_id = Lab_UIID;
				DaCai_SwitchUI(appdis.pUI);
			}
			else if(appdis.pUI->ctrl_id == 3||appdis.pUI->ctrl_id == 4)	{//DNA RNA
				appdis.pUI->screen_id = Menu_UIID;
				DaCai_SwitchUI(appdis.pUI);
			}
			else if(appdis.pUI->ctrl_id == 5)	{//数据
				appdis.pUI->screen_id = Data_UIID;
				DaCai_SwitchUI(appdis.pUI);
			}
		}
	}
	else if(appdis.pUI->screen_id==Lab_UIID)	{//实验界面
		if(status == DEF_Releass)	{
			if(appdis.pUI->ctrl_id == 6)	{//新建
				appdis.pUI->screen_id = Menu_UIID;
				DaCai_SwitchUI(appdis.pUI);
			}
			else if(appdis.pUI->ctrl_id == 7)	{//删除
				DisplayUIIDAndBackup(Confirm_UIID);//备份当前界面 切换到下一个界面
				appdis.pUI->ctrl_id = 4;
				appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", &Code_Warning[0][0]);//显示 是否删除 
				DaCai_UpdateTXT(appdis.pUI);
				Sys.state |= SysState_DeleteLabTB;
			}
			else if(appdis.pUI->ctrl_id == 19)	{//启动按键
				DisplayUIIDAndBackup(Confirm_UIID);//备份当前界面 切换到下一个界面
				appdis.pUI->ctrl_id = 4;
				appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", &Code_Warning[1][0]);//显示 是否启动
				DaCai_UpdateTXT(appdis.pUI);
				Sys.state |= SysState_RunningTB;
			}
		}
	}
	else if(appdis.pUI->screen_id==Temp_UIID)	{
		if(status == DEF_Releass)	{
			if(appdis.pUI->ctrl_id == 9)	{//编辑热盖温度
				SaveUIEditInfor();//保存编辑信息
				DisplayKeyboardUI();//切换到全键盘界面					
			}
		}
	}
	else if(appdis.pUI->screen_id==Keyboard_UIID)	{//全键盘界面
		if(appdis.pUI->ctrl_id == 43&&ctrl_type==0x11)	{//用户输入值
			strcpy(appdis.pUI->pdata, (const char *)(pUsart->rx_buf+pUsart->rx_idx));
		}
		else if(appdis.pUI->ctrl_id == 42)	{//enter			
			if(appdis.pUI->editinfo.screen_id == Temp_UIID)	
			{
				temp = atoi(appdis.pUI->pdata);
				if(temp>105||temp<=0)	{
					DisplayMessageUI((char *)&Code_Message[3][0]);
				}else	{
					DisplayEditUI();//显示上次编辑界面
					appdis.pUI->ctrl_id = 6;
					appdis.pUI->datlen = strlen(appdis.pUI->pdata);//显示用户输入值
					DaCai_UpdateTXT(appdis.pUI);
				}
			}
		}
		else if(appdis.pUI->ctrl_id == 44&&status == DEF_Releass)	{//关闭 
			DisplayEditUI();//显示上次编辑界面
		}
	}
	else if(appdis.pUI->screen_id == Confirm_UIID&&status == DEF_Releass)	{//确认界面
		if(appdis.pUI->ctrl_id == 2)	{//取消键
			
		}
		else	if(appdis.pUI->ctrl_id == 3)	{//确认键
			if(Sys.state & SysState_RunningTB)	{//启动实验
//				Sys.devstate = DevState_Running;
			}
			else if(Sys.state & SysState_DeleteLabTB)	{//删除实验记录
			
			}
		}
		else 
			return;
		DisplayBackupUIID();		
	}
	else if(appdis.pUI->screen_id == Message_UIID&&status == DEF_Releass)	{//确认界面
		if(appdis.pUI->ctrl_id == 3)	{//确认键
			DisplayBackupUIID();
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
//				if(appdis.pUI->screen_id == Invalid_UIID)	{
//					DaCai_GetScreenID();
//				}
//				else if(appdis.pUI->screen_id == Welcome_UIID)	{
				if(appdis.pUI->screen_id == Invalid_UIID)	{
					OSTimeDly(2000);
					appdis.pUI->screen_id = Main_UIID;							
					DaCai_SwitchUI(appdis.pUI);//显示主界面
//					OSFlagPost(SysFlagGrp, (OS_FLAGS)FLAG_GRP_3, OS_FLAG_SET, &err);
				}
				else {
//					DaCai_TimeGet();
				}
			}
		}
	}
}
