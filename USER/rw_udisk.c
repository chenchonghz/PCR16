#include "rw_udisk.h"
#include "tlsf.h"

_udiskfs_t udiskfs;

// mount/unmount udisk
void MountUDISK(u8 src)
{
	FRESULT res;	
	
	switch(src)	{
		case APPLICATION_READY:
			{
				BSP_PRINTF("UDISK connected");
				res = f_mount(&udiskfs.fs,USBHPath,1);
				if(res != FR_OK)	{
					BSP_PRINTF("mount udisk failed");
				}
				else {					
//					FILINFO fn;
					DIR dir;
					BSP_PRINTF("mount udisk ok");
					res = f_opendir(&dir,USBHPath);
					if(res==FR_OK)	{
//						for(;;)	{
//							res=f_readdir(&dir,&fn);
//							if(res!=FR_OK||fn.fname[0]==0)	break;
//							BSP_PRINTF("udisk filename: %s",fn.fname);
//						}
						f_closedir(&dir);
						udiskfs.flag |= UDISKFLAG_MOUNTED;
					}
				}
				break;
			}
		case APPLICATION_DISCONNECT:
			{
				BSP_PRINTF("UDISK disconnect");
				res=f_mount(NULL,USBHPath,1);//(NULL:unmount)
				if(res != FR_OK)	{
					BSP_PRINTF("unmount udisk failed");
				}else	{
					BSP_PRINTF("unmount udisk ok");
				}
				udiskfs.flag &= ~UDISKFLAG_MOUNTED;
				break;
			}
	}
}
#define	 UDISK_COPY_SIZE		512
static 	FIL srcfile, destfile;
u8 CopyFile(char *psrc_path, char *pdest_path)
{
	u8 ret=0;
	FRESULT res;
	char *pbuf;
	UINT rsize,wsize;
	//打开第一个文件，即flash已存在的文件
	res = f_open(&srcfile,psrc_path, FA_OPEN_EXISTING|FA_READ);//不知道是文本还是二进制，所以统一用二进制
	if(res!=FR_OK) {
		return 0;
	}
	//打开第二个文件，不存在的文件
	res =  f_open(&destfile,pdest_path, FA_CREATE_ALWAYS|FA_READ|FA_WRITE);//以二进制打开，不能用a方式
	if(res!=FR_OK) {
		f_close(&srcfile);//当打开目标文件出错时，原始文件已经打开了，这里会直接结束程序，所以应该结束原始文件
		return 0;
	}
	pbuf = (char *)tlsf_malloc(UserMem, UDISK_COPY_SIZE);
	for(;;) {
		res =  f_read(&srcfile, pbuf, UDISK_COPY_SIZE, &rsize);//一次存储100字节整数倍，导致原目大小不一致，所以在存的时候用size	
		if(res != FR_OK||rsize==0)	break;
		res =  f_write(&destfile, pbuf, rsize, &wsize);
		if(res != FR_OK)	break;
	}
	tlsf_free(UserMem,pbuf);
	pbuf = NULL;
	if(f_size(&srcfile)==f_size(&destfile))	{//判断大小是否一致 拷贝是否成功
		BSP_PRINTF("copy %s ok", pdest_path);
		ret = 1;
	}	
	//f_sync(&destfile);
	f_close(&srcfile);
	f_close(&destfile);
	return ret;
}

//拷贝文件 从srcfile -- spi flash 拷贝到 destfile -- u盘
u8 CopyFileToUDISK(char *psrc_name, char *pdest_name)
{
//	FRESULT res;     /* FatFs return code */		
	char src_name[FILE_NAME_LEN],dest_name[FILE_NAME_LEN];
	
	sprintf(dest_name, "%s%s", USBHPath, pdest_name);//u盘设备路径
	sprintf(src_name, "%s%s", USERPath, psrc_name);//flash设置路径
	return CopyFile(src_name, dest_name);
}

u8 CopyFileToSpiflash(char *psrc_name, char *pdest_name)
{
//	FRESULT res;     /* FatFs return code */		
	char src_name[FILE_NAME_LEN],dest_name[FILE_NAME_LEN];
	
	sprintf(dest_name, "%s%s", USERPath, psrc_name);//flash设置路径
	sprintf(src_name, "%s%s", USBHPath, pdest_name);//u盘设备路径
	return CopyFile(src_name, dest_name);
}

