#include "sys_data.h"

_sys_t Sys;
_syserror_t SysError;
RTC_TIME_ST SysTime;
tlsf_t UserMem;

_sample_data_t sample_data;
_lab_data_t	lab_data;
_temp_data_t temp_data;

void SysDataInit(void)
{
	UserMem = tlsf_create_with_pool((void *)0x20015000, 0x3000);//内存0x15000 - 0x18000 区域12KB内存使用tlsf管理
	
	SysError.Y1.ubyte = 0x0;//来自传感器板的故障 
	SysError.Y2.ubyte = 0;//主板故障
//	SysData.HeatCoverTemp = 0;
//	SysData.PD_1 = 0;
//	SysData.PD_2 = 0;
	
	Sys.state = SysState_None;
	Sys.devstate = DevState_IDLE;
	
	SysTime.tm_year = 2019;
	SysTime.tm_mon = 7;
	SysTime.tm_mday = 17;
	SysTime.tm_hour = 17;
	SysTime.tm_min = 44;
	SysTime.tm_sec = 20;
	SysTime.tm_wday = 3;
}

void ResetSampleData(void)
{
	u8 i;
	
	for(i=0;i<HOLE_NUM;i++)	{
		memset((void *)sample_data.hole[i].name, 0x20, LAB_NAME_LEN);
		memset(sample_data.hole[i].prj, 0x20, LAB_NAME_LEN);
		sample_data.hole[i].channel = 1;
		sample_data.hole[i].sample_t = 0x20;
	}
}

void ResetLabData(void)
{
	memset(lab_data.name,0x20,LAB_NAME_LEN);
	lab_data.type = 0xff;
	lab_data.method = 0xff;
}

void ResetTempData(void)
{
	u8 i;
	temp_data.HeatCoverEnable = DEF_True;
	temp_data.HeatCoverEnable = 105;
	temp_data.StageNum = 0;
	for(i=0;i<STAGE_MAX;i++)	{
		temp_data.stage[i].CollEnable = DEF_False;
		temp_data.stage[i].Repeat = 0;
		temp_data.stage[i].StepNum = 0;
		temp_data.stage[i].Type = 0;
	}
}
