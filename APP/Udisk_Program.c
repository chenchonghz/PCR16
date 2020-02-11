#include "Udisk_Program.h"
#include "rw_udisk.h"


void Udisk_Program(void)
{
	if(Appli_state == APPLICATION_READY||Appli_state == APPLICATION_DISCONNECT)	{
		if(!(udiskfs.udisk_flag&UDISKFLAG_MOUNTED))
			MountUDISK(Appli_state);
	}
}

