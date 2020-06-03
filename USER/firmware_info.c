/*
 * compile_link_info.c
 *
 *  Created on: 2015-10-28
 *      Author: dengwu guo
 */
#include "firmware_info.h"
#include "sys_info.h"
#include "sys_data.h"

const char  firmware_check_string[0x10] __attribute__((section ("FWInfor"),used))	= CONFIG_SYSINFO_Manufacturer;
const char  firmware_attr[0x10] __attribute__((section ("FWInfor"),used))	= CONFIG_SYSINFO_Name;
const char  firmware_attr1[0x10] __attribute__((section ("FWInfor"),used))	= "Mainboard";
const char  firmware_ver[0x10] __attribute__((section ("FWInfor"),used))	= CONFIG_SYSINFO_FW_Version;
const char  compile_date[0x10] __attribute__((section ("FWInfor"),used))	= __DATE__;
const char  compile_time[0x10] __attribute__((section ("FWInfor"),used))	= __TIME__;
const char  unused[0x200-0x60] __attribute__((section ("FWInfor"),used)) = {0};
/*
#pragma default_variable_attributes = @ "firmware_info_sect"

__root const char firmware_check_string[0x10] @ "firmware_info_sect" = "Acea Co,Ltd";
__root const char firmware_attr[0x10] @ "firmware_info_sect" = "Shaker";
__root const char firmware_ver[0x10] @ "firmware_info_sect" = "V1.00"; 
__root const char compile_date[0x10] @ "firmware_info_sect" = __DATE__;
__root const char compile_time[0x10] @ "firmware_info_sect" = __TIME__;

#pragma default_variable_attributes =
*/
