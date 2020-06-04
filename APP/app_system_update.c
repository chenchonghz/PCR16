#include  "app_system_update.h"

#define BIN_BUFF_MAX_SIZE                       1024
#define		ApplicationAddress			0x0800F000//应用程序头地址
#define		ApplicationJump				0x200//应用程序启动地址

static  void    UpdateInit    (void);
static  void    update_bin_start    (update_ctrl_t *pBin);
//static  void    update_bin_transmit (update_ctrl_t *pBin);
static  void    update_bin_end      (update_ctrl_t *pBin);
static  void    TaskSystemUpdate      (void *parg);
u8 FLASH_Erase(void);
u8 FLASH_Program(u8 *pbuf, u32 size);
static  OS_STK  SystemUpdateTaskStk     [STK_SIZE_APP_SYSTEM_UPDATE];
static  INT8U   bin_buff                [BIN_BUFF_MAX_SIZE];
//INT32U BlockNbr = 0, UserMemoryMask = 0;
//volatile INT32U FlashProtection = 0;
INT32U FlashDestination = ApplicationAddress; /* Flash user program offset */
_system_update_t app_system_update;

update_ctrl_t   update_bin = {
    .state         = DEF_Busy,
    .ack           = ACK_OK,
    .update_is_ok  = DEF_No,
    .burn_cnt      = 0,
    .buf_addr      = bin_buff,
    .burn_addr     = ApplicationAddress,
    .size_max      = BIN_BUFF_MAX_SIZE,
    .size_total    = 0,
    .size_cnt      = 0,
    .frame_cnt     = 0,
    .calcsum       = 0,
    .chksum        = 0,
    .call_begin    = update_bin_start,
//    .call_transmit = update_bin_transmit,
    .call_end      = update_bin_end
};

/*F******************************************************************************************************
*                                            System update task
********************************************************************************************************/
void SystemUpdateTaskInit (void)
{
    OSTaskCreate(TaskSystemUpdate,  (void * )0, (OS_STK *)&SystemUpdateTaskStk[STK_SIZE_APP_SYSTEM_UPDATE-1], TASK_PRIO_SYSTEM_UPDATE);
}

/*F******************************************************************************************************
*
********************************************************************************************************/
static void UpdateInit(void)
{
	app_system_update.mbox        = OSMboxCreate((void *)0);
}

static void update_bin_start(update_ctrl_t *pBin)
{
	update_bin.state = DEF_Busy;
    pBin->burn_last = DEF_No;
    pBin->update_is_ok = DEF_No;
    pBin->burn_addr = ApplicationAddress;
    pBin->burn_cnt  = 0;
    pBin->size_cnt  = 0;                        // Firmware bin文件计数器
    pBin->calcsum   = 0;
    pBin->chksum    = 0;                        // Firmware bin校验和
    pBin->frame_cnt = 0;                        // Firmware bin帧计数器
	
    if (pBin->size_total >0 && pBin->size_total < TransmitFileMaxSize) {
		pBin->ack = ACK_OK;               // Firmware bin烧写ACK
		pBin->burn_start = DEF_Yes;
    } else {	
		pBin->ack = ACK_Error;
		pBin->burn_start = DEF_No;
	}
    update_bin.state = DEF_Idle;
}

//static void update_bin_transmit(update_ctrl_t *pBin)
//{
//    pBin->update_is_ok = DEF_No;
//    pBin->ack = ACK_OK;
//}

static void update_bin_end(update_ctrl_t *pBin)
{
	pBin->state = DEF_Busy;
    pBin->update_is_ok = DEF_No;
    if ((pBin->calcsum == pBin->chksum)&& (pBin->size_cnt == pBin->size_total)) {
        pBin->ack   = ACK_OK;      
        pBin->burn_last = DEF_Yes;
    } else {
        pBin->ack = ACK_Error;            // receive bin length is not match
        pBin->burn_start = DEF_No;
    }
	update_bin.state = DEF_Idle;
}

static INT8S copy_check_bin(update_ctrl_t *pBin, const INT8U *pdata, INT32U len)
{
    //INT8U   data;
    INT8U  *pbuf;
    INT32U  idx = len;
    INT32U  chksum = 0;
    char buf[0x10];
        
    if(pBin->frame_cnt==1)      {
      memcpy(buf,(const char*)pdata,0x10);
      if(0==strcmp(buf,CONFIG_SYSINFO_Manufacturer))  {
        memcpy(buf,(const char*)(pdata+0x10),0x10);
        if(0==strcmp(buf,CONFIG_SYSINFO_Name))        {
          ;
        }
        else    {
          return 0;
        }
      }
      else      {
        return 0;
      }
	  if(!FLASH_Erase())	{//擦除固件ok
		  return 0;
	  }
    }
    pbuf = &pBin->buf_addr[pBin->burn_cnt];
    while (idx--) {
        chksum += *pdata;
       *pbuf++  = *pdata++;
    }

    pBin->calcsum  += chksum;
    pBin->burn_cnt += len;
    pBin->size_cnt += len;
	if(FLASH_Program(pbuf, len)==0)	{//写固件	
		return 0;
	}
    return 1;
}


