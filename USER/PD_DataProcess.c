#include "PD_DataProcess.h"
#include "motor.h"

#define BUFSIZE         64
#define	HOLE_POSITION_MIN_OFFSET	(u32)(1.5*Motor_StepsPerum)	//
#define	HOLE_POSITION_MAX_OFFSET	(u32)(1.3*Motor_StepsPerum)	//
#define	HOLE_POSITION_DISTANCE	(u32)(4.5*Motor_StepsPerum)	//

_pd_data_t gPD_Data;
u16 PD_max;
u8 HolePositionCaliFlag;

typedef struct _pd_buf	{
	u8 idx;
	u16 buf[BUFSIZE];
}_pd_buf_t;

_pd_buf_t pdbuf;
_pd_buf_t *p_pdbuf;


void PD_DataInit(void)
{
	gPD_Data.coll_enable = DEF_False;
	gPD_Data.ch = LED_NONE;
	memset((u8 *)gPD_Data.PDBaseBlue, 0, sizeof(gPD_Data.PDBaseBlue));
	memset((u8 *)gPD_Data.PDBaseGreen, 0, sizeof(gPD_Data.PDBaseGreen));
	memset(&pdbuf, 0, sizeof(pdbuf));
//	memset(&pdbuf[1], 0, sizeof(pdbuf[1]));
	p_pdbuf = &pdbuf;
	memset((u8 *)gPD_Data.PDVol, 0, sizeof(gPD_Data.PDVol));
	PD_max = 0x00;
}

//开启孔位置校准 使用蓝光LED扫描
void StartCaliHolePosition(void)
{
	gPD_Data.ch = LED_BLUE;
	FluoLED_OnOff(LED_BLUE, DEF_ON);
	PD_max = 0;	
	HolePositionCaliFlag = 0;
	Sys.state |= SysState_CaliHolePostion;//孔位置校准
	gPD_Data.coll_enable = DEF_True;
}
void StopCaliHolePosition(void)
{	
	Sys.state &= ~SysState_CaliHolePostion;//校准结束
	gPD_Data.coll_enable = DEF_False;
	gPD_Data.ch = LED_NONE;
	FluoLED_OnOff(LED_BLUE, DEF_OFF);
}
//计算孔位置 已知第一个孔位置 其它孔位置按照孔间距4.5mm推算
void CalcHolePositon(void)
{
	u8 i;
	s16 temp;
	
	temp = HolePos.pos[0].x1;
	HolePos.pos[0].x1 = temp - HOLE_POSITION_MIN_OFFSET;
	HolePos.pos[0].x2 = temp + HOLE_POSITION_MAX_OFFSET;
	for(i=1;i<HOLE_NUM;i++)	{
		HolePos.pos[i].x1 = HolePos.pos[i-1].x1 + HOLE_POSITION_DISTANCE;
		HolePos.pos[i].x2 =  HolePos.pos[i-1].x2 + HOLE_POSITION_DISTANCE;
	}
}

//开启空孔PD本底信号校准 使用蓝光LED扫描
void StartCaliHolePDBase(void)
{	
	memset((u8 *)gPD_Data.PDBaseBlue, 0, sizeof(gPD_Data.PDBaseBlue));
	memset((u8 *)gPD_Data.PDBaseGreen, 0, sizeof(gPD_Data.PDBaseGreen));
	StartCollPDData();//孔PD数据采集准备
}
void StopCaliHolePDBase(void)
{	
	StopCollPDData();
}
//计算孔本底信号
void CalcHolePDBase(u8 flag)
{
	if(flag==LED_BLUE)
		memcpy((u8 *)gPD_Data.PDBaseBlue, (u8 *)gPD_Data.PDVol, sizeof(gPD_Data.PDBaseBlue));
	else if(flag==LED_GREEN)
		memcpy((u8 *)gPD_Data.PDBaseGreen, (u8 *)gPD_Data.PDVol, sizeof(gPD_Data.PDBaseGreen));
}

//PD数据采集
void PD_DataCollect(u16 ad_vol, u8 pd_ch)
{	
	if(gPD_Data.coll_enable==DEF_False)//是否采集数据
		return;
	if(pd_ch != gPD_Data.ch)
		return;
	
	if(Sys.state & SysState_CollHolePD)	{//采集孔PD有效值
		if(p_pdbuf->idx<BUFSIZE)	{
			p_pdbuf->buf[p_pdbuf->idx] = ad_vol;
			p_pdbuf->idx++;
		}
	}
	else if(Sys.state & SysState_CaliHolePostion)	{//孔位置校准 找到第一个孔位置 其它孔位置按照孔间距4.5mm推算
		if(ad_vol <= 450)	{
			HolePositionCaliFlag = 1;
		}
		else if(HolePositionCaliFlag == 1)	{	//找到第一个孔的最大值 
			if(ad_vol>PD_max)	{
				PD_max = ad_vol;
				HolePos.pos[0].x1 = tMotor[MOTOR_ID1].CurSteps;
			}
		}
	}
}
//准备采集实验PD数据 采用双缓存
void StartCollPDData(void)
{
	p_pdbuf = &pdbuf;
	p_pdbuf->idx = 0;
	Sys.state |= SysState_CollHolePD;
	gPD_Data.coll_enable = DEF_False;
//	gPD_Data.DataValid = DEF_False;
}
void StopCollPDData(void)
{
	Sys.state &= ~SysState_CollHolePD;//采集结束
	gPD_Data.coll_enable = DEF_False;
	gPD_Data.ch = LED_NONE;
	FluoLED_OnOff(LED_BLUE|LED_GREEN, DEF_OFF);
	
}

//u16 xxxx;
//根据采集到的PD值 计算孔PD均值
#define PDMaxFont		2
#define PDMaxBack		2
void CalcPDData(u8 hole_idx)
{
	u8 i,max_idx;
	s16 m,n;
	u16 max;
	u32 temp;

	max = p_pdbuf->buf[0];
	max_idx = 0;
	for(i=0;i<p_pdbuf->idx;i++)//先找到最大值 再将最大值前5个 后5个去平均 得到PD均值
    {
        if(p_pdbuf->buf[i] > max)
        {
            max = p_pdbuf->buf[i];
            max_idx = i;
        }
	}	
	m=0;n=0;
	n = max_idx + PDMaxBack;
	if(n>p_pdbuf->idx)
		n = p_pdbuf->idx;
	m = max_idx - PDMaxFont;
	if(m<0)
		m = 0;
	temp = 0;
	for(i=m;i<n;i++)	{
		temp += p_pdbuf->buf[i];
	}
	temp /= (i-m);
	gPD_Data.PDVol[hole_idx] = (u16)temp;//当前孔PD有效值
//	xxxx = p_pdbuf->idx;
	p_pdbuf->idx = 0;
}

