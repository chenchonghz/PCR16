#ifndef __SYS_INFO_H__
#define __SYS_INFO_H__

#include "includes.h"

#define SYSINFO_SIZE          30

/*
********************************************************************************
* Pre-processor Definitions
********************************************************************************
*/
#define CONFIG_SYSINFO_Name                            "PCR16"
#define CONFIG_SYSINFO_Type                            "xxxxxx"
#define CONFIG_SYSINFO_SN                              "281090312893"
#define CONFIG_SYSINFO_BoardSN                  		"********"
#define CONFIG_SYSINFO_HW_Version   				   	"V0.1"
#define CONFIG_SYSINFO_BOARD_Version                  	"V0.1"
#define CONFIG_SYSINFO_FW_Version   				   	"V0.0.3M"
#define CONFIG_SYSINFO_BuildDate                      	__DATE__
#define CONFIG_SYSINFO_BuildTime                     	__TIME__
#define CONFIG_SYSINFO_Manufacturer                    "TechWay, Inc."

#define SYS_FW_Type                            "Debug"

enum IG_INDEX {
	IDX_PRODUCT_NAME            	=   0x00,   // 仪器名称
    IDX_PRODUCT_TYPE,   // 仪器型号
    IDX_PRODUCT_SN,   // 仪器序列号
	IDX_MAINBOARD_FWVer,//主板固件版本
    IDX_MAINBOARD_PCBVer,   // 主板PCB版本
    IDX_MAINBOARD_PCBAVer,   // 主板PCBA版本
    IDX_MAINBOARD_SN ,   // 主板序列号
    IDX_SENSORBOARD_FWVer,   // 传感器板固件版本
    IDX_SENSORBOARD_PCBVer,//传感器板PCB版本
    IDX_SENSORBOARD_PCBAVer ,//传感器板PCBA版本
    IDX_SENSORBOARD_SN ,//传感器板序列号
};
/*
enum PassWard_Type	{
	ChangeProductorType  =0x01,//修改产品型号
	ChangeProductorSN   =	0x02,//修改产品序列号
	ChangeMainbordPCBVer =0x03,//修改主板PCB版本号
	ChangeMainbordPCBAVer =	0x04,//修改主板PCBA版本号
	ChangeProductorDate 	= 0x05,//修改产品日期
	ChangeSlavebordPCBVer  = 0x06,//修改从板PCB版本号
	ChangeSlavebordPCBAVer =0x07,//修改从板PCBA版本号
	ChangePlateType = 0x08,   //修改plate照明类型
};
*/
typedef struct _ident_infor {
    char    type[SYSINFO_SIZE];
    char    product_sn[SYSINFO_SIZE];
    char    pcbver[SYSINFO_SIZE];
    char    pcbaver[SYSINFO_SIZE];
    char    board_sn[SYSINFO_SIZE];
	//u16 	crc;
} ident_infor_t;

typedef struct _sensorboard_infor {
    char    fwver[SYSINFO_SIZE];   
    char    pcbver[SYSINFO_SIZE];
    char    pcbaver[SYSINFO_SIZE];
	char    sn[SYSINFO_SIZE];
} sensorboard_infor_t;

typedef struct _sys_infor {
    char        *pname;
    char        *pfwver;
	char 		*pfwtype;
    char        *pbuilddate;
    char        *pbuildtime;
	char 		*pmanufacturer;
    ident_infor_t *pident;
	sensorboard_infor_t	*psensorinfor;
} sys_infor_t;

//extern sensorboard_infor_t sensor_infor;
//extern ident_infor_t g_ident_infor;
extern const sys_infor_t g_sys_infor;
extern const u8 SYSINFOR_PASSWORD[6];

void show_sys_info(void);
INT8U SystemInfoRead(INT8U idxInfo, char *pbuf);
INT8U SystemInfoWrite(INT8U idxInfo, char *pbuf);
#endif
