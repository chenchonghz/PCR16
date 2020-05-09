#include "includes.h"
#include "app_usart.h"
#include "app_display.h"
//#include "sys_info.h"
#include "protocol.h"

static  message_pkt_t    msg_pkt_pro;
static u8 data_buf[15];

u8 UsartCmdProcess(usart_t *pUsart, message_pkt_t *msg)
{
	u8 cmd;
	u8 ack_state;
	
	ack_state = ACK_OK;
	cmd = msg->Cmd;
	switch(cmd)	{
		case _CMD_RW_SYS_INFOR:
			break;
		case _CMD_EXECUTE_SYS_INFOR:
		
			break;				
		case _CMD_CALIBRATE:
			break;
		default:
			break;
	}
	return ack_state;
}

