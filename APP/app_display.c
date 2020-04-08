#include "app_display.h"
#include "display_code.h"
#include "app_temp.h"
#include "DaCai_TouchEvent.h"
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
u8 touchid;
static void ButtonClickProcess(u8 button);
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
		case 0x01:
			if(appdis.pUI->screen_id == Temp_UIID)	{
				u16 x,y;
				x = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
				y = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
				touchid = TempButtonClick(x,y);
				ButtonClickProcess(touchid);
			}
		case 0xb1:	{//画面相关指令
			iPara = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
			if(iPara==0x01)	{//回读screen id
				appdis.pUI->screen_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
				if(appdis.pUI->screen_id == Temp_UIID)	{//温度程序界面 刷新温度图形
					DisplayTempProgramUI(0,0);//刷新温度界面
				}
			}
			else if(iPara==0x11)	{//按钮状态
				appdis.pUI->screen_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
				appdis.pUI->ctrl_id = UsartRxGetINT16U(pUsart->rx_buf,&pUsart->rx_idx);
				ScreenDataProcess(pUsart);
			}
			break;
		}
		case 0xf7:	{//时间格式是BCD码
			SysTime.tm_year = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx)) + 2000;
			SysTime.tm_mon = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_wday = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_mday = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_hour = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_min = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			SysTime.tm_sec = BCD_Decimal(UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx));
			break;
		}
	}
}

static void ButtonClickProcess(u8 button)
{
	if(button>=0&&button<=5)	
	{
		if(touchid==5)	{
			if(temp_data.HeatCoverEnable == DEF_False)	{									
				temp_data.HeatCoverEnable = DEF_True;
			}
			else if(temp_data.HeatCoverEnable == DEF_True)	{
				temp_data.HeatCoverEnable = DEF_False;
			}
			DisplayHeatCoverIcon();		
		}
		else 	{
			TempButtonCheckOn(button);
		}
		OSTimeDly(10);
		DisplayTempProgramUI(0,0);
	}
}

