/* Includes -----------------------------------------------------------------*/
#include "task_manage.h"
#include "app_udisk.h"
#include "app_spiflash.h"
#include "app_display.h"
//#include "sys_monitor.h"
//#include "globalvariable.h"
#include "app_motor.h"
//#include "app_usart.h"
#include "app_ad.h"
//////////////////////////////////////////////////////////
__align(4) OS_STK  TASK_START_STK[STK_SIZE_START]; //任务堆栈声明

//////////////////////////////////////////////////////////
static void TaskCreateOther(void);             //用这个函数来创建其他任务
static void TaskStart(void * ppdata);

void CreatMainTask(void)
{				
	OSTimeSet(0);				
	OSInit(); 
	OSTaskCreate(TaskStart,  									//start_task??
							(void*)0,    									//??
							(OS_STK*)&TASK_START_STK[STK_SIZE_START-1], 	//??????
							TASK_PRIO_START);  								//?????
	OSStart();  
}
//////////////////////////////////////////////////////////
//起始任务 又这个任务再创建其他任务
static void TaskStart(void * ppdata)
{
	//OS_CPU_SR cpu_sr;
	ppdata = ppdata;

	//OS_ENTER_CRITICAL();
	__set_PRIMASK(0);
    //使能ucos 的统计任务
    #if (OS_TASK_STAT_EN > 0)
    OSStatInit();                       //统计任务初始化函数   
    #endif  
      
    SysDataInit();                   //用户应用程序全局变量初始化
	bsp_init();                              //板载硬件初始化
//	SysAlarmDataInit();
	msg_init();
    TaskCreateOther();			        //建立其他的任务
    OSTaskSuspend(OS_PRIO_SELF);	    //suspend but not delete 挂起任务
		//OS_EXIT_CRITICAL();
}

/////////////////////////////////////////////////////////////////////////////
//创建其他所有任务 在这里我建立了个LED闪烁的任务当例子
static void TaskCreateOther(void)
{
	AppADInit();	
//	AppSensorBoardInit();	
//	AppKeyScanInit();
	AppMotorInit();		
//	AppDisplayInit();
//	AppPerfusemonitorInit();
//	AppSysmonitorInit();
	AppSpiFlashInit();
	AppUSBInit();	
}


      
