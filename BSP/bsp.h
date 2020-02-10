/*
H********************************************************************************************************
*                                      Weight System Firmware
*
*                    (c) Copyright 2010-2014 ACEA Biosciences, Inc.
*                    All rights reserved.  Protected by international copyright laws.
*
* File      :  bsp.h
* By        :  AlexShi
* Email     :  shiweining123@gmail.com
* Version   :  V1.00.00
* Compiler  :  KEIL
*********************************************************************************************************
* Note(s)   :
*
*
*
*********************************************************************************************************
*/
#ifndef _BSP_H
#define _BSP_H

#ifdef  BSP_GLOBALS
#define BSP_EXT
#else
#define BSP_EXT extern
#endif

/*
*********************************************************************************************************
*                                          INCLUDE HEADER FILES
*********************************************************************************************************
*/

#include "stm32l4xx_hal.h"  
#include "stm32l4xx_hal_tim.h"  
#include "sys_bits.h"
#include "sys_types.h"
#include "sys_defs.h"


/*
*********************************************************************************************************
*                                                  MACROS
*********************************************************************************************************
*/
#define SYSTEM_SUPPORT_OS       1
/*
*********************************************************************************************************
*                                            MACROS FUNCTIONS
*********************************************************************************************************
*/

#if 1
//位操作，实现51类似的GPIO控制功能
//具体实现思想，参考《M3权威指南》第五章（87页-92页）M4同M3类似，只是寄存器地址变了
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

#define SET_L(io)               io->port->BRR  = io->pin
#define SET_H(io)               io->port->BSRR = io->pin
#define IS_HIGH(io)             (io->port->IDR&io->pin)
#else
#define SET_L(io)               GPIO_ResetBits(io->port, io->pin)
#define SET_H(io)               GPIO_SetBits(io->port, io->pin)
#define IS_HIGH(io)             GPIO_ReadInputDataBit(io->port, io->pin)
#endif

/*
*********************************************************************************************************
*                                             TYPE DEFINITION
*********************************************************************************************************
*/
#define	_SYS_SHUTDOWN_DETECT()			HAL_GPIO_ReadPin(INTR_LT_GPIO_Port, INTR_LT_Pin)
#define	ReadValveDetectPin()			HAL_GPIO_ReadPin(ValveDet_GPIO_Port,ValveDet_Pin)
#define	SB_OVC_DETECT()				HAL_GPIO_ReadPin(SB_OVC_GPIO_Port, SB_OVC_Pin)
#define	USB_OVC_DETECT()			HAL_GPIO_ReadPin(USB_OVC_GPIO_Port, USB_OVC_Pin)

#define	SysPowerEnable()		HAL_GPIO_WritePin(SysPwrCtrl_GPIO_Port, SysPwrCtrl_Pin,GPIO_PIN_SET)
#define	SysPowerDisable()		HAL_GPIO_WritePin(SysPwrCtrl_GPIO_Port, SysPwrCtrl_Pin,GPIO_PIN_RESET)
#define	SlaveboardPowerEnable()			HAL_GPIO_WritePin(SBPwrCtrl_GPIO_Port, SBPwrCtrl_Pin,GPIO_PIN_SET)
#define	SlaveboardPowerDisable()		HAL_GPIO_WritePin(SBPwrCtrl_GPIO_Port, SBPwrCtrl_Pin,GPIO_PIN_RESET)
#define	DisplayPowerEnable()		HAL_GPIO_WritePin(DispPwrCtrl_GPIO_Port, DispPwrCtrl_Pin,GPIO_PIN_SET)
#define	DisplayPowerDisable()		HAL_GPIO_WritePin(DispPwrCtrl_GPIO_Port, DispPwrCtrl_Pin,GPIO_PIN_RESET)
#define	UsbPowerDisable()		HAL_GPIO_WritePin(USB_PWR_GPIO_Port, USB_PWR_Pin,GPIO_PIN_RESET)
#define	UsbPowerEnable()		HAL_GPIO_WritePin(USB_PWR_GPIO_Port, USB_PWR_Pin,GPIO_PIN_SET)
#define	AlarmActive()				HAL_GPIO_WritePin(AlarmActive_GPIO_Port, AlarmActive_Pin,GPIO_PIN_SET)
#define	AlarmInactive()				HAL_GPIO_WritePin(AlarmActive_GPIO_Port, AlarmActive_Pin,GPIO_PIN_RESET)

#define	PaiqiValvePort		VALVE1_GPIO_Port
#define	PaiqiValvePin		VALVE1_Pin
#define	VeinValvePort		VALVE2_GPIO_Port
#define	VeinValvePin		VALVE2_Pin
#define	ArteryValvePort		VALVE3_GPIO_Port
#define	ArteryValvePin		VALVE3_Pin

#define	PaiqiValveRelease()			HAL_GPIO_WritePin(PaiqiValvePort, PaiqiValvePin, GPIO_PIN_RESET)
#define	VeinValveRelease()			HAL_GPIO_WritePin(VeinValvePort, VeinValvePin, GPIO_PIN_RESET)
#define	ArteryValveRelease()		HAL_GPIO_WritePin(ArteryValvePort, ArteryValvePin, GPIO_PIN_RESET)
#define	AllValveRelease()	\
do{						\
	ArteryValveRelease();	\
	VeinValveRelease();	\
	PaiqiValveRelease();	\
}while(0)

#define	PaiqiValveActive()			HAL_GPIO_WritePin(PaiqiValvePort, PaiqiValvePin, GPIO_PIN_SET)
#define	VeinValveActive()			HAL_GPIO_WritePin(VeinValvePort, VeinValvePin, GPIO_PIN_SET)
#define	ArteryValveActive()			HAL_GPIO_WritePin(ArteryValvePort, ArteryValvePin, GPIO_PIN_SET)
#define	AllValveActive()	\
do{						\
	ArteryValveActive();	\
	VeinValveActive();	\
	PaiqiValveActive();	\
}while(0)

//#define	BEEP_ON()		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin,GPIO_PIN_SET)
//#define	BEEP_OFF()		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin,GPIO_PIN_RESET)

struct _io_map {
    GPIO_TypeDef   *port;
    uint16_t        pin;
};

void ioconfig(const struct _io_map *pio, cpu_bool_t sw);
void delay_us(u16 us);
void	bsp_init(void);
void FWUpdate_reboot(void);
void SoftReset(void);
void SysShutDown(void);
void SysStandbyEnter(void);
void SysStandbyQuit(void);
void UartBaudrateSet(UART_HandleTypeDef *phuart, u32 baudrate);
#endif
