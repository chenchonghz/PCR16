#include "display_code.h"
#include "rw_spiflash.h"

typedef struct 	{
	_MultiDat data[8];
}_MultiTXT_;
_MultiTXT_ *pMultiTXT_t;

const char Code_Warning[][12] = {	
	{"是否删除？"},
	{"是否启动？"},
	{"是否停止？"},
};

const char Code_Message[][12] = {	
	{"复制完成!"},
	{"U盘未连接!"},
	{"复制中..."},
	{"无效输入"},
	{"设置成功"},
	{"运行中"},
	{"参数错误!"},
};

const char Code_Choose[][5] = {	
	{"取消"},
	{"确认"},
	{"删除"},
	{"编辑"},
};

void SaveUIEditInfor(void)
{
	appdis.pUI->editinfo.screen_id = appdis.pUI->screen_id;
	appdis.pUI->editinfo.ctrl_id = appdis.pUI->ctrl_id;
}

void UUIDBackup(void)
{
	appdis.pUI->screen_idbk = appdis.pUI->screen_id;
	appdis.pUI->ctrl_idbk = appdis.pUI->ctrl_id;
}

void DisplayUIIDAndBackup(u8 id)
{
	UUIDBackup();
	appdis.pUI->screen_id = id;
	DaCai_SwitchUI(appdis.pUI);
}

void DisplayBackupUIID(void)
{
	appdis.pUI->screen_id = appdis.pUI->screen_idbk;
//	if(LIFOBuffer_Pop(&ScreenIDLIFO, (INT8U *)&appdis.pUI->screen_id)==1)	
	{//获取
		appdis.pUI->ctrl_id = appdis.pUI->ctrl_idbk;
		DaCai_SwitchUI(appdis.pUI);
	}
	if(appdis.pUI->screen_id==Lab_UIID||appdis.pUI->screen_id==Menu_UIID)	{
		appdis.pUI->button_id = 19;
		if(Sys.devstate == DevState_Running)	{		
			DaCai_ButtonCtrl(appdis.pUI, DEF_Press);
		}
		else	{
			DaCai_ButtonCtrl(appdis.pUI, DEF_Release);
		}
	}
	else if(appdis.pUI->screen_id==Temp_UIID)	{
		DisplayTempProgramUI(0,0);
	}
}

void DisplayEditUI(void)
{
	appdis.pUI->screen_id = appdis.pUI->editinfo.screen_id;
	appdis.pUI->ctrl_id = appdis.pUI->editinfo.ctrl_id;
	DaCai_SwitchUI(appdis.pUI);
}

void DisplayKeyboardUI(void)
{
//	s8 id;
	DisplayUIIDAndBackup(Keyboard_UIID);
//	id = appdis.pUI->ctrl_id;
	appdis.pUI->ctrl_id = 43;
	DaCai_ClearTXT(appdis.pUI);
}

void DisplayMessageUI(char *pbuf)
{	
	DisplayUIIDAndBackup(Message_UIID);
//	appdis.pUI->sub_screen_id = Message_UIID;
//	DaCai_SwitchSubUI(appdis.pUI);
	appdis.pUI->ctrl_id  = 4;	
	appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", pbuf);
	DaCai_UpdateTXT(appdis.pUI);
}

void DisplayWarningUI(char *pbuf, char *buf1, char *buf2)
{	
	DisplayUIIDAndBackup(Confirm_UIID);
//	UUIDBackup();
//	appdis.pUI->screen_id = Confirm_UIID;
//	DaCai_SwitchUI(appdis.pUI);
	appdis.pUI->ctrl_id  = 4;	
	appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", pbuf);
	DaCai_UpdateTXT(appdis.pUI);
//	if(buf1!=NULL)	{	
		appdis.pUI->ctrl_id  = 1;	
		appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", buf1);
		DaCai_UpdateTXT(appdis.pUI);
//	}
//	if(buf2!=NULL)	{
		appdis.pUI->ctrl_id  = 5;	
		appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", buf2);
		DaCai_UpdateTXT(appdis.pUI);
//	}
}

void DisplayLogUI(void)
{
	u32 ret;

	appdis.pUI->screen_id = Log_UIID;//LOG界面					
	DaCai_SwitchUI(appdis.pUI);
	ret = read_log(appdis.pUI->pdata);
	if(ret)	{//读取log长度不为0
		appdis.pUI->ctrl_id = 3;
		appdis.pUI->datlen = ret;
		DaCai_UpdateTXT(appdis.pUI);
	}
}

