#ifndef __APP_FILETRANSMIT_H__
#define __APP_FILETRANSMIT_H__

#include "includes.h"

#define	TransmitBufMaxSize 	1024
#define	TransmitFileMaxSize 	102400//100kb
#define	TransmitFilePathSize 	32

typedef struct _app_filetransmit_t {
    OS_EVENT            *Mbox;
}app_filetransmit_t;

typedef enum {
    TransmitCmd_Query     = 0x01,     // 查询状态
    TransmitCmd_Start     = 0x02,     // 传输启动
    TransmitCmd_Transmit  = 0x03,     // 传输bin文件
    TransmitCmd_End       = 0x04,     // 传输结束
} Transmit_cmd_t;

typedef enum {
	TransmitType_Unkown = 0,
	TransmitType_FW = 1,
	TransmitType_File = 2,
}TransmitType;

typedef enum {
	TYPE_JumpToIap = 0X05,////jump to IAP
	TYPE_JumpToApp = 0x06,//jump to APP
} UpdateType;


typedef struct _transmit_ctrl {
    INT8U   state;                          // transmit状态
    INT8U   ack;                            // transmit ACK
    INT8U   trans_start;                     // transmit启动
    INT8U   trans_type;                      // transmit结束
    INT8U   trans_is_ok;                   // transmit是否成功
    INT16U  trans_cnt;                       // 一次收发计数器
    INT8U  *const buf_addr;                 // transmit缓存起始地址
    char  filepath[TransmitFilePathSize];                      // transmit 文件保存路径
	FILE	*fp;
    INT32U const size_max;                  // 可存储最大量
    INT32U  size_total;                     // 文件总大小
    INT32U  size_cnt;                       // 单帧字节数
    INT32U  frame_cnt;                      // transmit帧计数器
	INT32U  frame_total;
    INT32U  calcsum;                        // transmit计算校验和
    INT32U  chksum;                         // 校验和
    void (* call_begin)(struct _transmit_ctrl *);     // 启动回调
    void (* call_transmit)(struct _transmit_ctrl *);  // 传输回调
    void (* call_end)(struct _transmit_ctrl *);       // 结束回调
} transmit_ctrl_t;

extern app_filetransmit_t	app_filetransmit;
void AppFileTransmitInit(void);
u8 GetTransmitState(void);
#endif