/*F******************************************************************************************************
*
********************************************************************************************************/
void FirmwareBinOpt(update_ctrl_t *pBin, message_pkt_t *pmsg)
{
    INT8U  *pdata;
    INT32U  len;
    BIT32 tmp;

    pdata = (INT8U *)pmsg->Data;
    switch (pdata[0])
    {
        case UpdateCmd_Start:
            if (pBin->state == DEF_Idle) {
//				pBin->trans_type = pdata[1];//传输文件类型
                tmp.ubyte[0] = pdata[1];
                tmp.ubyte[1] = pdata[2];
                tmp.ubyte[2] = pdata[3];
                tmp.ubyte[3] = pdata[4];
                pBin->size_total = tmp.uword;           // Firmware bin文件的大小
				pBin->call_begin(pBin);
            }
            break;
        case UpdateCmd_Transmit:
            if ((pBin->state == DEF_Idle)&& (pBin->burn_start == DEF_Yes)) {             
				 update_bin.state = DEF_Busy;
                len = pmsg->dLen - 1;
                pBin->frame_cnt++;
                if(copy_check_bin(pBin, &pdata[1], len))        {
					pBin->update_is_ok = DEF_No;
					pBin->ack = ACK_OK;
                }
                else    {
					pBin->ack = ACK_Error;
					pBin->burn_start = DEF_No;
                }
				update_bin.state = DEF_Idle;
            }
            break;
        case UpdateCmd_End:
            if ((pBin->state == DEF_Idle)&& (pBin->burn_start == DEF_Yes)) {            
                tmp.ubyte[0] = pdata[1];
                tmp.ubyte[1] = pdata[2];
                tmp.ubyte[2] = pdata[3];
                tmp.ubyte[3] = pdata[4];
                pBin->chksum = tmp.uword;
                pBin->call_end(pBin);
            }else    {
                pBin->ack = ACK_Error;
                pBin->burn_start = DEF_No;
            }
            break;
        case UpdateCmd_Query:
			if(pBin->state == DEF_Busy)	{
				pBin->ack = ACK_BUSY;				
			} else	{
				pBin->ack = ACK_OK;		
			}
        default:
            break;
    }

    pmsg->dLen = 1;
    pmsg->Data    = (void *)(&pBin->ack);   
}

u8 GetUpdateState(void)
{
	return update_bin.state;
}

//void update_led(void)
//{
//  if(autosampler_bin.burn_start)        {
//    system_red_twinkle();
//  }
//  else  {
//    system_work_status(autosampler_bin.ack);
//  }
//}

uint32_t PageError = 0;
uint32_t f_status;
u8 FLASH_Erase(void)
{
	FLASH_EraseInitTypeDef f;
		
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.Page = 30;//固件存储地址0x0800F000
	f.Banks = FLASH_BANK_1;
	if(update_bin.size_total%FLASH_PAGE_SIZE != 0)
		f.NbPages = update_bin.size_total/FLASH_PAGE_SIZE + 1;
	else
		f.NbPages = update_bin.size_total/FLASH_PAGE_SIZE;
	HAL_FLASH_Unlock();
	HAL_FLASHEx_Erase(&f, &PageError);	//调用擦除函数
//	HAL_FLASH_Lock();
	if(PageError == HAL_FLASH_ERROR_NONE||PageError == 0xFFFFFFFF)	{//0xFFFFFFFF means that all the pages have been correctly erased
		return 1;
	}
	return 0;
}

u8 FLASH_Program(u8 *pbuf, u32 size)
{
	u32 idx,temp;
	u8 delay_cnt;
	
	delay_cnt = 0;
	for (idx = 0; idx < size; idx += 8)
	{
		/* Program the data received into STM32F10x Flash */
		f_status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, update_bin.burn_addr, *(INT64U *)&pbuf[idx]);
		if(f_status == HAL_OK)	{
//			SYS_PRINTF("Updating, addr:0x%x.",updata_bin.burn_addr);	
			update_bin.burn_addr += 8;
		}
		else	{
			HAL_Delay(30);
			delay_cnt ++;
			if(delay_cnt>100)	{
				return 0;
			}
		}
	}
	temp = size%8;
	if(temp!=0)	{
		u8 buff[8] = {0};
		memcpy(buff,pbuf + (size - temp), temp);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, update_bin.burn_addr, *(INT64U *)buff);
	}
	return 1;
}