void DisplayLabUI(void)
{
	appdis.pUI->screen_id = Lab_UIID;
	DaCai_SwitchUI(appdis.pUI);
	appdis.pUI->button_id = 19;
	if(Sys.devstate == DevState_Running)	{		
		DaCai_ButtonCtrl(appdis.pUI, DEF_Press);
	}
	else	{
		DaCai_ButtonCtrl(appdis.pUI, DEF_Release);
	}
}

void DisplayMenuUI(void)
{
	appdis.pUI->screen_id = Menu_UIID;
	DaCai_SwitchUI(appdis.pUI);
	appdis.pUI->button_id = 19;
	if(Sys.devstate == DevState_Running)	{		
		DaCai_ButtonCtrl(appdis.pUI, DEF_Press);
	}
	else	{
		DaCai_ButtonCtrl(appdis.pUI, DEF_Release);
	}
}

void DisplayHeatCoverIcon(void)
{
	appdis.pUI->ctrl_id = 8;
	DaCai_IconCtrl(appdis.pUI,temp_data.HeatCoverEnable);
}

void DisplayQiTingLab(void)
{	
//	DisplayUIIDAndBackup(Confirm_UIID);//备份当前界面 切换到下一个界面
	if(Sys.devstate == DevState_Running)	{
		DisplayWarningUI((char *)&Code_Warning[2][0], (char *)&Code_Choose[0][0], (char *)&Code_Choose[1][0]);
//		appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", &Code_Warning[2][0]);//显示 是否停止
		Sys.state |= SysState_StopTB;
	}
	else if(Sys.devstate == DevState_IDLE)		{		
//		appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", &Code_Warning[1][0]);//显示 是否启动
		DisplayWarningUI((char *)&Code_Warning[1][0], (char *)&Code_Choose[0][0], (char *)&Code_Choose[1][0]);		
		Sys.state |= SysState_RunningTB;
	}
}

void DisplayStepUI(s8 stageid, s8 stepid)
{
	s8 m,n,i;

	pMultiTXT_t = (_MultiTXT_ *)tlsf_malloc(UserMem, sizeof(_MultiTXT_));
	appdis.pUI->screen_id = Step_UIID;//step界面					
	DaCai_SwitchUI(appdis.pUI);
//	if(temp_data.StageNum>0)
	{
//		m = stageid-1;
//		if(m<0)	m = 0;
//		n = stageid-1;
//		if(n<0) n = 0;
		m = stageid;
		n = stepid;
		i=0;
	//	pMultiTXT_t->data[i].id = 26;
	//	pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "Step", temp_data.stage[m].StepNum);
		pMultiTXT_t->data[++i].id = 8;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d.%d", temp_data.stage[m].step[n].temp/10,temp_data.stage[m].step[n].temp%10);
		pMultiTXT_t->data[++i].id = 9;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d", temp_data.stage[m].step[n].tim/60);
		pMultiTXT_t->data[++i].id = 10;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d", temp_data.stage[m].step[n].tim%60);
		DaCai_UpdateMultiTXT(appdis.pUI, pMultiTXT_t->data, i+1);
	}
	appdis.pUI->button_id = 1;
	DaCai_ButtonCtrl(appdis.pUI, temp_data.stage[m].step[n].CollEnable);
	tlsf_free(UserMem, pMultiTXT_t);
}

void DisplayStageUI(void)
{
	u8 i,j;
	
	appdis.pUI->screen_id = Stage_UIID;//stage界面					
	DaCai_SwitchUI(appdis.pUI);
	if(temp_data.StageNum>0)
		j = temp_data.StageNum-1;
	else
		j = 0;
	i=0;
	pMultiTXT_t = (_MultiTXT_ *)tlsf_malloc(UserMem, sizeof(_MultiTXT_));
//	pMultiTXT_t->data[i].id = 26;
//	pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "Stage%d", temp_data.StageNum);
	pMultiTXT_t->data[++i].id = 8;
	pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d", temp_data.stage[j].StepNum);
	pMultiTXT_t->data[++i].id = 4;
	pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d", temp_data.stage[j].RepeatNum);
	DaCai_UpdateMultiTXT(appdis.pUI, pMultiTXT_t->data, i+1);
	tlsf_free(UserMem, pMultiTXT_t);
}
#define TEMP_RECTANGLE_X	17
#define TEMP_RECTANGLE_X_INTER	154
#define TEMP_RECTANGLE_Y	142
#define TEMP_RECTANGLE_W	150
#define TEMP_RECTANGLE_H	200
#define TEMP_UI_MAXSTEP		5
#include "DaCai_TouchEvent.h"

