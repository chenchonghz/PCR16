#ifndef __TASK_MANAGE_H__
#define __TASK_MANAGE_H__

#include "includes.h"

#define TASK_NONE_RUN		DEF_BIT00_MASK
#define TASK_MOTOR_RUN		DEF_BIT01_MASK
#define TASK_AD_RUN			DEF_BIT02_MASK
#define TASK_FLASH_RUN		DEF_BIT03_MASK
#define TASK_UDISK_RUN		DEF_BIT04_MASK

typedef enum _message_src{
    /* Task Mbox */
    USART_MSG_RX_TASK = 0,      // 邮箱消息来自Usart Rx(串口接收)任务(Mbox From Usart Message Receive Task)
	USART_MSG_TX_TASK,
    SYSTEM_MONITOR_TASK,        // 邮箱消息来自System Monitor(系统监控)任务(Mbox From System Monitor Task)
    MSG_MOTOR_ACTION,//
    MSG_SYSTEM_RESTART,//重启
	MSG_WRITELOG,
	MSG_SB_OVC_EVENT,
	MSG_USB_OVC_EVENT,
	MSG_SYS_SHUTDOWN,
	MSG_KEYEVENT,
	MSG_COPY_LOGFILE,
	MSG_COPY_PERFUSEFILE,
	MSG_COPY_UPDATE_FW,
	MSG_MESSAGE_DSIPLAY,
	MSG_WARNING_DSIPLAY
} message_src_t;

typedef struct _message_pkt {
    message_src_t   Src;        // 消息来源
    u8           Cmd;        // 命令
//    u16 			 pLen;		//参数长度
    u16          dLen;    // 消息长度
    void         *Data;       // 消息参数
} message_pkt_t;

typedef struct _sys_task	{
	volatile u32 flag;
}_sys_task_t;
extern _sys_task_t SysTask;

void Task_DataInit(void);
void Task_Init(void);
message_pkt_t *RecMsgPkt(void);
void SendMsgPkt(message_pkt_t *msg);
#endif