u8 ctrl_type,subtype,status;
s32 g_tempdata;
//屏幕相关数据处理
static void ScreenDataProcess(_dacai_usart_t *pUsart)
{
//	u8 ctrl_type,subtype,status;
	s32 temp;
	u8 iPara;
	
	ctrl_type = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);//0x10-表示按钮 0x11-表示文本	
	if(ctrl_type==0x10)	{		
		subtype = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
		status = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);//获取按钮状态 按下/弹起
	}
	if(appdis.pUI->screen_id==Main_UIID)	{//主界面按钮响应		
		if(status == DEF_Release)	{
			if(appdis.pUI->ctrl_id == 1)	{
				DisplayLabUI();
			}
			else if(appdis.pUI->ctrl_id == 3||appdis.pUI->ctrl_id == 4)	{//DNA RNA
				DisplayMenuUI();
			}
			else if(appdis.pUI->ctrl_id == 5)	{//数据
				appdis.pUI->screen_id = Data_UIID;
				DaCai_SwitchUI(appdis.pUI);
			}
			else if(appdis.pUI->ctrl_id == 5)	{//数据
				
			}
		}
	}
	else if(appdis.pUI->screen_id==Lab_UIID)	{//实验界面
		if(status == DEF_Release)	{
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
			else if(appdis.pUI->ctrl_id == 19)	{//启动实验
				DisplayQiTingLab();
			}
		}
	}
	else if(appdis.pUI->screen_id==Menu_UIID&&status == DEF_Release)	{//菜单界面
		if(appdis.pUI->ctrl_id == 2)	{//进入温度程序		
			ClearTempProgramIdx();		
			DisplayTempProgramUI(0,1);	//刷新温度界面	清屏
		}
		else if(appdis.pUI->ctrl_id == 19)	{//启动实验
			DisplayQiTingLab();
		}
	}
	else if(appdis.pUI->screen_id==Temp_UIID)	{//温度程序	
		if(status == DEF_Release)	{		
			if(appdis.pUI->ctrl_id == 9)	{//编辑热盖温度
				SaveUIEditInfor();//保存编辑信息
				DisplayKeyboardUI();//切换到全键盘界面					
			}
			else if(appdis.pUI->ctrl_id == 1)	{//加阶
//				UUIDBackup();
				DisplayStageUI();
			}
			else if(appdis.pUI->ctrl_id == 4)	{//加步
//				UUIDBackup();
				DisplayStepUI();
			}
			else if(appdis.pUI->ctrl_id == 33)	{//上一页
				ClearTempProgramIdx();
				DisplayTempProgramUI(0,1);
			}
			else if(appdis.pUI->ctrl_id == 34)	{//下一页
				DisplayTempProgramUI(1,1);
			}
		}
//		if(appdis.pUI->ctrl_id == 8)	{//关闭热盖温度
//			if(status == DEF_Release)	{
//				HeatCoverOnOff(DEF_False);
//			}
//			else if(status == DEF_Press)
//				HeatCoverOnOff(DEF_True);
//		}		
	}
	else if(appdis.pUI->screen_id==Stage_UIID)	{//阶段设置
		if(Sys.state&SysState_ReadTXT)	{
			strcpy(appdis.pUI->pdata, (const char *)(pUsart->rx_buf+pUsart->rx_idx));
			temp = atoi(appdis.pUI->pdata);	
			if(appdis.pUI->ctrl_id == 4)	{//循环数设置			
				if(temp > STAGE_REPEAT_MAX||temp <= 0)	{
					DisplayMessageUI((char *)&Code_Message[3][0]);
				}
				else	{
					temp_data.stage[temp_data.StageNum].RepeatNum = temp;
					appdis.pUI->ctrl_id = 8;
					DaCai_ReadTXT(appdis.pUI);
				}
			}
			else if(appdis.pUI->ctrl_id == 8)	{//总步数设置				
				if(temp > STEP_MAX||temp <= 0)	{
					DisplayMessageUI((char *)&Code_Message[3][0]);
				}
				else	{
					temp_data.stage[temp_data.StageNum].StepNum = temp;
					Sys.state &= ~SysState_ReadTXT;
					DisplayMessageUI((char *)&Code_Message[4][0]);	
					Sys.state |= SysState_SetOK;				
				}
			}
		}
		else if(appdis.pUI->ctrl_id == 24)	{//enter
			appdis.pUI->ctrl_id = 4;
			DaCai_ReadTXT(appdis.pUI);
			Sys.state |= SysState_ReadTXT;
		}
		else if(appdis.pUI->ctrl_id == 25&&status == DEF_Press)	{//关闭 
			Sys.state &= ~SysState_ReadTXT;
			if(Sys.state & SysState_SetOK)	{
				Sys.state &= ~SysState_SetOK;
				temp_data.StageNum++;
			}
		}
	}
	else if(appdis.pUI->screen_id==Step_UIID)	{//步设置
		if(Sys.state&SysState_ReadTXT)	{
			strcpy(appdis.pUI->pdata, (const char *)(pUsart->rx_buf+pUsart->rx_idx));
			g_tempdata = temp = (int)(atof(appdis.pUI->pdata)*10);	
			if(appdis.pUI->ctrl_id == 8)	{//温度值设置
				if(temp > HOLE_TEMP_MAX||temp < HOLE_TEMP_MIN)
					DisplayMessageUI((char *)&Code_Message[3][0]);
				else	{
					iPara = temp_data.StageNum;
					temp_data.stage[iPara].step[temp_data.stage[iPara].StepNum].temp = temp;
					appdis.pUI->ctrl_id = 9;
					DaCai_ReadTXT(appdis.pUI);
				}
			}	
			else if(appdis.pUI->ctrl_id == 9)	{//恒温时间 min	
				temp /= 10;
				if(temp > 10||temp < 0)	
					DisplayMessageUI((char *)&Code_Message[3][0]);
				else	{
					iPara = temp_data.StageNum;
					temp_data.stage[iPara].step[temp_data.stage[iPara].StepNum].tim = temp*60;
					appdis.pUI->ctrl_id = 10;
					DaCai_ReadTXT(appdis.pUI);
				}
			}
			else if(appdis.pUI->ctrl_id == 10)	{//恒温时间 sec	
				temp /= 10;
				if(temp > 60||temp < 0)	
					DisplayMessageUI((char *)&Code_Message[3][0]);
				else	{
					iPara = temp_data.StageNum;
					temp_data.stage[iPara].step[temp_data.stage[iPara].StepNum].tim += temp;
					Sys.state &= ~SysState_ReadTXT;
					DisplayMessageUI((char *)&Code_Message[4][0]);					
					Sys.state |= SysState_SetOK;
				}
			}
		}
		else if(appdis.pUI->ctrl_id == 24)	{//enter
			appdis.pUI->ctrl_id = 8;
			DaCai_ReadTXT(appdis.pUI);
			Sys.state |= SysState_ReadTXT;//回读所有文本内容
		}
		if(appdis.pUI->ctrl_id == 25&&status == DEF_Press)	{//关闭 			
			Sys.state &= ~SysState_ReadTXT;
			if(Sys.state & SysState_SetOK)	{
				Sys.state &= ~SysState_SetOK;
				iPara = temp_data.StageNum;
				temp_data.stage[iPara].StepNum++;
				temp_data.StageNum++;
			}
		}
		else if(appdis.pUI->ctrl_id == 1)	{
			if(status == DEF_Release)	{
				iPara = temp_data.StageNum;
				temp_data.stage[iPara].step[temp_data.stage[iPara].StepNum].CollEnable = DEF_False;
			}
			else if(status == DEF_Press)	{
				iPara = temp_data.StageNum;
				temp_data.stage[iPara].step[temp_data.stage[iPara].StepNum].CollEnable = DEF_True;
			}
		}		
	}
	else if(appdis.pUI->screen_id==Keyboard_UIID)	{//全键盘界面
		if(appdis.pUI->ctrl_id == 43&&ctrl_type==0x11)	{//用户输入值
			strcpy(appdis.pUI->pdata, (const char *)(pUsart->rx_buf+pUsart->rx_idx));		
			if(appdis.pUI->editinfo.screen_id == Temp_UIID)	
			{
				if(appdis.pUI->editinfo.ctrl_id == 9)	{//编辑热盖温度
					temp = atoi(appdis.pUI->pdata);
					if(temp > HEATCOVER_TEMPMAX || temp < HEATCOVER_TEMPMIN)	{
						DisplayMessageUI((char *)&Code_Message[3][0]);
					}else	{
						DisplayEditUI();//显示上次编辑界面
						appdis.pUI->ctrl_id = 6;
						appdis.pUI->datlen = strlen(appdis.pUI->pdata);//显示用户输入值
						DaCai_UpdateTXT(appdis.pUI);
						temp_data.HeatCoverEnable = temp;//保存用户输入值
					}
				}
			}
		}
		else if(appdis.pUI->ctrl_id == 44&&status == DEF_Release)	{//关闭 
			DisplayEditUI();//显示上次编辑界面
			if(appdis.pUI->screen_id==Temp_UIID&&appdis.pUI->ctrl_id == 9)	
				DisplayTempProgramUI(0,0);//刷新温度界面
		}
	}
	else if(appdis.pUI->screen_id == Confirm_UIID&&status == DEF_Release)	{//确认界面
		if(appdis.pUI->ctrl_id == 2)	{//取消键
			Sys.state &= ~SysState_RunningTB;
			Sys.state &= ~SysState_DeleteLabTB;
		}
		else	if(appdis.pUI->ctrl_id == 3)	{//确认键
			if(Sys.state & SysState_RunningTB)	{//启动实验
				Sys.state &= ~SysState_RunningTB;
				//if(Sys.devstate == DevState_IDLE)	
				{					
					iPara = StartAPPTempCtrl();
					DisplayBackupUIID();	
					if(iPara==0)	{//启动温控					
						DisplayMessageUI((char *)&Code_Message[6][0]);//参数错误 无法启动
					}	
					return;
				}
			}
			if(Sys.state & SysState_StopTB)	{//启动实验
				Sys.state &= ~SysState_StopTB;
				StopAPPTempCtrl();
			}
			else if(Sys.state & SysState_DeleteLabTB)	{//删除实验记录
				Sys.state &= ~SysState_DeleteLabTB;
			}
		}
		else 
			return;
		DisplayBackupUIID();		
	}
	else if(appdis.pUI->screen_id == Message_UIID&&status == DEF_Release)	{//确认界面
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