static void ClearTempProgramUI(u8 clear_flag)
{
	u8 j;
	
	if(clear_flag==0)
		return;
	for(j=0;j<9;j++)	{
		appdis.pUI->ctrl_id = 15+j;
		DaCai_ClearTXT(appdis.pUI);
		appdis.pUI->ctrl_id = 24+j;
		DaCai_ClearTXT(appdis.pUI);
	}
//	for(j=0;j<TEMP_UI_MAXSTEP;j++)	{//清文字 图片
//		rec_x = TEMP_RECTANGLE_X + TEMP_RECTANGLE_X_INTER*j;	
//		DaCai_DisplayCutPic(rec_x, TEMP_RECTANGLE_Y, 59, 0, 0, TEMP_RECTANGLE_W, TEMP_RECTANGLE_H);
//		TempButtonState[j] = 0;
//		appdis.pUI->ctrl_id = 10 + j;
//		DaCai_IconCtrl(appdis.pUI, TempButtonState[j]);
//	}
}
struct _dis_idx_t	{
	u8 StageIdx;
	u8 StepIdx;
 }CurIdx,LastIdx;
static u16 g_templast;
void ClearTempProgramIdx(void)
{
	CurIdx.StageIdx = 0;
	CurIdx.StepIdx = 0;
	LastIdx.StageIdx = 0;
	LastIdx.StepIdx = 0;
	g_templast = 30;
}

//按照按钮id 查找当前是那个stage step
u8 CheckIdFromButton(u8 button, u8 *stageid, u8 *stepid)
{
	s8 i,j;
	u8 flag;
	u8 Stageid,Stepid,stepcnt;
	
	Stageid = 0;
	Stepid = 0;
	flag = 0;
	stepcnt = 0;
	j = CurIdx.StepIdx;
	for(i=CurIdx.StageIdx;i<temp_data.StageNum;i++)	{//根据按钮id 找到要删除的stage id和step id
		for(;j<temp_data.stage[i].StepNum;j++)	{
			if(button==stepcnt)	{
				flag=1;
				Stageid = i;
				Stepid = j;
				break;
			}
			stepcnt ++;
			if(stepcnt>=TEMP_UI_MAXSTEP)	{
				return 0;
			}
		}
		if(flag)	{
			*stageid = Stageid;
			*stepid = Stepid;
			return 1;
		}
		j=0;
	}
	return 0;
}
//删除操作 type=0 删阶段，type=1 删除步
void DeleTempProgam(u8 button, u8 type)
{
	s8 i,j,k;
	u8 Stageid,Stepid;

	if(CheckIdFromButton(button, &Stageid, &Stepid)==1)
	{
		if(type==0)	{//删阶
			DelStage(Stageid);
		}
		else if(type==1)	{//删步
			i = Stageid;
			j = temp_data.stage[i].StepNum-1;
			if(j==0)	{//该阶段无步 直接删除该阶段
				DelStage(Stageid);
				temp_data.stage[i].StepNum = j;				
			}
			else if(j>0)	{
				for(k=Stepid;k<j;k++)	{
					memcpy(&temp_data.stage[i].step[k], &temp_data.stage[i].step[k+1], sizeof(_step_t));
				}
				temp_data.stage[i].StepNum = j;
				ResetStep(i,k);
			}
		}
	}
	ClearTempProgramIdx();		
	DisplayTempProgramUI(0,1);
}

