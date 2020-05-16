#include "PD_DataProcess.h"

#define BUFSIZE         64

_pd_data_t gPD_Data;
_pd_maxmin_t templatehole;

typedef struct _pd_buf	{
	u8 idx;
	u16 buf[BUFSIZE];
}_pd_buf_t;

_pd_buf_t pdbuf[2];
_pd_buf_t *p_pdbuf,*p_pdbuf_bk;


void PD_DataInit(void)
{
	gPD_Data.coll_enable = DEF_False;
	gPD_Data.ch = LED_NONE;
	memset(&templatehole, 0, sizeof(templatehole));
	p_pdbuf = &pdbuf[0];
	p_pdbuf->idx = 0;
	memset(gPD_Data.PDVol, 0, HOLE_NUM);
}

//开启空孔PD值采集 使用蓝光LED扫描
void StartCollTemplateHolePD(void)
{	
	gPD_Data.ch = LED_BLUE;
	FluoLED_OnOff(LED_BLUE, DEF_ON);
	Sys.state |= SysState_CollTemplateHolePD;//校准空孔PD值
	memset(&templatehole, 0, sizeof(templatehole));
	templatehole.min[0] = 0xffff;
	templatehole.min[1] = 0xffff;
	gPD_Data.coll_enable = DEF_False;
}
void StopCollTemplateHolePD(void)
{	
	Sys.state &= ~SysState_CollTemplateHolePD;//校准结束
	gPD_Data.coll_enable = DEF_False;
	FluoLED_OnOff(LED_BLUE, DEF_OFF);	
}

//计算空孔PD均值
void CalcTemplateHolePDAver(void)
{
	u32 temp;
	
	temp = templatehole.min[0] + templatehole.min[1] + templatehole.max[0] + templatehole.max[1];
	templatehole.aver = temp/4;//计算空孔PD均值
	temp = templatehole.aver*15;//计算有效孔位置PD阀值
	gPD_Data.HoleThreshold = temp/10;
}

#include "motor.h"
u8 HolePositionCaliFlag;
//PD数据采集
void PD_DataCollect(u16 ad_vol, u8 pd_ch)
{
	u8 idx;
	if(gPD_Data.coll_enable==DEF_False)//是否采集数据
		return;
	if(pd_ch != gPD_Data.ch)
		return;
	if(Sys.state & SysState_CollHolePD)	{//采集孔PD值
		if(p_pdbuf->idx<BUFSIZE)	{
			p_pdbuf->buf[p_pdbuf->idx] = ad_vol;
			p_pdbuf->idx++;
		}
	}
	else if(Sys.state & SysState_CollTemplateHolePD)	{//采集空孔PD最大值 最小值		
		idx = templatehole.idx;
		if(ad_vol>templatehole.max[idx])	//最大值
			templatehole.max[idx] = ad_vol;
		else if(ad_vol<templatehole.min[idx])//最小值
			templatehole.min[idx] = ad_vol;
	}
	else if(Sys.state & SysState_CaliHolePostion)	{//孔位置校准
		idx = HolePos.idx;
//		if(ad_vol < gPD_Data.HoleThreshold)	{
		if(ad_vol <= templatehole.aver)	{
			if(HolePositionCaliFlag == 0)	{
				HolePositionCaliFlag = 1;
			}
			else if(HolePositionCaliFlag == 2)	{
				HolePos.pos[idx].x2 = tMotor[MOTOR_ID1].CurSteps;
				HolePositionCaliFlag = 0;
				HolePos.idx ++;
			}
		}
		else if(ad_vol >= gPD_Data.HoleThreshold)	{
			if(HolePositionCaliFlag == 1)	{
				HolePos.pos[idx].x1 = tMotor[MOTOR_ID1].CurSteps;
				HolePositionCaliFlag = 2;	
			}				
		}
	}
}
//准备采集实验PD数据 采用双缓存
void ReadyToCollPD_LabData(void)
{
	p_pdbuf = &pdbuf[0];
	p_pdbuf->idx = 0;
//	memset(gPD_Data.PDVol, 0, HOLE_NUM);
}
//交换缓冲地址
void ExchangePDBuf(void)
{
	p_pdbuf_bk = p_pdbuf;
	if(p_pdbuf == &pdbuf[0])
		p_pdbuf = &pdbuf[1];
	else
		p_pdbuf = &pdbuf[0];
	p_pdbuf->idx = 0;
}
//根据采集到的PD值 计算孔PD均值
#define PDMaxFont		5
#define PDMaxBack		5
void CalcPDData(u8 hole_idx)
{
	u8 i,max_idx,m,n;
	u16 max;
	u32 temp;
	
	max = p_pdbuf_bk->buf[0];
	for(i=0;i<p_pdbuf_bk->idx;i++)//先找到最大值 再将最大值前5个 后5个去平均 得到PD均值
    {
        if(p_pdbuf_bk->buf[i] > max)
        {
            max = p_pdbuf_bk->buf[i];
            max_idx = i;
        }
	}	
	n = max_idx + PDMaxBack;
	m = max_idx - PDMaxFont;
	temp = 0;
	for(i=m;i<n;i++)	{
		temp += p_pdbuf_bk->buf[i];
	}
	temp /= (PDMaxFont + PDMaxBack);
	gPD_Data.PDVol[hole_idx] = (u16)temp;//当前孔PD值
}

