#ifndef APP_SYSTEM_UPDATE_H
#define APP_SYSTEM_UPDATE_H

#include "includes.h"
#include "protocol.h"

#define	TransmitFileMaxSize 	102400//100kb

typedef enum {
    UpdateCmd_Query     = 0x01,     // 查询状态
    UpdateCmd_Start     = 0x02,     // 传输启动
    UpdateCmd_Transmit  = 0x03,     // 传输bin文件
    UpdateCmd_End       = 0x04,     // 传输结束
    UpdateCmd_JumpToIap = 0x05,     // jump to IAP
    UpdateCmd_JumpToApp = 0x06      // jump to APP
} update_cmd_t;

typedef struct _update_ctrl {
    INT8U   state;                          // Firmware bin烧写状态
    INT8U   ack;                            // Firmware bin烧写ACK
    INT8U   burn_start;                     // 烧写启动
    INT8U   burn_last;                      // 烧写结束
    INT8U   update_is_ok;                   // 烧写是否成功
    INT16U  burn_cnt;                       // 一次烧写接收计数器
    INT8U  *const buf_addr;                 // Firmware bin缓存起始地址
    INT32U  burn_addr;                      // 烧写地址
    INT32U const size_max;                  // 可存储最大量
    INT32U  size_total;                     // Firmware bin文件的大小
    INT32U  size_cnt;                       // Firmware bin文件计数器
    INT32U  frame_cnt;                      // Firmware bin帧计数器
    INT32U  calcsum;                        // Firmware bin计算校验和
    INT32U  chksum;                         // Firmware bin校验和
//	INT8U	trans_type;
    void (* const call_begin)(struct _update_ctrl *);     // 启动回调
    void (* const call_transmit)(struct _update_ctrl *);  // 传输回调
    void (* const call_end)(struct _update_ctrl *);       // 结束回调
} update_ctrl_t;

typedef struct _system_update	{
	OS_EVENT           *mbox;
}_system_update_t;
extern _system_update_t app_system_update;


void    SystemUpdateTaskInit    (void);
void    run_application         (void);
void    FirmwareBinOpt          (update_ctrl_t *pBin, message_pkt_t *pmsg);
u8 		GetUpdateState(void);
#endif    /* APP_SYSTEM_UPDATE_H */

/*
*********************************************************************************************************
*                                                No More!
*********************************************************************************************************
*/