//温度程序 界面刷新
void DisplayTempProgramUI(u8 page_flag, u8 clear_flag)
{
	u8 i,j,k,stepcnt;
	u8 stage_id,repeat_id;
	u16 rec_x,rec_y,height,rec_y_last;
	u16 xie_w,xie_h;
	u16 temp,templast;
	u8 flag;
	
	appdis.pUI->screen_id = Temp_UIID;
	DaCai_SwitchUI(appdis.pUI);
	
//	DaCai_DisplayUpdateOnOff(DEF_Disable);
	DisplayHeatCoverIcon();	

	ClearTempProgramUI(clear_flag);
	pMultiTXT_t = (_MultiTXT_ *)tlsf_malloc(UserMem, sizeof(_MultiTXT_));
	i=0;
	if(page_flag)	{
		CurIdx.StageIdx = LastIdx.StageIdx;
		CurIdx.StepIdx = LastIdx.StepIdx;
	}
	pMultiTXT_t->data[i].id = 6;
	pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d", temp_data.HeatCoverTemp);

	stage_id = 14;repeat_id = 23;
	j = CurIdx.StageIdx;
	flag = temp_data.stage[j].StepNum - CurIdx.StepIdx;
	for(;temp_data.StageNum>0;)	{
		stage_id += flag;
		pMultiTXT_t->data[++i].id = stage_id;
		stage_id += flag;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "Stage%d", j+1);
		repeat_id += flag;
		pMultiTXT_t->data[++i].id = repeat_id;
		repeat_id += flag;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "x%d", temp_data.stage[j].RepeatNum);
		if(repeat_id>=32)
			break;		
		j++;
		if(j>=temp_data.StageNum)
			break;
		flag = temp_data.stage[j].StepNum;
	}
	DaCai_UpdateMultiTXT(appdis.pUI, pMultiTXT_t->data, i+1);
	tlsf_free(UserMem, pMultiTXT_t);
	templast = g_templast;
	stepcnt = 0;
	flag = 0;
	k=CurIdx.StepIdx;
	for(j=CurIdx.StageIdx;j<temp_data.StageNum;j++)	{//刷新图形
		for(;k<temp_data.stage[j].StepNum;k++)	{
			if(stepcnt==TEMP_UI_MAXSTEP-1)	{//显示已满 
				flag = 1;//需要翻到第1页
			}
			else if(stepcnt>=TEMP_UI_MAXSTEP)	{
				g_templast = temp;
				LastIdx.StageIdx = j;
				if(k>=temp_data.stage[j].StepNum)
					LastIdx.StepIdx = 0;
				else
					LastIdx.StepIdx = k;
				return;
			}
			temp = temp_data.stage[j].step[k].temp/10;			
			rec_x = TEMP_RECTANGLE_X + TEMP_RECTANGLE_X_INTER*stepcnt;
			height = temp*(TEMP_RECTANGLE_H/100);
			rec_y = TEMP_RECTANGLE_Y+TEMP_RECTANGLE_H-height;
			if(temp>templast)	{		
				DaCai_DisplayCutPic(rec_x, rec_y, 60, 0, 0, TEMP_RECTANGLE_W, height);
				xie_h = (temp - templast)*(TEMP_RECTANGLE_H/100);
				xie_w = xie_h/(TEMP_RECTANGLE_H/50);
				DaCai_DisplayCutPic(rec_x, rec_y, 59, 50-xie_w, 0, xie_w, xie_h);
			}
			else	if(temp<templast)	{
				DaCai_DisplayCutPic(rec_x, rec_y, 60, 0, 0, TEMP_RECTANGLE_W, height);
//				height = templast*(TEMP_RECTANGLE_H/100);
				rec_y_last = TEMP_RECTANGLE_Y+TEMP_RECTANGLE_H - templast*(TEMP_RECTANGLE_H/100);
				xie_h = (templast-temp)*(TEMP_RECTANGLE_H/100);
				xie_w = xie_h/(TEMP_RECTANGLE_H/50);
				DaCai_DisplayCutPic(rec_x, rec_y_last, 58, 0, 0, xie_w, xie_h);
			}
			else	{
				DaCai_DisplayCutPic(rec_x, rec_y, 60, 0, 0, TEMP_RECTANGLE_W, height);
			}
			DaCai_SetFontColor(65504);
			appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%d.%d℃", temp, temp_data.stage[j].step[k].temp%10);
			DaCai_DisplayTXT(appdis.pUI, rec_x+60, rec_y-18, 6);
			appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%d:%d", temp_data.stage[j].step[k].tim/60, temp_data.stage[j].step[k].tim%60);
			DaCai_DisplayTXT(appdis.pUI, rec_x+60, rec_y+5, 6);
			if(temp_data.stage[j].step[k].CollEnable)
				PaintTriangle(rec_x+130,rec_y,14,30);
			templast = temp;
			stepcnt	++;
		}
		k=0;
	}
//	DaCai_DisplayUpdateOnOff(DEF_Enable);
	if(flag)	{
		g_templast = temp;
		LastIdx.StageIdx = j;
		if(k>=temp_data.stage[j].StepNum)
			LastIdx.StepIdx = 0;
		else
			LastIdx.StepIdx = k;
	}
}
//画三角形 (x,y)是顶点坐标 w,h是三角形宽和高
void PaintTriangle(u16 x, u16 y, u16 w, u16 h)
{
	_coordinate_t coo[4];
	u16 w2,h2;
	u8 i;
	
	w2 = w/2;
	h2 = h/2;
	for(i=0;i<2;i++)	{
		coo[0].x = x+i;
		coo[0].y = y+i;
		coo[1].x = x-w2+i;
		coo[1].y = y+h2+i;
		coo[2].x = x+w2+i;
		coo[2].y = y+h2+i;
		coo[3].x = x+i;
		coo[3].y = y+i;
		DaCai_PaintLine(coo, 4);
	}
}


