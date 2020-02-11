#include "Motor_Task.h"

static  message_pkt_t    msg_pkt_motor;

void Motor_TaskInit(void)
{
	Motor_init();
}

void Motor_Task(void)
{
	message_pkt_t *msg;
	
	if(!(SysTask.flag&TASK_MOTOR_RUN))
		return;
	msg = RecMsgPkt();
	if(msg->Src == MSG_MOTOR_ACTION)	{
		
	}
}

