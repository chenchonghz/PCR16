#include "display_code.h"
#include "rw_spiflash.h"

typedef struct 	{
	_MultiTxtDat data[8];
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

void DisplaMenuUI(void)
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

void DisplayQiTingLab(void)
{	
	DisplayUIIDAndBackup(Confirm_UIID);//备份当前界面 切换到下一个界面
	appdis.pUI->ctrl_id = 4;
	if(Sys.devstate == DevState_Running)	{
//		DisplayMessageUI((char *)&Code_Message[5][0]);
		appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", &Code_Warning[2][0]);//显示 是否停止
		Sys.state |= SysState_StopTB;
	}
	else if(Sys.devstate == DevState_IDLE)		{		
		appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%s", &Code_Warning[1][0]);//显示 是否启动		
		Sys.state |= SysState_RunningTB;
	}
	DaCai_UpdateTXT(appdis.pUI);
}

void DisplayStepUI(void)
{
	s8 m,n,i;

	pMultiTXT_t = (_MultiTXT_ *)tlsf_malloc(UserMem, sizeof(_MultiTXT_));
	appdis.pUI->screen_id = Step_UIID;//step界面					
	DaCai_SwitchUI(appdis.pUI);
//	if(temp_data.StageNum>0)
	{
		m = temp_data.StageNum-1;
		if(m<0)	m = 0;
		n = temp_data.stage[m].StepNum-1;
		if(n<0) n = 0;
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

void ClearTempProgramUI(u8 start)
{
	u8 j;
	u16 rec_x;
	
	if(start==0xff)
		return;
	for(j=start;j<9;j++)	{
		appdis.pUI->ctrl_id = 15+j;
		DaCai_ClearTXT(appdis.pUI);
		appdis.pUI->ctrl_id = 24+j;
		DaCai_ClearTXT(appdis.pUI);
	}
	for(j=start;j<TEMP_UI_MAXSTEP;j++)	{//清文字 图片
		rec_x = TEMP_RECTANGLE_X + TEMP_RECTANGLE_X_INTER*j;	
		DaCai_DisplayCutPic(rec_x, TEMP_RECTANGLE_Y, 58, 0, 0, TEMP_RECTANGLE_W, TEMP_RECTANGLE_H);	
	}
}
//温度程序 界面刷新
void DisplayTempProgramUI(u8 clear_s)
{
	u8 i,j,k,stepcnt;
	u8 stage_id,repeat_id;
	u16 rec_x,rec_y,height;
	u16 xie_w,xie_h;
	u16 temp,templast;
	
	appdis.pUI->screen_id = Temp_UIID;
	DaCai_SwitchUI(appdis.pUI);
	appdis.pUI->button_id = 8;
	DaCai_ButtonCtrl(appdis.pUI, temp_data.HeatCoverEnable);

	ClearTempProgramUI(clear_s);
	pMultiTXT_t = (_MultiTXT_ *)tlsf_malloc(UserMem, sizeof(_MultiTXT_));
	i=0;
	pMultiTXT_t->data[i].id = 6;
	pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "%d", temp_data.HeatCoverTemp);

	stage_id = 14;repeat_id = 23;
	for(j=0;j<temp_data.StageNum;j++)	{//刷新文字
		stage_id += temp_data.stage[j].StepNum;
		pMultiTXT_t->data[++i].id = stage_id;
		stage_id += temp_data.stage[j].StepNum;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "Stage%d", j+1);
		repeat_id += temp_data.stage[j].StepNum;
		pMultiTXT_t->data[++i].id = repeat_id;
		repeat_id += temp_data.stage[j].StepNum;
		pMultiTXT_t->data[i].len = sprintf(pMultiTXT_t->data[i].buf, "x%d", temp_data.stage[j].RepeatNum);
		if(repeat_id>=32)
			break;
	}
	DaCai_UpdateMultiTXT(appdis.pUI, pMultiTXT_t->data, i+1);
	tlsf_free(UserMem, pMultiTXT_t);
	templast = 30;
	stepcnt = 0;
	for(j=0;j<temp_data.StageNum;j++)	{//刷新图形
		for(k=0;k<temp_data.stage[j].StepNum;k++)	{	
			if(stepcnt>=TEMP_UI_MAXSTEP)	{
				return;
			}
			temp = temp_data.stage[j].step[k].temp/10;
			height = temp*(TEMP_RECTANGLE_H/100);
			rec_x = TEMP_RECTANGLE_X + TEMP_RECTANGLE_X_INTER*stepcnt;			
			DaCai_DisplayCutPic(rec_x, TEMP_RECTANGLE_Y, 58, 0, 0, TEMP_RECTANGLE_W, TEMP_RECTANGLE_H);			
			
			if(temp>templast)	{
				rec_y = TEMP_RECTANGLE_Y+TEMP_RECTANGLE_H-height;
				DaCai_DisplayCutPic(rec_x, rec_y, 57, 0, 0, TEMP_RECTANGLE_W, height);
				xie_h = TEMP_RECTANGLE_H - templast*(TEMP_RECTANGLE_H/100);
				xie_w = xie_h/(TEMP_RECTANGLE_H/50);
				DaCai_DisplayCutPic(rec_x, rec_y, 56, 50-xie_w, 0, xie_w, xie_h);
			}
			else	if(temp<templast)	{
				xie_h = (templast-temp)*(TEMP_RECTANGLE_H/100);
				xie_w = xie_h/(TEMP_RECTANGLE_H/50);
				DaCai_DisplayCutPic(rec_x, rec_y, 55, 0, 0, xie_w, xie_h);
				rec_y = TEMP_RECTANGLE_Y+TEMP_RECTANGLE_H-height;
				DaCai_DisplayCutPic(rec_x, rec_y, 57, 0, 0, TEMP_RECTANGLE_W, height);
			}
			else	{
				DaCai_DisplayCutPic(rec_x, rec_y, 57, 0, 0, TEMP_RECTANGLE_W, height);
			}
			appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%d.%d℃", temp, temp_data.stage[j].step[k].temp%10);
			DaCai_DisplayTXT(appdis.pUI, rec_x+60, rec_y-18, 6);
			appdis.pUI->datlen = sprintf((char *)appdis.pUI->pdata,"%d:%d", temp_data.stage[j].step[k].tim/60, temp_data.stage[j].step[k].tim%60);
			DaCai_DisplayTXT(appdis.pUI, rec_x+60, rec_y+5, 6);
			if(temp_data.stage[j].step[k].CollEnable)
				PaintTriangle(rec_x+130,rec_y,14,30);
			templast = temp;
			stepcnt	++;
		}
	}
}
//画三角形 (x,y)是顶点坐标 w,h是三角形宽和高
void PaintTriangle(u16 x, u16 y, u16 w, u16 h)
{
	_coordinate_t coo[4];
	u16 w2,h2;
	
	w2 = w/2;
	h2 = h/2;
	coo[0].x = x;
	coo[0].y = y;
	coo[1].x = x-w2;
	coo[1].y = y+h2;
	coo[2].x = x+w2;
	coo[2].y = y+h2;
	coo[3].x = x;
	coo[3].y = y;
	DaCai_PaintLine(coo, 4);
}

