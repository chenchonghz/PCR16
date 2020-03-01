#ifndef __DACAI_PROTOANALYSIS_H__
#define __DACAI_PROTOANALYSIS_H__

#include "includes.h"
#include "rw_spiflash.h"

#define 	SENDBUF		20
#define 	DACAITXTBUF_SIZE		1024//(LOG_DISPLAY_SIZE)
#define 	DACAIUSART_RXSIZE		50
#define 	DACAIUSART_TXSIZE		(DACAITXTBUF_SIZE)//串口屏串口发送缓存

#define DaCaiHEND		0XEE//针头
#define CMD_HANDSHAKE		0X04
#define HANDSHAKE_OK		0X55
#define RESTART_OK		0X07

#define Invalid_UIID						(-1)
#define Welcome_UIID						0//欢迎使用界面id
#define Main_UIID							1//主界面
#define Menu_UIID						2//菜单界面
#define SysSetting_UIID						3//系统设置界面
#define PerfuseSetting_UIID					4//灌注参数 设置界面
#define LOG_UIID							5//LOG界面
#define INFOR_UIID							6//
#define HELP_UIID							7//HELP
#define ERROR_UIID							14
#define WARNING_UIID						12
#define MESSAGE_UIID						13
#define PerfuseData_UIID						16//显示灌注数据界面

#define BUTTON_RELEASE	DEF_False
#define BUTTON_PRESS		DEF_True
#define DynamicCtrl_Enable		DEF_False
#define DynamicCtrl_Disable		DEF_True

#define  DACAI_PROTOCOL_RX_SD0        0xEE           /* Start delimiters                                */
//#define  IG_PROTOCOL_RX_SD1        0x7E
#define  DACAI_PROTOCOL_RX_END0        0xFF           /* End   delimiter                                 */
#define  DACAI_PROTOCOL_RX_END1        0xFC 
#define  DACAI_PROTOCOL_RX_END2        0xFF 
#define  DACAI_PROTOCOL_RX_END3        0xFF 

#define DEF_OFFLINE         0u
#define DEF_ONLINE          1u
#define DEF_STANDBY          2u

typedef struct _dacai_usart{
	volatile INT8U      rx_state;
	INT8U				endflag;
	INT32U              rx_idx;
	INT32U              rx_cnt;
	INT32U              rx_len;
	INT32U              rx_bufsize;
	INT16U              rx_crc;
	INT16U              rx_err;
	INT8U              *rx_buf;
	void (*rx_indicate)(struct _dacai_usart *pUsart, INT8U rx_dat);
	
	INT8U				*tx_buf;
	INT32U              tx_bufsize;
}_dacai_usart_t;

typedef struct _DACAI	{
	OS_EVENT		*lock;
	UART_HandleTypeDef *phuart;
	_dacai_usart_t *puart_t;
	u8 state;//是否在线
}_DACAI_t;


typedef struct _MultiTXT	{
	u8 id;
	u8 len;
	char buf[64];	
}_MultiTXT;

//extern const u8 CMD_END[4];
//extern const u8 CMD_SwitchUI[2];
//extern const u8 CMD_SetBacklight[2];
//extern const u8 CMD_DynamicCtrl[2];
//extern const u8 CMD_UpdateTXT[2] ;;
//extern const u8 CMD_ButtonCtrl[2];
//extern const u8 CMD_SetCursorPos[2];
//extern const u8 CMD_StandyMode[2];
//extern const u8 CMD_KeyBoard[2];

void DaCai_SendData(u8 *pbuf, u16 len);
void DaCaiProto_init(void);

extern _DACAI_t  dacai;

#endif
