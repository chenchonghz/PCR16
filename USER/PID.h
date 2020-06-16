#ifndef __PID_H__
#define __PID_H__

#include "includes.h"

typedef enum {
    PID_ID1      = 0,
    PID_ID2      = 1
} PID_ID;

#define PID_NUMS       	(PID_ID2+1)

typedef struct _PID{
    float    Kp,Ki,Kd;         // 比例系数,积分系数,微分系数
    s32      period;     // 控制周期
    s32    Target;         // 目标
    s32    Actual;         // 实际
    s32    diff;      // 偏差值
    s32    diff_last;      // 上一个偏差值
    s32    diff_llast;      // 上上一个偏差值
    s32 integral;
    u8 issue_cnt;
} _PID_t;

extern _PID_t PID[PID_NUMS];
void PIDParamInit(void);
void SetPIDVal(u8 id, float P,float I,float D);
void SetPIDTarget(u8 id, s32 data);
float PID_control(u8 id, s32 actual_dat);
s32 GetPIDDiff(u8 id);
void ClearPIDDiff(u8 id);
#endif

