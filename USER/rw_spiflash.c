#include "rw_spiflash.h"
#include "json.h"
#include "app_spiflash.h"

#define FORMAT_DISK			0

struct _flashfs	{
	DIR dir;
	FATFS fs;
	FIL	fil;
} flashfs;

_loginfor_t LogInfor;

u8 GetFlashSpace(u32 *ptotal, u32 *pfree)
{
	FATFS *fs;
	FRESULT res;
    DWORD fre_clust, fre_sect, tot_sect;

    /* Get volume information and free clusters of drive 1 */
    res = f_getfree(USERPath, &fre_clust, &fs);
    if (res!=FR_OK) return 0;

    /* Get total sectors and free sectors */
    tot_sect = (fs->n_fatent - 2) * fs->csize;
    fre_sect = fre_clust * fs->csize;

    /* Print the free space (assuming 4 Kbytes/sector) */
	*ptotal = (u32)(tot_sect * 4);
	*pfree = (u32)(fre_sect * 4);
    //BSP_PRINTF("%10lu KiB total drive space.\n%10lu KiB available.\n", tot_sect, fre_sect);	

	return 1;
}

int FlashFSInit(void)
{
	FRESULT res;
	
	u32 disk_tot, disk_free;
#if FORMAT_DISK == 0	
	res = f_mount(&flashfs.fs, USERPath, 1);
	if(res == FR_OK)	{
		DIR dir;
		FILINFO fn;
		BSP_PRINTF("mount flash ok");
		res = f_opendir(&dir,USERPath);
		if(res==FR_OK)	{
			for(;;)	{
				res=f_readdir(&dir,&fn);
				if(res!=FR_OK||fn.fname[0]==0)	break;
				BSP_PRINTF(" %s %ld",fn.fname, fn.fsize);
			}
			f_closedir(&dir);
		}
	}else if(res == FR_NO_FILESYSTEM)	{
#endif
		u8 *work;		
		work = (u8 *)user_malloc(_MAX_SS);
		/* Create FAT volume */
		BSP_PRINTF("mount flash failed, format flash...");
		BSP_W25Qx_Erase_Chip();//先格式化
		//OSTimeDly(5000);
		res = f_mkfs(USERPath, FM_ANY, 0, work, _MAX_SS);
		tlsf_free(UserMem,work);
		work = NULL;
		if(res == FR_OK)	{
			BSP_PRINTF("format flash ok");
		}else	{
			BSP_PRINTF("format flash failed");
			return res;
		}
		res = f_mount(&flashfs.fs, USERPath, 1);
		if(res == FR_OK)			
			BSP_PRINTF("mount flash ok");
		else	
			return res;
#if FORMAT_DISK == 0
	}
#endif
	
//	GetPerfuseDataFilename(NULL);
	GetFlashSpace(&disk_tot, &disk_free);
	BSP_PRINTF("SPI Flash Space:");
	BSP_PRINTF("    %u KiB total drive space.\n    %u KiB available.\n", disk_tot, disk_free);
	return res;
}

//创建系统所需文件 若文件已存在，检测大小，超过指定大小，删除旧数据
void CreateLogFile(void)
{
	FILINFO fn;
	FRESULT res;     /* FatFs return code */
	u32 fil_size,rsize;
	char filename[FILE_NAME_LEN];
	char *pLogBufer;
	
	sprintf(filename, "%s%s", USERPath, LOG_FILE_NAME);
	res = f_stat(filename, &fn);
	if(res == FR_OK)	{//If exist, the function returns with FR_OK
		BSP_PRINTF("Time: %u/%02u/%02u, %02u:%02u\n",
               (fn.fdate >> 9)+1980, (fn.fdate >> 5) & 0x0f, fn.fdate & 0x1f,
               fn.ftime >> 11, (fn.ftime >> 5) & 0x3f);
		res = f_open(&flashfs.fil, filename, FA_OPEN_APPEND | FA_WRITE| FA_READ);
		fil_size = f_size(&flashfs.fil);//获取log文件大小
		if(fil_size>LOG_FILE_MAXSIZE)	{//文件大于10k 删除旧数据
			char data;
			fil_size -= LOG_FILE_TRUNCATION_SIZE;
			f_lseek(&flashfs.fil, fil_size);//将文件指针指向文件末尾1k的位置
			for(;;)		{//找到换行位置 目的是在截取文件时 保留完整的行
				f_read(&flashfs.fil, &data, 1, &rsize);
				if(rsize==0)	goto _exit;
				if(data == '\n')	{
					break;
				}
			}
			pLogBufer = (char *)user_malloc(RLOG_BUFSIZE);
			f_read(&flashfs.fil, pLogBufer, LOG_FILE_TRUNCATION_SIZE, &rsize);//读出需要保留的内容
			f_close(&flashfs.fil);
			f_open(&flashfs.fil, filename, FA_CREATE_ALWAYS | FA_WRITE);//重新创建文件
			f_write(&flashfs.fil, pLogBufer, rsize, NULL);//重新写入保留的内容
			user_free(pLogBufer);
		}
	}
	else if(res == FR_NO_FILE)	
	{//文件不存在 or 强制创建标志有效 创建文件
		res = f_open(&flashfs.fil, filename, FA_CREATE_ALWAYS | FA_WRITE);//create new file and w mode
		if(res != FR_OK)	{
			return;
		}
		f_close(&flashfs.fil);
//		write_log("Create file.");
	}
_exit:
	f_close(&flashfs.fil);
	pLogBufer = NULL;
//	return 1;
}

