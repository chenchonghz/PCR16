#include "task_manage.h"
#include "Udisk_Task.h"
#include "Flash_Task.h"
#include "Motor_Task.h"
#include "AD_Task.h"
#include "TempCtrl_Task.h"

_sys_task_t SysTask;
message_pkt_t *TaskMsg;

void Task_DataInit(void)
{
	SysTask.flag = TASK_NONE_RUN;
}

void Task_Init(void)
{
	Flash_TaskInit();
	Motor_TaskInit();
	AD_TaskInit();
	TempCtrl_TaskInit();
}

message_pkt_t *RecMsgPkt(void)
{
	return TaskMsg;
}

void SendMsgPkt(message_pkt_t *msg)
{
	TaskMsg = msg;
}
