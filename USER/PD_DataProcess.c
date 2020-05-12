#include "PD_DataProcess.h"

_pd_data_t gPD_Data;

void PD_DataInit(void)
{
	gPD_Data.coll_enable = DEF_False;
	gPD_Data.ch = LED_NONE;
	memset(&gPD_Data.templatehole, 0, sizeof(gPD_Data.templatehole));
}

//开启空孔PD值采集 使用蓝光LED
void StartCollTemplateHolePD(void)
{	
	gPD_Data.ch = LED_BLUE;
	FluoLED_OnOff(LED_BLUE, DEF_ON);
	Sys.state |= SysState_CaliTemplateHoleFluo;//校准空孔PD值
	memset(&gPD_Data.templatehole, 0, sizeof(gPD_Data.templatehole));
	gPD_Data.templatehole.min[0] = 0xffff;
	gPD_Data.templatehole.min[1] = 0xffff;
	gPD_Data.coll_enable = DEF_False;
}
void StopCollTemplateHolePD(void)
{	
	Sys.state &= ~SysState_CaliTemplateHoleFluo;//校准结束
	gPD_Data.coll_enable = DEF_False;
	FluoLED_OnOff(LED_BLUE, DEF_OFF);	
}

//计算空孔PD均值
void CalcTemplateHolePDAver(void)
{
	u32 temp;
	
	temp = gPD_Data.templatehole.min[0] + gPD_Data.templatehole.min[1] + gPD_Data.templatehole.max[0] + gPD_Data.templatehole.max[1];
	gPD_Data.templatehole.aver = temp/4;//计算空孔PD均值
	temp = gPD_Data.templatehole.aver*15;//计算有效孔位置PD阀值
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
	if(Sys.state & SysState_CaliTemplateHoleFluo)	{//保存空孔PD最大值 最小值		
		idx = gPD_Data.templatehole.idx;
		if(ad_vol>gPD_Data.templatehole.max[idx])	//最大值
			gPD_Data.templatehole.max[idx] = ad_vol;
		else if(ad_vol<gPD_Data.templatehole.min[idx])//最小值
			gPD_Data.templatehole.min[idx] = ad_vol;
	}
	else if(Sys.state & SysState_CaliHolePostion)	{//孔位置校准
		idx = HolePos.idx;
		if(ad_vol < gPD_Data.HoleThreshold)	{
			if(HolePositionCaliFlag == 0)	{
				HolePositionCaliFlag = 1;
			}
			else if(HolePositionCaliFlag == 2)	{
				HolePos.pos[idx].x2 = tMotor[MOTOR_ID1].CurSteps;
				HolePositionCaliFlag = 0;
				HolePos.idx ++;
			}
		}
		else if(HolePositionCaliFlag == 1)	{				
			HolePos.pos[idx].x1 = tMotor[MOTOR_ID1].CurSteps;
			HolePositionCaliFlag = 2;			
		}
	}
}

