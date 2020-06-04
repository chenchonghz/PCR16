#include "includes.h"
#include "app_usart.h"
#include "app_display.h"
#include "protocol.h"
#include  "app_system_update.h"

static u8 data_buf[20];

u8 UsartCmdProcess(usart_t *pUsart, message_pkt_t msg[])
{
	u8 iPara,data,idx;
	u8 ack_state;
	s32 temp;
	
	ack_state = ACK_NONE;
	switch(msg[0].Cmd)	{
		case _CMD_RW_SYS_INFOR:
			break;
		case _CMD_EXECUTE_SYS_INFOR:
		
			break;	
		case _CMD_READ_DevState://0X03,//读取设备运行状态
			idx = 0;
			data_buf[idx++] = Sys.devstate;
			data_buf[idx++] = Sys.devsubstate;
			msg[1].Data = data_buf;
			msg[1].dLen = idx;
			OSMboxPost(usart.mbox, &msg[1]);
			break;
		case _CMD_FILETRANSMIT_DOWNLOAD://0X0A,//下载文件
			if(GetUpdateState() == DEF_Busy)	{
				ack_state = ACK_BUSY;
				break;
			}
			msg[0].Src = MSG_FILETRANSMIT_DOWNLOAD;
			msg[0].Data = (u8 *)(pUsart->rx_buf+pUsart->rx_idx);
			msg[0].dLen = pUsart->rx_cnt - 1;
			OSMboxPost(app_system_update.mbox, &msg[0]);
			break;
		case _CMD_UPDATE_FW://0x0c 固件升级
			if(GetUpdateState() == DEF_Busy)	{
				ack_state = ACK_BUSY;
				break;
			}
			iPara = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
			if(iPara==1)	{//主板固件升级
				msg[0].Data = (u8 *)(pUsart->rx_buf+pUsart->rx_idx);
				msg[0].dLen = pUsart->rx_cnt - 1;
				msg[0].Src = MSG_JUMP_APP;//跳转至app
				OSMboxPost(app_system_update.mbox, &msg[0]);
			}
			ack_state = ACK_OK;
			break;
		default:
			break;
	}
	return ack_state;
}

