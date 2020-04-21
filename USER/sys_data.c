#include "sys_data.h"

_sys_t Sys;
_syserror_t SysError;
RTC_TIME_ST SysTime;
tlsf_t UserMem;

_sample_data_t sample_data;
_lab_data_t	lab_data;
_temp_data_t temp_data;

const char SampleType[][1] = {
	{0},{'S'},{'U'},{'N'},{'P'},
};//样本类型：S-标准品;U-待测;N-阴性对照;P-阳性对照;0-空

const char SampleChannel[][4] = {
	{0},{"FAM"},{"HEX"}
};//样本类型：S-标准品;U-待测;N-阴性对照;P-阳性对照;0-空

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
	
	ResetLabDataDefault();
	ResetSampleDataDefault();
	ResetTempDataDefault();
}

void SetSampleDataSampleT(u32 enable, char typeidx)
{
	u8 i;
	u32 tmp;
	
	for(i=0;i<HOLE_NUM;i++)	{
		tmp = enable&(DEF_BIT00_MASK<<i);
		if(tmp)	{
			sample_data.hole[i].sample_t = SampleType[typeidx][0];
//			sample_data.enable |= tmp;
		}
	}
}

void SetSampleDataChannel(u32 enable, char typeidx)
{
	u8 i;
	u32 tmp;
	
	for(i=0;i<HOLE_NUM;i++)	{
		tmp = enable&(DEF_BIT00_MASK<<i);
		if(tmp)	{
			strcpy(sample_data.hole[i].channel, &SampleChannel[typeidx][0]);
//			sample_data.enable |= tmp;
		}
	}
}

void DisableSampleData(u32 enable)
{
	u8 i;
	u32 tmp;
	
	for(i=0;i<HOLE_NUM;i++)	{
		tmp = enable&(DEF_BIT00_MASK<<i);
		if(tmp)	{
			sample_data.hole[i].sample_t = SampleType[0][0];
			strcpy(sample_data.hole[i].channel, &SampleChannel[0][0]);
//			sample_data.enable &= ~tmp;
		}
	}
}

void ResetSampleDataDefault(void)
{
	u8 i;
	
//	sample_data.enable = 0;
	for(i=0;i<HOLE_NUM;i++)	{
		memset((void *)sample_data.hole[i].name, 0, LAB_NAME_LEN);
		memset(sample_data.hole[i].prj, 0, LAB_NAME_LEN);
		strcpy(sample_data.hole[i].channel, &SampleChannel[0][0]);
		sample_data.hole[i].sample_t = SampleType[0][0];
	}
}

void ResetLabDataDefault(void)
{
	memset(lab_data.name,0,LAB_NAME_LEN);
	lab_data.type = LabTypeNone;
	lab_data.method = LabMethodNone;
}

void ResetStep(u8 stageid, u8 stepid)
{
	temp_data.stage[stageid].step[stepid].CollEnable = DEF_False;
	temp_data.stage[stageid].step[stepid].temp = 950;
	temp_data.stage[stageid].step[stepid].tim = 60;
}

void ResetStage(u8 id)
{
	u8 j;
	
	temp_data.stage[id].RepeatNum = 1;
	temp_data.stage[id].StepNum = 0;
	temp_data.stage[id].CurStep = 0;
	temp_data.stage[id].CurRepeat = 0;
	temp_data.stage[id].Type = 0;
	for(j=0;j<STEP_MAX;j++)	{
		ResetStep(id,j);
	}
}
//删除阶段
void DelStage(u8 del_id)
{
	s8 i,j;
	
	j = temp_data.StageNum-1;
	if(j>=0)	{
		for(i=del_id;i<j;i++)	{
			memcpy(&temp_data.stage[i], &temp_data.stage[i+1], sizeof(_stage_t));
		}
		ResetStage(i);
		temp_data.StageNum = j;
	}
}

void ResetTempDataDefault(void)
{
	u8 i;
	temp_data.HeatCoverEnable = DEF_True;
	temp_data.HeatCoverTemp = 105;
	temp_data.StageNum = 0;
	temp_data.CurStage = 0;
	for(i=0;i<STAGE_MAX;i++)	{
		ResetStage(i);
	}
}

void HeatCoverOnOff(u8 flag)
{
	temp_data.HeatCoverEnable = flag;
}

//void CollDataOnOff_InStep(u8 flag)
//{
//	u8 m,n;
//	
//	m = temp_data.StageNum-1;
//	if(m<0)	m = 0;
//	n = temp_data.stage[m].StepNum-1;
//	if(n<0)	n = 0;
//	temp_data.stage[m].step[n].CollEnable = flag;
//}

void ClearAllSysStateTB(void)
{
	Sys.state &= ~SysState_RunningTB;
	Sys.state &= ~SysState_StopTB;
	Sys.state &= ~SysState_DeleteLabTB;
	Sys.state &= ~SysState_StageTB;
	Sys.state &= ~SysState_StepTB;
}

