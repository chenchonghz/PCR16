#ifndef __rw_spiflash_H__
#define __rw_spiflash_H__

#include "includes.h"
//#include "bsp_w25qxx.h"

#define ONELOG_SIZE		80
#define RLOG_BUFSIZE		1024
#define LOG_FILE_MAXSIZE		(10*1024)//log最大长度10k
#define LOG_FILE_TRUNCATION_SIZE		1024//log文件截断长度
#define LOG_DISPLAY_LINE	13//最多显示13行
#define	LOG_DISPLAY_SIZE	500//255//(WLOG_BUFSIZE*LOG_DISPLAY_LINE)//log文本框显示最大文本字节

#define	LabTemplateMax		6

typedef struct _loginfor	{
	char *pbuf;
	u32 len;
}_loginfor_t;

typedef struct _labtemplate	{
	char name[LAB_NAME_LEN];
	char time[18];
}_labtemplate_t;

typedef struct _labtemplatelist	{
	u8 num;
	_labtemplate_t list[LabTemplateMax];
}_labtemplatelist_t;

extern _labtemplatelist_t gLabTemplatelist;

extern struct _flashfs flashfs;
extern _loginfor_t LogInfor;
//void fs_test();
int FlashFSInit(void);
u8 write_log(void);
u32 read_log(char *pbuf);
void CreateSysFile(void);
void WriteTempJsonFile(void);
void ReadTempJsonFile(void);
u8 GetFlashSpace(u32 *ptotal, u32 *pfree);
void WriteLabTemplate(void);
void ReadLabTemplateList(void);
void DeleteLabTemplate(u8 item);
int AnalysisLabTemplate(u8 item);
FRESULT f_deldir(const TCHAR *path);
#endif
