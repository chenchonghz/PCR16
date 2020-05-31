#include "app_filetransmit.h"
#include "app_usart.h"
#include "rw_spiflash.h"

__align(4) OS_STK  TASK_FILETRANSMIT_STK[STK_SIZE_FILETRANSMIT]; //任务堆栈声?
u8 transmit_buff[TransmitMaxBufSize];

app_filetransmit_t	app_filetransmit;
static  message_pkt_t    msg_pkt_filetrans;

static void TaskFileTransmit(void * ppdata);
static void upload_start(transmit_ctrl_t *ptfile);
static void upload_transmit(transmit_ctrl_t *ptfile);
static void upload_end(transmit_ctrl_t *ptfile);

transmit_ctrl_t   transmit_ctrl = {
    .state         = DEF_Idle,
    .ack           = ACK_OK,
    .trans_is_ok  = DEF_No,
    .trans_cnt      = 0,
    .buf_addr      = transmit_buff,
    .filepath     = {0},
	.fp			  = NULL,
    .size_max      = TransmitMaxBufSize,
    .size_total    = 0,
    .size_cnt      = 0,
    .frame_cnt     = 0,
	.frame_total = 0,
    .calcsum       = 0,
    .chksum        = 0,
    .call_begin    = upload_start,
    .call_transmit = upload_transmit,
    .call_end      = upload_end
};

void AppFileTransmitInit(void)
{
	OSTaskCreate(TaskFileTransmit,  (void * )0, (OS_STK *)&TASK_FILETRANSMIT_STK[STK_SIZE_FILETRANSMIT-1], TASK_PRIO_FILETRANSMIT);
}

static void DatInit(void)
{
	app_filetransmit.Mbox                = OSMboxCreate((void *)0);
}
//文件上传开始
static void upload_start(transmit_ctrl_t *ptfile)
{
	FRESULT res;
	
	ptfile->state = DEF_Busy;
    ptfile->trans_last = DEF_No;
    ptfile->trans_is_ok = DEF_No;
	ptfile->trans_cnt = 0;
    ptfile->size_total  = 0;
    ptfile->size_cnt  = 6;                        // 计数器
    ptfile->calcsum   = 0;
    ptfile->chksum    = 0;                        //  校验和
    ptfile->frame_cnt = 0;                        // 帧计数器
	ptfile->frame_total = 0;
		
//	ptfile->fp = fopen((const char *)ptfile->filepath, "r");	
	res = f_open(flashfs.fil, (const char *)ptfile->filepath, FA_READ);
	if(res != FR_OK)	{//文件不存在 回复文件大小0
		memset(ptfile->buf_addr, 0, ptfile->size_cnt);
		ptfile->trans_start = DEF_No;
	}
	else	{
//		fseek(ptfile->fp, 0L, SEEK_END);
//		ptfile->size_total = ftell(ptfile->fp);
		ptfile->size_total = f_size(flashfs.fil);
		memcpy(ptfile->buf_addr, (u8 *)&ptfile->size_total, 4);//文件大小
		ptfile->frame_total = ptfile->size_total/TransmitMaxBufSize;
		if((ptfile->size_total%TransmitMaxBufSize)>0)	ptfile->frame_total += 1;//文件分几个序列传输
		memcpy(ptfile->buf_addr+4, (u8 *)&ptfile->frame_total, 2);
		ptfile->trans_start = DEF_Yes;
	}
    ptfile->ack = ACK_NONE;               // 烧写ACK  
	ptfile->state = DEF_Idle;	
}
//文件上传
static void upload_transmit(transmit_ctrl_t *ptfile)
{
	INT8U *pbuf;
	u32 i;
	INT32U  chksum = 0;
	
	ptfile->state = DEF_Busy;
	if(ptfile->frame_cnt >= ptfile->frame_total)
		ptfile->call_end(ptfile);
	else	{
		ptfile->ack = ACK_NONE;
		memcpy(ptfile->buf_addr, (u8 *)&ptfile->frame_cnt, 2);//上传帧序列号
		pbuf = ptfile->buf_addr+2;
//		ptfile->size_cnt = fread(pbuf, 1, TransmitMaxBufSize, ptfile->fp);//上传帧内容 每帧最大1kb
		f_read(flashfs.fil, pbuf, TransmitMaxBufSize, &ptfile->size_cnt);
		for(i=0;i<ptfile->size_cnt;i++)	{//计算校验和
			 chksum += pbuf[i];
		}
		ptfile->calcsum = chksum;
		ptfile->frame_cnt ++;
		ptfile->trans_cnt += ptfile->size_cnt;
		ptfile->size_cnt += 2;
	}
	ptfile->state = DEF_Idle;	
}
//文件上传结束
static void upload_end(transmit_ctrl_t *ptfile)
{
	ptfile->state = DEF_Busy;
		
	if(ptfile->trans_cnt == ptfile->size_total&&ptfile->calcsum == ptfile->chksum)	{//传输文件大小和校验和ok 传输完成
		ptfile->ack = ACK_OK;
	}else	{
		ptfile->ack = ACK_Error;
	}
	if(ptfile->fp)
//		fclose(ptfile->fp);
		f_close(flashfs.fil);
	ptfile->state = DEF_Idle;
}
//上位机读取文件操作
void UploadFileOpt(transmit_ctrl_t *ptfile, message_pkt_t *pmsg)
{
    INT8U  *pdata;
    INT32U  len;
    BIT32 tmp;

	if(ptfile->state == DEF_Busy)	{
		ptfile->ack = ACK_BUSY;
		goto _end;
	}
    pdata = (INT8U *)pmsg->Data;
    switch (pdata[0])
    {
        case TransmitCmd_Start://传输启动
            if (ptfile->state == DEF_Idle) {
				if(strlen((char *)&pdata[1]) >= 64)	{
					ptfile->ack = ACK_Error;
					break;
				}
				strcpy(ptfile->filepath, (char *)&pdata[1]);//上传文件路径
				ptfile->call_begin    = &upload_start;
				ptfile->call_transmit = &upload_transmit;
				ptfile->call_end      = &upload_end;
                ptfile->call_begin(ptfile);
            }
            break;
        case TransmitCmd_Transmit://请求传输
            if (ptfile->trans_start == DEF_Yes) {          
                ptfile->call_transmit(ptfile);    
			}				
            else    {
                ptfile->ack = ACK_Error;
                ptfile->trans_start = DEF_No;
            }
            break;
        case TransmitCmd_End://传输结束
            if (ptfile->trans_start == DEF_Yes) {
                tmp.ubyte[0] = pdata[1];
                tmp.ubyte[1] = pdata[2];
                tmp.ubyte[2] = pdata[3];
                tmp.ubyte[3] = pdata[4];
                ptfile->chksum = tmp.uword;
                ptfile->call_end(ptfile);
            }else    {
                ptfile->ack = ACK_Error;
                ptfile->trans_start = DEF_No;
            }
            break;
        case TransmitCmd_Query:
			if(ptfile->state == DEF_Busy)	{
				ptfile->ack = ACK_BUSY;				
			} else	{
				ptfile->ack = ACK_OK;		
			}
			break;
        default:
            break;
    }
_end:
	msg_pkt_filetrans.Src = USART_MSG_RX_TASK;
	msg_pkt_filetrans.Cmd = _CMD_FILETRANSMIT_UPLOAD;
	if(ptfile->ack == ACK_NONE)	{
		msg_pkt_filetrans.dLen = ptfile->size_cnt;
		msg_pkt_filetrans.Data    = ptfile->buf_addr;
		OSMboxPost(usart.mbox, &msg_pkt_filetrans);
	}
	else	{
		UsartSendAck(&msg_pkt_filetrans, ptfile->ack);
	}
}

