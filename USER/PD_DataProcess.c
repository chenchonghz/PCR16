#include "PD_DataProcess.h"
#include "motor.h"

#define BUFSIZE         64
#define	HOLE_POSITION_MIN_OFFSET	(u32)(1*Motor_StepsPerum)	//
#define	HOLE_POSITION_MAX_OFFSET	(u32)(1*Motor_StepsPerum)	//
#define	HOLE_POSITION_DISTANCE	(u32)(4.5*Motor_StepsPerum)	//

_pd_data_t gPD_Data;
u16 PD_max;

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
	memset(gPD_Data.PDBase, 0, sizeof(gPD_Data.PDBase));
	p_pdbuf = &pdbuf[0];
	p_pdbuf->idx = 0;
	memset(gPD_Data.PDVol, 0, HOLE_NUM);
	PD_max = 0x00;
}

//开启孔位置校准 使用蓝光LED扫描
void StartCaliHolePosition(void)
{
	gPD_Data.ch = LED_BLUE;
	FluoLED_OnOff(LED_BLUE, DEF_ON);
	PD_max = 0;	
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
	
	HolePos.pos[0].x1 -= HOLE_POSITION_MIN_OFFSET;
	HolePos.pos[0].x2 =  HolePos.pos[0].x1 + HOLE_POSITION_MAX_OFFSET;
	for(i=1;i<HOLE_NUM;i++)	{
		HolePos.pos[i].x1 = HolePos.pos[i-1].x1 + HOLE_POSITION_DISTANCE;
		HolePos.pos[i].x2 =  HolePos.pos[i-1].x2 + HOLE_POSITION_DISTANCE;
	}
}

//开启空孔PD本底信号校准 使用蓝光LED扫描
void StartCaliHolePDBase(void)
{	
	gPD_Data.ch = LED_BLUE;
	FluoLED_OnOff(LED_BLUE, DEF_ON);
	memset(gPD_Data.PDBase, 0, sizeof(gPD_Data.PDBase));
	StartCollPDData();//孔PD数据采集准备
}
void StopCaliHolePDBase(void)
{	
	StopCollPDData();
}
//计算孔本底信号
void CalcHolePDBase(void)
{
	memcpy(gPD_Data.PDBase, gPD_Data.PDVol, HOLE_NUM);
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
		if(ad_vol>PD_max)	{	//找到第一个孔的最大值 
			PD_max = ad_vol;
			HolePos.pos[0].x1 = tMotor[MOTOR_ID1].CurSteps;
		}
	}
}
//准备采集实验PD数据 采用双缓存
void StartCollPDData(void)
{
	p_pdbuf = &pdbuf[0];
	p_pdbuf->idx = 0;
	Sys.state |= SysState_CollHolePD;
	gPD_Data.coll_enable = DEF_False;
}
void StopCollPDData(void)
{
	Sys.state &= ~SysState_CollHolePD;//采集结束
	gPD_Data.coll_enable = DEF_False;
	gPD_Data.ch = LED_NONE;
	FluoLED_OnOff(LED_BLUE|LED_GREEN, DEF_OFF);
	
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
	gPD_Data.PDVol[hole_idx] = (u16)temp;//当前孔PD有效值
}

