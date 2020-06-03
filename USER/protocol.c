#include "includes.h"
#include "app_usart.h"
#include "app_display.h"
#include "protocol.h"

static u8 data_buf[100];

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
		
		default:
			break;
	}
	return ack_state;
}

