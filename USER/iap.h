#ifndef _IAP_H
#define _IAP_H

#include "includes.h"

#define 	RunIAPKeyword               0xA5A55A5A
#define		ApplicationAddress			0x0800F000
#define		ApplicationJump				0x200
#define		ApplicationKeywordAddr		0x080FF800

#define 	FIRMWARE_CHECK_STRING    	"TechWay, Inc."
#define		FIRMWARE_ATRR				"PCR16"

typedef  void (*pFunction)(void);

void run_application(void);
void JumpToApp(void);
void FWUpdate_reboot(void);
#endif
