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
//#define	_SYS_SHUTDOWN_DETECT()			HAL_GPIO_ReadPin(INTR_LT_GPIO_Port, INTR_LT_Pin)
//#define	ReadValveDetectPin()			HAL_GPIO_ReadPin(ValveDet_GPIO_Port,ValveDet_Pin)
//#define	SB_OVC_DETECT()				HAL_GPIO_ReadPin(SB_OVC_GPIO_Port, SB_OVC_Pin)
//#define	USB_OVC_DETECT()			HAL_GPIO_ReadPin(USB_OVC_GPIO_Port, USB_OVC_Pin)

//#define	SysPowerEnable()		HAL_GPIO_WritePin(SysPwrCtrl_GPIO_Port, SysPwrCtrl_Pin,GPIO_PIN_SET)
//#define	SysPowerDisable()		HAL_GPIO_WritePin(SysPwrCtrl_GPIO_Port, SysPwrCtrl_Pin,GPIO_PIN_RESET)
//#define	SlaveboardPowerEnable()			HAL_GPIO_WritePin(SBPwrCtrl_GPIO_Port, SBPwrCtrl_Pin,GPIO_PIN_SET)
//#define	SlaveboardPowerDisable()		HAL_GPIO_WritePin(SBPwrCtrl_GPIO_Port, SBPwrCtrl_Pin,GPIO_PIN_RESET)
//#define	DisplayPowerEnable()		HAL_GPIO_WritePin(DispPwrCtrl_GPIO_Port, DispPwrCtrl_Pin,GPIO_PIN_SET)
//#define	DisplayPowerDisable()		HAL_GPIO_WritePin(DispPwrCtrl_GPIO_Port, DispPwrCtrl_Pin,GPIO_PIN_RESET)

#define	Led_Fluo_On()		HAL_GPIO_WritePin(Fluo_OnOff_GPIO_Port, Fluo_OnOff_Pin, GPIO_PIN_RESET)
#define	Led_Fluo_Off()		HAL_GPIO_WritePin(Fluo_OnOff_GPIO_Port, Fluo_OnOff_Pin, GPIO_PIN_SET)
#define	Led_FluoBlue_On()		HAL_GPIO_WritePin(Fluo_Blue_GPIO_Port, Fluo_Blue_Pin,GPIO_PIN_RESET)
#define	Led_FluoBlue_Off()		HAL_GPIO_WritePin(Fluo_Blue_GPIO_Port, Fluo_Blue_Pin,GPIO_PIN_SET)
#define	Led_FluoGreen_On()		HAL_GPIO_WritePin(Fluo_Green_GPIO_Port, Fluo_Green_Pin,GPIO_PIN_RESET)
#define	Led_FluoGreen_Off()		HAL_GPIO_WritePin(Fluo_Green_GPIO_Port, Fluo_Green_Pin,GPIO_PIN_SET)

#define	TEC_DIR_COLD()				HAL_GPIO_WritePin(TEC_DIR_GPIO_Port, TEC_DIR_Pin,GPIO_PIN_SET)
#define	TEC_DIR_HOT()				HAL_GPIO_WritePin(TEC_DIR_GPIO_Port, TEC_DIR_Pin,GPIO_PIN_RESET)
#define	TEC_ON()				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_SET)
#define	TEC_OFF()				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6, GPIO_PIN_RESET)

#define	LED_RED_ON()				HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin,GPIO_PIN_SET)
#define	LED_RED_OFF()				HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin,GPIO_PIN_RESET)
#define	LED_GREEN_ON()				HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin,GPIO_PIN_SET)
#define	LED_GREEN_OFF()				HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin,GPIO_PIN_RESET)
#define	LED_BLUE_ON()				HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin,GPIO_PIN_SET)
#define	LED_BLUE_OFF()				HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin,GPIO_PIN_RESET)

#define	BEEP_ON()		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET)
#define	BEEP_OFF()		HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)

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