#include "rw_udisk.h"
#include "DaCai_API.h"
#define	UPDATE_FW_NAME	"PCR16_UpdateFW.bin"
#define	UPDATE_FW_ATTR_SIZE		0X10
#define	UPDATE_FW_FILE_MINSIZE		0X400
u8 CheckUpdateFWFromUdisk(void)
{
	FRESULT res;
	char file_name[FILE_NAME_LEN];
	char FW_attr[UPDATE_FW_ATTR_SIZE];
	u32 rsize;
	u8 ret=0;
	
	sprintf(file_name, "%s%s", USERPath, UPDATE_FW_NAME);//固件路径
	res = f_open(&udiskfs.fil, file_name, FA_READ);
	if(res==FR_OK)	{
		update_bin.size_total = f_size(&udiskfs.fil);
		if(update_bin.size_total < UPDATE_FW_FILE_MINSIZE)	{//固件太小 判定为有问题固件
			f_close(&udiskfs.fil);
			return ret;
		}
		f_read(&udiskfs.fil, FW_attr, UPDATE_FW_ATTR_SIZE, &rsize);
		if(0==strcmp(FW_attr, CONFIG_SYSINFO_Manufacturer))  {//校验公司名称
			f_read(&udiskfs.fil, FW_attr, UPDATE_FW_ATTR_SIZE, &rsize);
			if(0==strcmp(FW_attr, CONFIG_SYSINFO_Name))        {//校验设备名称
				SYS_PRINTF("FW check ok");
				ret = 1;
			}
		}
		f_close(&udiskfs.fil);
	}
	SYS_PRINTF("FW check failed");
	return ret;
}

u8 UpdateFWFromUdisk(void)
{
	FRESULT res;
	char file_name[FILE_NAME_LEN];
	u32 rsize;
	u8 ret=0;
	u32 percent;	
	u8 *fw_buf;
	
	sprintf(file_name, "%s%s", USERPath, UPDATE_FW_NAME);//固件路径
	res = f_open(&udiskfs.fil, (const char *)file_name, FA_READ);
	if(res==FR_OK)	{
		if(FLASH_Erase())	{//擦除固件
			SYS_PRINTF("FLASH_Erase ok.");
			update_bin.size_total = f_size(&udiskfs.fil);
			update_bin.size_cnt = 0;
			update_bin.burn_addr = ApplicationAddress;
			SYS_PRINTF("Update start");
			fw_buf = update_bin.buf_addr;
			if(fw_buf==NULL)
				goto _exit;
			for(;;)	{
				res =  f_read(&udiskfs.fil, fw_buf, BIN_BUFF_MAX_SIZE, &rsize);
				if(res != FR_OK) goto _exit;
				if(rsize==0)	break;
				if(FLASH_Program(fw_buf, rsize)==1)	
				{
					update_bin.size_cnt += rsize;
					percent = (update_bin.size_cnt*100)/update_bin.size_total;
					DaCai_UpdateProgressBar(&UI, percent);	//更新进度条
					SYS_PRINTF("Updating, addr:0x%x, per:%d %%",update_bin.burn_addr, percent);						
					if(update_bin.size_cnt == update_bin.size_total)	{//烧写成功
						SYS_PRINTF("Update ok.");
						ret = 1;
						break;
					}
				}
				else	{
					goto _exit;
				}
			}
		}
	}
_exit:
	f_close(&udiskfs.fil);
	fw_buf = NULL;
	HAL_FLASH_Lock();
	return ret;
}

/*F******************************************************************************************************
*                                               start task
********************************************************************************************************/
static void TaskSystemUpdate (void *parg)
{
 	u8 err;
	message_pkt_t *msg;
	parg = parg;
	
    UpdateInit();

	for(;;)	
	{
		msg = (message_pkt_t *)OSMboxPend(app_system_update.mbox, 0, &err);
		if(err==OS_ERR_NONE)    {
			if(msg->Src == MSG_UPDATE_FROM_UDISK)	{//从udisk升级固件
				update_bin.state = DEF_Busy;
				if(CheckUpdateFWFromUdisk())	{
					if(UpdateFWFromUdisk()==1)	{	//升级成功
						FWUpdate_reboot();//重启  重启前擦除RunIAPKeyword
					}
				}
				update_bin.state = DEF_Idle;
			}
			else if(msg->Src == _CMD_FILETRANSMIT_DOWNLOAD)	{
				FirmwareBinOpt(&update_bin, msg);
			}
			else if(msg->Src == MSG_JUMP_APP)	{
				FWUpdate_reboot();//重启  重启前擦除RunIAPKeyword
			}
		}
	}
}


/*
*********************************************************************************************************
*                                               No More!
*********************************************************************************************************
*/
