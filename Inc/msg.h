#ifndef __MSG_H__
#define __MSG_H__

#include    "ucos_ii.h"
#include    "os_cpu.h"
#include    "os_cfg.h"
#include 		"sys_defs.h"
#include 		"sys_types.h"
#include 		"sys_bits.h"

typedef enum _message_src{
    /* Task Mbox */
    USART_MSG_RX_TASK = 0,      // 邮箱消息来自Usart Rx(串口接收)任务(Mbox From Usart Message Receive Task)
	USART_MSG_TX_TASK,
    MSG_USB_DISCONNECT,   //u盘拔出事件
    MSG_USB_READY,       // u盘事件
	MSG_USB_START,	// u盘事件
    MSG_SYSTEM_RESTART,//重启
	MSG_CaliTemplateHolePD_EVENT,//校准空孔pd最大值 最小值
	MSG_CaliHolePostion_EVENT,//孔位置校准
	MSG_WriteLabTemplate,
	MSG_WRITELOG,
	MSG_SB_OVC_EVENT,
	MSG_USB_OVC_EVENT,
	MSG_SYS_SHUTDOWN,
	MSG_KEYEVENT,
	MSG_COPY_LOGFILE,
	MSG_COPY_PERFUSEFILE,
	MSG_COPY_UPDATE_FW,
	MSG_MESSAGE_DSIPLAY,
	MSG_WARNING_DSIPLAY,
	MSG_NONE_DSIPLAY,
} message_src_t;

typedef struct _message_pkt {
    message_src_t   Src;        // 消息来源
    u8           Cmd;        // 命令
//    u16 			 pLen;		//参数长度
    u16          dLen;    // 消息长度
    void         *Data;       // 消息参数
} message_pkt_t;

#define FLAG_GRP_1		0X01
#define FLAG_GRP_2		0X02
#define FLAG_GRP_3		0X04
#define FLAG_GRP_4		0X08

extern OS_FLAG_GRP    *SysFlagGrp;

void msg_init(void);
void mutex_lock(OS_EVENT *lock);
void mutex_unlock(OS_EVENT *lock);
INT8U  UsartRxGetINT8U(u8 *buf,INT32U *idx);
INT16U  UsartRxGetINT16U(u8 *buf,INT32U *idx);
INT32U  UsartRxGetINT32U(u8 *buf,INT32U *idx);
u16 crc16(u8 *buf,u8 len);


#endif
