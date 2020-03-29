#ifndef __SYS_DATA_H
#define __SYS_DATA_H

#include "sys_bits.h"
#include "sys_types.h"
#include "sys_defs.h"
#include "tlsf.h"
#include "timestamp.h"

#define	FILE_NAME_LEN		50
#define	LOG_FILE_NAME		"Pcr16Log.txt"//"PerfusionLog.txt"

#define	HEATCOVER_TEMPMAX		110
#define	HEATCOVER_TEMPMIN		30

#define SysState_None							0
#define SysState_Standby					DEF_BIT00_MASK
#define SysState_DeleteLabTB			DEF_BIT01_MASK
#define SysState_RunningTB				DEF_BIT04_MASK

//设备运行模式
enum	{
	DevState_IDLE,//待机模式
	DevState_Running,
	DevState_Error,//设备有故障
	DevState_Test,
};

typedef struct _syserror	{
	BIT8 Y1;
	BIT8 Y2;
	//BIT8 Y3;
}_syserror_t;

typedef struct _sys	{
	volatile u32 state;
	volatile u8 devstate;//设备运行状态
	volatile u8 devsubstate;//设备运行子状态
	volatile u8 AlarmType;//报警类型
	u8 StandbyFlag;
}_sys_t;

//typedef struct _sys_data	{
//	s8 HeatCoverTemp;//热盖温度
//	s8 Temp1;//
//	s8 Temp2;//
//	s16 PD_1;//PD传感器1
//	s16 PD_2;//PD传感器2
//}_sys_data_t;
#define	STAGE_MAX		9
#define	STEP_MAX		9
#define	LAB_ID_LEN		22
#define	LAB_NAME_LEN		15
#define	HOLE_NUM		16
typedef struct _step	{
	s16 temp;
	u16 tim;
}_step_t;

typedef struct _stage	{
	u8 CollEnable;//采集使能
	u8 Type;//0-repeat模式;1-continue 模式;2-step 模式
	u16 T_Rate;//升温速率
	u16 T_Inter;//温度间隔
	u16 T_Tim;//恒温时间 s
	u8 Repeat;//
	u8 StepNum;
	_step_t step[STEP_MAX];
}_stage_t;

typedef struct _temp_data	{//温度信息
	u8 StageNum;
	u8 HeatCoverEnable;
	s16 HeatCoverTemp;
	_stage_t stage[STAGE_MAX];
}_temp_data_t;

typedef struct _lab_data	{//实验数据
	char id[LAB_ID_LEN];
	char name[LAB_NAME_LEN];
	char type;//实验类型。0-阴性/阳性；1-定量；2-基因分型
	char method;//实验方法。0-标准曲线；1-比较CT
}_lab_data_t;

typedef struct _hole_data	{//孔信息
	char name[LAB_NAME_LEN];
	char prj[LAB_NAME_LEN];
	char sample_t;//样本类型：S-标准品;U-待测;N-阴性对照;P-阳性对照;0x20-空
	char channel;//通道	
}_hole_data_t;

typedef struct _sample_data	{//样本数据
//	u8 id;
	_hole_data_t hole[HOLE_NUM];
}_sample_data_t;

extern _sys_t Sys;
extern RTC_TIME_ST SysTime;
extern tlsf_t UserMem;
extern _syserror_t SysError;
extern _sample_data_t sample_data;
extern _lab_data_t	lab_data;
extern _temp_data_t temp_data;

void SysDataInit(void);

#endif
