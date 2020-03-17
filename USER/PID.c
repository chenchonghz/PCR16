#include "PID.h"


_PID_t PID[PID_NUMS];
void PIDParamInit(void)
{
	u8 i;
	
	for(i=0;i<PID_NUMS;i++)	{
		PID[i].Target = 0;
		PID[i].Actual = 0;
		PID[i].diff = 0;
		PID[i].diff_last = 0;
		PID[i].diff_llast = 0;
		PID[i].Kp = 0;
		PID[i].Ki = 0;
		PID[i].Kd = 0;
		PID[i].integral = 0;
		PID[i].issue_cnt = 0;
	}
}
//设置pid控制参数
void SetPIDVal(u8 id, float P,float I,float D)
{
	_PID_t *pPid = &PID[id];
	
	pPid->Kp = P;
	pPid->Ki = I;
	pPid->Kd = D;
	pPid->diff = 0;
	pPid->diff_last = 0;
	pPid->diff_llast = 0;
	pPid->integral = 0;
}
//设置pid控制目标
void SetPIDTarget(u8 id, s32 data)
{
	_PID_t *pPid = &PID[id];
	
	pPid->Target = data;
}
//准的位置式计算法公式 Pout(t)=Kp*e(t)+Ki*∑e(t)+Kd*(e(t)-e(t-1));
//PID控制 设置值：set_dat  实际值：actual_dat
#if 0
s32 PID_control(u8 id, s32 set_dat,s32 actual_dat)
{
	float increment;
	s32 ret;
	_PID_t *pPid = &PID[id];

	pPid->Actual = actual_dat;
	pPid->diff_last = pPid->diff;// 上次误差
	pPid->diff = pPid->Target - pPid->Actual;// 本次误差
//	temp = abs(pPid->diff);

	pPid->integral += pPid->diff;//积分值
	increment = pPid->Kp * pPid->diff + pPid->Ki * pPid->integral + pPid->Kd*(pPid->diff - pPid->diff_last);//计算增量
	return (s32)(increment);
}
#endif
//增量法计算公式：Pdlt=Kp*(e(t)-e(t-1))+Ki*e(t)+Kd*(e(t)-2*e(t-1)+e(t-2));
s32 PID_control(u8 id, s32 set_dat,s32 actual_dat)
{	
	float increment;
//	s32 ret;
	_PID_t *pPid = &PID[id];
	
	pPid->Actual = actual_dat;
	pPid->diff_llast = pPid->diff_last;// 上上次误差
	pPid->diff_last = pPid->diff;// 上次误差
	pPid->diff = pPid->Target - pPid->Actual;// 本次误差
	increment = pPid->Kp * (pPid->diff - pPid->diff_last)   \
              + pPid->Ki *  pPid->diff         \
              + pPid->Kd * (pPid->diff - 2*pPid->diff_last + pPid->diff_llast);
	return (s32)(increment);
}

