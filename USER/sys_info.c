#include "sys_info.h"


////////////////////这些信息是可修改的/////////////////////
ident_infor_t g_ident_infor = {//仪器版本信息
    .type           = CONFIG_SYSINFO_Type,//默认产品型号
    .product_sn     = CONFIG_SYSINFO_SN,//默认产品序列号
    .pcbver			= CONFIG_SYSINFO_HW_Version,//默认PCBA号
    .pcbaver		= CONFIG_SYSINFO_BOARD_Version,//默认PCB号
    .board_sn    	= CONFIG_SYSINFO_BoardSN//默认板序列号
};

sensorboard_infor_t sensor_infor;
////////////////////////////////////////////////////////
const sys_infor_t g_sys_infor = {
    .pname          = CONFIG_SYSINFO_Name,
    .pfwver         = CONFIG_SYSINFO_FW_Version,
	.pfwtype		= SYS_FW_Type,
    .pbuilddate     = CONFIG_SYSINFO_BuildDate,
    .pbuildtime     = CONFIG_SYSINFO_BuildTime,
    .pmanufacturer  = CONFIG_SYSINFO_Manufacturer,
    .pident         = &g_ident_infor,
	.psensorinfor	= &sensor_infor,
};

const u8 SYSINFOR_PASSWORD[6] = {0x38,0x32,0x38,0x30,0x30,0x35};//执行修改系统信息匹配密码

void show_sys_info(void)
{
    sys_infor_t const *pinfo = &g_sys_infor;
    ident_infor_t *pident = pinfo->pident;

    SYS_PRINTF("Producte name:        \t\t%s",    pinfo->pname);
    SYS_PRINTF("Producte type:        \t\t%s",    pident->type);
    SYS_PRINTF("Producte SN:          \t\t%s",    pident->product_sn);
    SYS_PRINTF("Board SN:        \t\t%s",  pident->board_sn);
    SYS_PRINTF("Firmware Type:      \t\t%s",    pinfo->pfwtype);
    SYS_PRINTF("Firmware Version:   \t\t%s",    pinfo->pfwver);
    SYS_PRINTF("Hardware Version:   \t\t%s",    pident->pcbver);
    SYS_PRINTF("Board Version:      \t\t%s",    pident->pcbaver);
    SYS_PRINTF("Firmware Build Date:\t\t%s",    pinfo->pbuilddate);
    SYS_PRINTF("Firmware Build Time:\t\t%s",    pinfo->pbuildtime);
    SYS_PRINTF("Manufacturer:       \t\t%s",    pinfo->pmanufacturer);
}


