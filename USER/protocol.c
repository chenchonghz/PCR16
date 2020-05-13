#include "includes.h"
#include "app_usart.h"
#include "app_display.h"
#include "app_motor.h"
#include "protocol.h"
#include "PD_DataProcess.h"

static  message_pkt_t    msg_pkt_pro;
static u8 data_buf[15];

u8 UsartCmdProcess(usart_t *pUsart, message_pkt_t *msg)
{
	u8 iPara,data;
	u8 ack_state;
	
	ack_state = ACK_OK;
	iPara = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
	switch(msg->Cmd)	{
		case _CMD_RW_SYS_INFOR:
			break;
		case _CMD_EXECUTE_SYS_INFOR:
		
			break;				
		case _CMD_CALIBRATE://0x08 执行校准
			if(iPara==0)	{//空孔荧光值校准				
				msg_pkt_pro.Src = MSG_CollTemplateHolePD_EVENT;//启动电机 开始采集空孔PD最大值 最小值
				OSMboxPost(tMotor[MOTOR_ID1].Mbox, &msg_pkt_pro);	
			}
			else if(iPara==1)	{//孔位置校准				
				msg_pkt_pro.Src = MSG_CaliHolePostion_EVENT;//启动电机 开始校准孔位置
				OSMboxPost(tMotor[MOTOR_ID1].Mbox, &msg_pkt_pro);	
			}
			else if(iPara==2)	{
				msg_pkt_pro.Src = MSG_CollectHolePD_EVENT;//启动电机 开始采集孔PD值
				OSMboxPost(tMotor[MOTOR_ID1].Mbox, &msg_pkt_pro);	
			}
			break;
		case _CMD_LED_CTRL://0x0d LED灯控制
		{
			u8 led_type;
			led_type = LED_NONE;
			data = UsartRxGetINT8U(pUsart->rx_buf,&pUsart->rx_idx);
			if(iPara==0)	{//0--蓝色灯，1—绿色灯，2—所有灯
				led_type = LED_BLUE;
			}
			else if(iPara==1)	{
				led_type = LED_GREEN;
			}
			else if(iPara==2)	{
				led_type = LED_GREEN | LED_BLUE;
			}
			FluoLED_OnOff(led_type, data);//0—灭，1—点亮
			break;
		}
		default:
			break;
	}
	return ack_state;
}

