#include "sys_data.h"

_sys_t Sys;
_syserror_t SysError;
RTC_TIME_ST SysTime;
tlsf_t UserMem;


void SysDataInit(void)
{
	UserMem = tlsf_create_with_pool((void *)0x20012000, 0x6000);//内存0x12000 - 0x18000 区域24KB内存使用tlsf管理
	SysError.Y1.ubyte = 0x0;//来自传感器板的故障 
	SysError.Y2.ubyte = 0;//主板故障
	
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

//重定向tlsf_malloc
void *user_malloc(size_t size)
{
	void *ret;
	ret = tlsf_malloc(UserMem, size);
	return ret;
}
//重定向user_free
void user_free(void* ptr)
{
	tlsf_free(UserMem, ptr);
	ptr = NULL;
}