void DownloadFileOpt(transmit_ctrl_t *ptfile, message_pkt_t *pmsg)
{
    INT8U  *pdata;
    INT32U  len;
    BIT32 tmp;

	if(ptfile->state == DEF_Busy)	{
		ptfile->ack = ACK_BUSY;
		goto _end;
	}
    pdata = (INT8U *)pmsg->Data;
    switch (pdata[0])
    {
        case TransmitCmd_Start:
            if (ptfile->state == DEF_Idle) {
				strcpy(ptfile->filepath, (char *)pdata);
				ptfile->call_begin    = &upload_start;
				ptfile->call_transmit = &upload_transmit;
				ptfile->call_end      = &upload_end;
                ptfile->call_begin(ptfile);
            }
            break;
        case TransmitCmd_Transmit:
            if (ptfile->trans_start == DEF_Yes) {          
                ptfile->call_transmit(ptfile);    
			}				
            else    {
                ptfile->ack = ACK_Error;
                ptfile->trans_start = DEF_No;
            }
            break;
        case TransmitCmd_End:
            if (ptfile->trans_start == DEF_Yes) {
                tmp.ubyte[0] = pdata[1];
                tmp.ubyte[1] = pdata[2];
                tmp.ubyte[2] = pdata[3];
                tmp.ubyte[3] = pdata[4];
                ptfile->chksum = tmp.uword;
                ptfile->call_end(ptfile);
            }else    {
                ptfile->ack = ACK_Error;
                ptfile->trans_start = DEF_No;
            }
            break;
        case TransmitCmd_Query:
			if(ptfile->state == DEF_Busy)	{
				ptfile->ack = ACK_BUSY;				
			} else	{
				ptfile->ack = ACK_OK;		
			}
			break;
        default:
            break;
    }
_end:
	msg_pkt_filetrans.Src = USART_MSG_RX_TASK;
	msg_pkt_filetrans.Cmd = _CMD_FILETRANSMIT_DOWNLOAD;
	if(ptfile->ack == ACK_NONE)	{
		msg_pkt_filetrans.dLen = ptfile->size_cnt;
		msg_pkt_filetrans.Data    = ptfile->buf_addr;
		msg_pkt_filetrans.Src = USART_MSG_RX_TASK;
		OSMboxPost(usart.mbox, &msg_pkt_filetrans);
	}
	else	{
		msg_pkt_filetrans.Src = USART_MSG_RX_TASK;
		UsartSendAck(&msg_pkt_filetrans, ptfile->ack);
	}
}

static void TaskFileTransmit(void * ppdata)
{
	u8 err;
	message_pkt_t *msg;
	ppdata = ppdata;
	
	DatInit();
	
	for(;;)
	{
		msg = (message_pkt_t *)OSMboxPend(app_filetransmit.Mbox, 0, &err);
		if(msg->Src == MSG_FILETRANSMIT_UPLOAD)	{//文件上传操作
			UploadFileOpt(&transmit_ctrl, msg);
		}
		else if(msg->Src == MSG_FILETRANSMIT_DOWNLOAD)	{//文件下载操作
			DownloadFileOpt(&transmit_ctrl, msg);
		}
	}
}

