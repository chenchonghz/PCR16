#include "DaCai_API.h"

const u8 CMD_END[4]       = {0xFF , 0xFC , 0xFF , 0xFF}; //针尾
const u8 CMD_SwitchUI[2]      = {0xB1, 0x00}; //切换画面指令
const u8 CMD_SetBacklight[2]			 = {0x60 , 0x00};//设置背光指令
const u8 CMD_DynamicCtrl[2]      = {0xB1, 0x20}; //切换动画指令
const u8 CMD_UpdateTXT[2]      = {0xB1, 0x10}; //切换动画指令
const u8 CMD_ButtonCtrl[2]      = {0xB1, 0x10}; //切换动画指令
const u8 CMD_SetCursorPos[2]      = {0xB1, 0x02};
const u8 CMD_StandyMode[2]      = {0x60, 0x00}; //切换画面指令
const u8 CMD_KeyBoard[2]      = {0x86, 0x01}; //切换画面指令

u8 *ptxbuf;
_UI_t UI;

void DaCaiAPI_Init(void)
{
	ptxbuf = dacai.puart_t->tx_buf;
	UI.screen_id = Invalid_UIID;
	UI.ctrl_id = 0;
	UI.datlen=0;
	UI.pdata = (char *)tlsf_malloc(UserMem,DACAITXTBUF_SIZE);//用于向串口屏发送大数据缓存
}
//检测屏是否在线
void DaCai_CheckDevice(void)
{
	u8 len=0;
	
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_HANDSHAKE;
	DaCai_SendData(ptxbuf, len);
}
//获取屏画面当前id
void DaCai_GetScreenID(void)
{
	u8 len=0;
//	u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_SwitchUI[0];
	ptxbuf[len++] = CMD_SwitchUI[1]|0x1;
	DaCai_SendData(ptxbuf, len);
}
//切换屏画面
void DaCai_SwitchUI(_UI_t *pUI)
{
	u8 len=0;

	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	memcpy(ptxbuf+len, CMD_SwitchUI, 2);
	len += 2;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	DaCai_SendData(ptxbuf, len);
}
//子画面 
void DaCai_SwitchSubUI(_UI_t *pUI)
{
	u8 len=0;

	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xb1;
	ptxbuf[len++] = 0x0a;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->sub_screen_id;
	DaCai_SendData(ptxbuf, len);
}
//dynamic controls flag 0-启动动画；1-停止
void DaCai_DynamicCtrl(_UI_t *pUI,u8 flag)
{
	u8 len=0;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_DynamicCtrl[0];
	ptxbuf[len++] = CMD_DynamicCtrl[1]|flag;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	//ptxbuf[len++] = flag;
	DaCai_SendData(ptxbuf, len);
}
//按钮控制 0:弹起 1:按下
void DaCai_ButtonCtrl(_UI_t *pUI,u8 state)
{
	u8 len=0;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_ButtonCtrl[0];
	ptxbuf[len++] = CMD_ButtonCtrl[1];
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->button_id;
	ptxbuf[len++] = state;//0:弹起 1:按下
	DaCai_SendData(ptxbuf, len);
}
//
void DaCai_ChooseCtrl(_UI_t *pUI)
{
	u8 len=0;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_ButtonCtrl[0];
	ptxbuf[len++] = CMD_ButtonCtrl[1];
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	ptxbuf[len++] = pUI->index;//0:弹起 1:按下
	DaCai_SendData(ptxbuf, len);
}

void DaCai_SetCursorPos(_UI_t *pUI,u8 state)
{
	u8 len=0;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_SetCursorPos[0];
	ptxbuf[len++] = CMD_SetCursorPos[1];
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	ptxbuf[len++] = state;//0:弹起 1:按下
	DaCai_SendData(ptxbuf, len);
}

void DaCai_DisKeyboard(u16 x,u16 y)
{
	u8 len=0;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_KeyBoard[0];
	ptxbuf[len++] = CMD_KeyBoard[1];
	ptxbuf[len++] = (x>>8)&0xff;
	ptxbuf[len++] = x&0xff;
	ptxbuf[len++] = (y>>8)&0xff;
	ptxbuf[len++] = y&0xff;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = 0;//
	ptxbuf[len++] = 10;
	DaCai_SendData(ptxbuf, len);
}
//一次更新多个按钮状态
void DaCai_UpdateMultiButton(_UI_t *pUI,u8 *pButtonID, u8 *pVal, u8 num)
{
	u8 len=0,i;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xB1;
	ptxbuf[len++] = 0X12;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	for(i=0;i<num;i++)	{
		ptxbuf[len++] = 0;
		ptxbuf[len++] = pButtonID[i];
		ptxbuf[len++] = 0;
		ptxbuf[len++] = 1;
		ptxbuf[len++] = pVal[i];
	}
	DaCai_SendData(ptxbuf, len);
}
//u8 xxlen;
//一次更新多个文本框
void DaCai_UpdateMultiTXT(_UI_t *pUI,_MultiTXT **pMultiTXT, u8 num)
{
	u8 len=0,i;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xB1;
	ptxbuf[len++] = 0X12;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	for(i=0;i<num;i++)	{
		ptxbuf[len++] = 0;
		ptxbuf[len++] = pMultiTXT[i]->id;
		ptxbuf[len++] = 0;
		ptxbuf[len++] = pMultiTXT[i]->len;
		memcpy(ptxbuf+len, pMultiTXT[i]->buf, pMultiTXT[i]->len);
		len += pMultiTXT[i]->len;
	}
	//xxlen = len;
	DaCai_SendData(ptxbuf, len);
}
//更新单个文本框
void DaCai_UpdateTXT(_UI_t *pUI)
{
	u16 len=0;	
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_UpdateTXT[0];
	ptxbuf[len++] = CMD_UpdateTXT[1];
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	memcpy(ptxbuf+len,pUI->pdata,pUI->datlen);
	len += pUI->datlen;
	DaCai_SendData(ptxbuf, len);
}

