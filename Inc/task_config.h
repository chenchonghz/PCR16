/*
********************************************************************************
* task_config.h
*
*   Copyright (C) 2010-2017 ACEA Biosciences, Inc. All rights reserved.
*   Author: AlexShi <shiweining123@163.com>
*
********************************************************************************
*/

#ifndef __TASK_CONFIG_H
#define __TASK_CONFIG_H

/*
********************************************************************************
* Included Files
********************************************************************************
*/



/*
********************************************************************************
* Pre-processor Definitions
********************************************************************************
*/

#define TASK_HIGHEST_PRIO               3
#define TASK_LOWEST_PRIO               (OS_LOWEST_PRIO-2)
#define OS_TICK_RATE_HZ                 OS_TICKS_PER_SEC
#define OS_TICK_RATE_MS                (1000/OS_TICKS_PER_SEC)

/* Task Stack Size Configure */
#define STK_SIZE_MIN                    256u
#define STK_SIZE_START                 (STK_SIZE_MIN)
#define STK_SIZE_USART_TX              (STK_SIZE_MIN * 2)
#define STK_SIZE_USART_RX              (STK_SIZE_MIN * 2)
#define STK_SIZE_SYSMONITOR            (STK_SIZE_MIN * 2)
#define STK_SIZE_UDISK								 (STK_SIZE_MIN * 2)
#define STK_SIZE_SPIFLASH							 (STK_SIZE_MIN * 2)
#define STK_SIZE_DISPLAY							 (STK_SIZE_MIN * 2)
#define STK_SIZE_MOTOR								 (STK_SIZE_MIN * 2)
#define STK_SIZE_AD								 (STK_SIZE_MIN * 2)
#define STK_SIZE_TEMP								 (STK_SIZE_MIN * 2)
#define STK_SIZE_FLUO								 (STK_SIZE_MIN * 2)


/* Task Message Size Configure */
#define TASK_MSG_SIZE_START             1
#define TASK_MSG_SIZE_USART_TX          4
#define TASK_MSG_SIZE_USART_RX          4
#define TASK_MSG_SIZE_SHELL             4
#define TASK_MSG_SIZE_LASER             4
#define TASK_MSG_SIZE_TEMP              4
#define TASK_MSG_SIZE_PID               4
#define TASK_MSG_SIZE_SYSMONITOR        4

#define MSG_SIZE_TEC_RX                 2

/*
********************************************************************************
* Shared Types
********************************************************************************
*/



/*
********************************************************************************
* Public Types
********************************************************************************
*/

enum _task_prio {
    MUTEX_PRIO_TLSF = TASK_HIGHEST_PRIO,
    TASK_PRIO_START,  
	TASK_PRIO_AD, 	
	TASK_PRIO_TEMP,
	TASK_PRIO_FLUO,
    TASK_PRIO_USART_TX,
    TASK_PRIO_USART_RX,
	TASK_PRIO_MOTOR,	
	TASK_PRIO_DISPLAY,	
	TASK_PRIO_SYSMONITOR,	
	TASK_PRIO_SPIFLASH,
	TASK_PRIO_UDISK,
};

enum _task_id {
    TASK_ID_START = TASK_PRIO_START,
    TASK_ID_USART_TX,
    TASK_ID_USART_RX,
	TASK_ID_UDISK,
    TASK_ID_SYSMONITOR,
    TASK_ID_LED,
};


/*
********************************************************************************
* Inline Functions
********************************************************************************
*/



/*
********************************************************************************
* Shared Data
********************************************************************************
*/



/*
********************************************************************************
* Public Data
********************************************************************************
*/



/*
********************************************************************************
* Public Function Prototypes
********************************************************************************
*/

#ifdef __cplusplus
#define EXTERN extern "C"
extern "C" {
#else
#define EXTERN extern
#endif

#undef EXTERN
#ifdef __cplusplus
}
#endif

#endif      /* __TASK_CONFIG_H */

/*
********************************************************************************
* No More!
********************************************************************************
*/
