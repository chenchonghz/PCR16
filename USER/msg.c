#include "msg.h"

OS_FLAG_GRP    *SysFlagGrp;

void msg_init(void)
{
    INT8U err;

    //z_stop_sem          = OSSemCreate(0);
    //OSEventNameSet(piping_mail, (INT8U *)"piping_mail", &err);
	SysFlagGrp = OSFlagCreate(0, &err);
}

void mutex_lock (OS_EVENT *lock)
{
    INT8U   err;

    OSSemPend(lock, 0, &err);
	//OSMutexPend(lock, 0, &err);
}

void mutex_unlock (OS_EVENT *lock)
{
	//INT8U   err;
    OSSemPost(lock);
	//OSMutexPost(lock);
}

INT8U  UsartRxGetINT8U (u8 *buf,INT32U *idx)
{
    return (buf[(*idx)++]);
}

INT16U  UsartRxGetINT16U (u8 *buf,INT32U *idx)
{
    INT16U  lowbyte;
    INT16U  highbyte;

    highbyte  = UsartRxGetINT8U(buf,idx);
    lowbyte = UsartRxGetINT8U(buf,idx);
    return ((highbyte << 8) | lowbyte);
}

INT32U  UsartRxGetINT32U (u8 *buf,INT32U *idx)
{
    INT32U  highword;
    INT32U  lowword;

    highword = UsartRxGetINT16U(buf,idx);
     lowword= UsartRxGetINT16U(buf,idx);
    return ((highword << 16) | lowword);
}
