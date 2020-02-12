#include "sys_data.h"

_sys_data_t SysData;
tlsf_t UserMem;
void SysDataInit(void)
{
	UserMem = tlsf_create_with_pool((void *)0x20015000, 0x3000);//内存0x15000 - 0x18000 区域12KB内存使用tlsf管理
	
	SysData.HeatCoverTemp = 0;
	SysData.PD_1 = 0;
	SysData.PD_2 = 0;
}