//将日志从缓存写入flash
u8 write_log(void)
{
	FRESULT res;     /* FatFs return code */
	u32 len;
	char filename[FILE_NAME_LEN];
	
	if(LogInfor.len == 0)	{
		return 0;
	}
	mutex_lock(spiflash.lock);
	sprintf(filename, "%s%s", USERPath, LOG_FILE_NAME);//log文件名
	res = f_open(&flashfs.fil, filename, FA_OPEN_APPEND | FA_WRITE| FA_READ);//create new file and rw mode
	if(res != FR_OK)
		goto _exit;
	f_write(&flashfs.fil, LogInfor.pbuf, LogInfor.len, &len);	//写入log文件
	LogInfor.len -= len;	
//	BSP_PRINTF("Write Log OK");
_exit:
	f_close(&flashfs.fil);
	mutex_unlock(spiflash.lock);
	return 1;
}

//读日志 读取log最后一段LOG_DISPLAY_SIZE
u32 read_log(char *pbuf)
{
	FRESULT res;     /* FatFs return code */
	char filename[FILE_NAME_LEN];
	UINT rsize,line;
	u32 logPosition,dissize;
	char data;
	
	rsize = 0;	
	sprintf(filename, "%s%s", USERPath, LOG_FILE_NAME);
	res = f_open(&flashfs.fil, filename, FA_READ);
	if(res != FR_OK)
		return 0;
	logPosition = 0;
	dissize = f_size(&flashfs.fil);
	if(dissize>=25)
		logPosition = dissize - 25;
	dissize = 0;
	line = 1;
	for(;;)		{//只显示最后13行
		if(logPosition==0)	{
			break;
		}
		f_lseek(&flashfs.fil, logPosition--);
		f_read(&flashfs.fil, &data, 1, &rsize);
		if(rsize==0)	goto _exit;
		dissize += 1;
		if(dissize >= (LOG_DISPLAY_SIZE-25))	{//达到 最大显示内容
			break;
		}
		if(data == '\n')	{
			line++;
			if(line > LOG_DISPLAY_LINE)	{//达到最大显示行数 
				break;
			}
		}
	}
	res =  f_read(&flashfs.fil, pbuf, LOG_DISPLAY_SIZE, &rsize);//一次存储100字节整数倍，导致原目大小不一致，所以在存的时候用size	
	if(res != FR_OK) goto _exit;
_exit:
	f_close(&flashfs.fil);
	return rsize;
}

void WriteTempJsonFile(void)
{
	FRESULT res;
	char filepath[FILE_NAME_LEN];
	
	sprintf(filepath, "%s%s", USERPath, TEMPJSON_FILE_NAME);
	if(CreateTemp_Jsonfile(filepath)==0)	{
		SYS_PRINTF("write temp jsonfile ok");
	}
}

void ReadTempJsonFile(void)
{
	char filepath[FILE_NAME_LEN];
	
	sprintf(filepath, "%s%s", USERPath, TEMPJSON_FILE_NAME);
	AnalysisTemp_Jsonfile(filepath);
}

void WriteLabJsonFile(void)
{
	FRESULT res;
	char filepath[FILE_NAME_LEN];
	
	sprintf(filepath, "%s%s", USERPath, LabJSON_FILE_NAME);
	if(CreateLab_Jsonfile(filepath)==0)	{
		SYS_PRINTF("write lab jsonfile ok");
	}
}

void ReadLabJsonFile(void)
{
	char filepath[FILE_NAME_LEN];
	
	sprintf(filepath, "%s%s", USERPath, LabJSON_FILE_NAME);
	AnalysisLab_Jsonfile(filepath);
}



#if 0	//spi flash 挂载文件系统测试函数
FRESULT res;        /* API result code */
FATFS fs;           /* Filesystem object */
void fs_test()
{
    FIL fil;            /* File object */
    DWORD fre_clust;
    UINT bw;            /* Bytes written */
		FATFS *fls = &fs;
    BYTE work[4096]; /* Work area (larger is better for processing time) */
		char w_string[]={"Hello, World!\r\n"};
		char r_string[18];

		//res = f_getfree(USERPath,&fre_clust,&fls);         /* Get Number of Free Clusters */
		f_mkfs(USERPath, FM_ANY, 0, work, sizeof(work));
		res = f_mount(&fs, USERPath, 1);
		if(res == FR_OK)	{
			BSP_PRINTF("mount file ok");
		}else if(res == FR_NO_FILESYSTEM)	{
			/* Create FAT volume */
			res = f_mkfs(USERPath, FM_ANY, 0, work, sizeof(work));
			if(res == FR_OK)	{
				BSP_PRINTF("f_mkfs ok");
			}else	return;
			res = f_mount(&fs, USERPath, 1);
			if(res == FR_OK)			
				BSP_PRINTF("mount file ok");
			else	return;
		}

    /* Create a file as new */
    res = f_open(&fil, "1:/hello.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		if(res == FR_OK)	{ 
			BSP_PRINTF("open file ok");
		}else	return;

    /* Write a message */
    res = f_write(&fil, w_string, 15, &bw);
    if (bw == 15) 	{
				BSP_PRINTF("write file ok");
		}else	return;
		f_lseek(&fil, 0);
		f_gets(r_string, sizeof(r_string), &fil);
		BSP_PRINTF("r usb:%s\r\n",r_string);
		
    /* Close the file */
    f_close(&fil);

    /* Unregister work area */
    //f_mount(NULL, USERPath, 1);
}
#endif

