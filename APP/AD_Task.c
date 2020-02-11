#include "AD_Task.h"

_ad7124_t ad7124;

void AD_TaskInit(void)
{
	u8 ad7124_id;
	ad7124.pdev = bsp_ad7124_init(AD7124_ID1);
	ad7124_id = bsp_ad7124_id_get(ad7124.pdev);
	if(ad7124_id == AD7124_ID)	{
//		sys_error.Y2.bits.b2 = SENSOR_OK;
	}else	{
//		sys_error.Y2.bits.b2 = SENSOR_ERROR;//AD7124“Ï≥£
	}
}
