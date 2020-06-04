#include "iap.h"

static  pFunction Jump_To_Application;
static  INT32U    JumpAddress;

void run_application(void)
{
    char buf[0x10];
    
	if ((*(INT32U *)ApplicationKeywordAddr) == RunIAPKeyword) {
		return;
	}
    memcpy(buf,(const char*)ApplicationAddress,0x10);
    if(0==strcmp(buf,FIRMWARE_CHECK_STRING))  {
        memcpy(buf,(const char*)(ApplicationAddress+0x10),0x10);
        if(0==strcmp(buf,FIRMWARE_ATRR))        {
			JumpToApp();
        }
        else    {
          return;
        }
    }
    else        {
      return;
    }
}

void JumpToApp(void)
{
	 /* Test if user code is programmed starting from address "ApplicationAddress" */
	if (((*(volatile INT32U *)(ApplicationAddress+ApplicationJump)) & 0x2FFE0000) == 0x20000000)
	{
		/* Jump to user application */
		JumpAddress = *(volatile INT32U *)(ApplicationAddress + 4 + ApplicationJump);
		Jump_To_Application = (pFunction)JumpAddress;
		/* Initialize user application's Stack Pointer */
		__set_MSP(*(volatile INT32U *)(ApplicationAddress + ApplicationJump));
		Jump_To_Application();
	}
}

void SoftReset(void)
{
	__set_FAULTMASK(1);      // 关闭所有中端
	NVIC_SystemReset();// 复位
}

void FWUpdate_reboot(void)
{
	FLASH_EraseInitTypeDef f;
	uint32_t PageError = 0;
	
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.Page = 511;
	f.NbPages = 1;
	f.Banks = FLASH_BANK_2;
	//设置PageError	
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&f, &PageError);	//调用擦除函数 擦除RunIAPKeyword
	if (PageError == HAL_FLASH_ERROR_NONE||PageError == 0xFFFFFFFF) {
		HAL_FLASH_Lock();
		SYS_PRINTF("SYS Reboot");
		OSTimeDly(3000);
		SoftReset();//RUN IAP
	}
	HAL_FLASH_Lock();
}