void DaCai_StandbyMode(u8 state)	
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = CMD_StandyMode[0];
	if(state)	{//进入待机模式					
		ptxbuf[len++] = CMD_StandyMode[1];
	}else {//退出待机模式
		ptxbuf[len++] = CMD_StandyMode[1]|0x01;
	}
	DaCai_SendData(ptxbuf, len);
}

//void DaCai_TimeSet(RTC_TIME_ST *ptime)	
//{
//	u8 len=0;
//	//u8 *ptxbuf = dacai.puart->tx_buf;
//	mutex_lock(dacai.lock);
//	ptxbuf[len++] = DaCaiHEND;
//	ptxbuf[len++] = 0x81;
//	
//	ptxbuf[len++] = ptime->tm_sec;
//	ptxbuf[len++] = ptime->tm_min;
//	ptxbuf[len++] = ptime->tm_hour;
//	ptxbuf[len++] = ptime->tm_mday;
//	ptxbuf[len++] = ptime->tm_wday;
//	ptxbuf[len++] = ptime->tm_mon;
//	ptxbuf[len++] = ptime->tm_year;
//	DaCai_SendData(ptxbuf, len);
//}

void DaCai_TimeGet(void)	
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0x82;
	DaCai_SendData(ptxbuf, len);
}

void DaCai_TimeModeSet(void)	
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0x85;
	ptxbuf[len++] = 0x01;//0 : RTC关闭   1：RTC开启
	ptxbuf[len++] = 0x1;//0x01 : 格式20XX-MM-DD  HH:MM:SS 		0x00 : 格式 HH:MM:SS
	ptxbuf[len++] = 0x0;//font
	ptxbuf[len++] = 0x0;//x position
	ptxbuf[len++] = 0x0;
	ptxbuf[len++] = 0x0;//y position
	ptxbuf[len++] = 0x0;	
	DaCai_SendData(ptxbuf, len);
}
//显示or隐藏控件
void DaCai_CtrlONOFF(_UI_t *pUI, u8 flag)	
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xb1;
	ptxbuf[len++] = 0x03;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	ptxbuf[len++] = flag;
	DaCai_SendData(ptxbuf, len);
}

//屏保模式
void DaCai_ScreenSaveMode(u8 enable, u16 standy_t, u8 standy_brightness, u8 brightness)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0x77;
	ptxbuf[len++] = enable;//使能LPM模式
	ptxbuf[len++] = brightness;//屏幕亮度
	ptxbuf[len++] = standy_brightness;//待机亮度
	ptxbuf[len++] = (standy_t>>8)&0xff;//待机时间
	ptxbuf[len++] = standy_t&0xff;
	//ptxbuf[len++] = flag;
	DaCai_SendData(ptxbuf, len);
}

void DaCai_AdjustBrightness(u8 brightness)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0x60;
	ptxbuf[len++] = 0xff - brightness;
	DaCai_SendData(ptxbuf, len);
}
//待机指令 EE AA 02 AC FF FC FF FF
void DaCai_EnterStandby(void)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xAA;
	ptxbuf[len++] = 0x02;
	ptxbuf[len++] = 0xAC;
	DaCai_SendData(ptxbuf, len);
}
//唤醒指令 FF FF FF FF 00 00 00 00
void DaCai_ExitStandby(void)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = 0XFF;
	ptxbuf[len++] = 0XFF;
	ptxbuf[len++] = 0XFF;
	ptxbuf[len++] = 0XFF;
	ptxbuf[len++] = 0x00;
	ptxbuf[len++] = 0x00;
	ptxbuf[len++] = 0x00;
	ptxbuf[len++] = 0x00;
	DaCai_SendData(ptxbuf, len);
//	HAL_UART_Transmit_DMA(dacai.phuart, ptxbuf, len);
//	OSTimeDly(5);
}

//EE 06 01 FF FC FF FF 
void DaCai_ClearScreenAuto(u8 enable)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0x06;
	ptxbuf[len++] = enable;
	DaCai_SendData(ptxbuf, len);
}

void DaCai_ClearTXT(_UI_t *pUI)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xB1;
	ptxbuf[len++] = 0X10;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	DaCai_SendData(ptxbuf, len);
}
//设置进度条前景色
void DaCai_SetProgressForecolor(_UI_t *pUI,u8 R,u8 G)
{
	u8 len=0;
	//u8 *ptxbuf = dacai.puart->tx_buf;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xB1;
	ptxbuf[len++] = 0X19;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	ptxbuf[len++] = R;
	ptxbuf[len++] = G;
	DaCai_SendData(ptxbuf, len);
}
//设置文本闪烁 单位10ms
void DaCai_SetTXTtwinkle(_UI_t *pUI,u16 period)
{
	u8 len=0;
	mutex_lock(dacai.lock);
	ptxbuf[len++] = DaCaiHEND;
	ptxbuf[len++] = 0xB1;
	ptxbuf[len++] = 0X15;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->screen_id;
	ptxbuf[len++] = 0;
	ptxbuf[len++] = pUI->ctrl_id;
	ptxbuf[len++] = period>>8;
	ptxbuf[len++] = period&0x0ff;
	DaCai_SendData(ptxbuf, len);
}
