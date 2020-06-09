/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32l4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32l4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "includes.h"
#include "motor.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern HCD_HandleTypeDef hhcd_USB_OTG_FS;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
extern TIM_HandleTypeDef htim7;
extern DMA_HandleTypeDef hdma_uart4_tx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern UART_HandleTypeDef huart4;
extern UART_HandleTypeDef huart2;
/* USER CODE BEGIN EV */
#include "app_display.h"
#include "app_ad.h"
#include "app_usart.h"

extern void UART2_DMA_Callback(DMA_HandleTypeDef *hdma);
extern void DaCaiUART_DMA_Callback(DMA_HandleTypeDef *hdma);
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */ 
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Prefetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */
	OSIntEnter();		
  OSTimeTick();      
  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */
	OSIntExit();       
  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32L4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32l4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles EXTI line2 interrupt.
  */
void EXTI2_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI2_IRQn 0 */
OSIntEnter();
  /* USER CODE END EXTI2_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
  /* USER CODE BEGIN EXTI2_IRQn 1 */
OSIntExit();
  /* USER CODE END EXTI2_IRQn 1 */
}

/**
  * @brief This function handles EXTI line4 interrupt.
  */
void EXTI4_IRQHandler(void)
{
  /* USER CODE BEGIN EXTI4_IRQn 0 */
OSIntEnter();
  /* USER CODE END EXTI4_IRQn 0 */
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
  /* USER CODE BEGIN EXTI4_IRQn 1 */
OSIntExit();
  /* USER CODE END EXTI4_IRQn 1 */
}

/**
  * @brief This function handles DMA1 channel7 global interrupt.
  */
void DMA1_Channel7_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Channel7_IRQn 0 */
OSIntEnter();//上位机通讯串口uart2 dma发送中断
	if(__HAL_DMA_GET_FLAG(&hdma_usart2_tx, DMA_FLAG_TC7)==DMA_FLAG_TC7)	{
		UART2_DMA_Callback(&hdma_usart2_tx);
	}
  /* USER CODE END DMA1_Channel7_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Channel7_IRQn 1 */
OSIntExit();
  /* USER CODE END DMA1_Channel7_IRQn 1 */
}

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
OSIntEnter();
	if(__HAL_TIM_GET_FLAG(tMotor[MOTOR_ID1].tmr, TIM_IT_UPDATE)==SET)	{
		__HAL_TIM_CLEAR_FLAG(tMotor[MOTOR_ID1].tmr, TIM_IT_UPDATE);  //清除TIMx的中断待处理位:TIM 中断源 
		if(tMotor[MOTOR_ID1].status.is_run == MotorState_Run)      {
			if(tMotor[MOTOR_ID1].Dir == MOTOR_TO_MIN)
				tMotor[MOTOR_ID1].CurSteps--;
			else
				tMotor[MOTOR_ID1].CurSteps++;
			
			tMotor[MOTOR_ID1].StepCnt++;          
			if ((void *)tMotor[MOTOR_ID1].StepsCallback != (void *)0) {
				(*tMotor[MOTOR_ID1].StepsCallback)(&tMotor[MOTOR_ID1]);
			}
		}
	}
  /* USER CODE END TIM3_IRQn 0 */
  /* USER CODE BEGIN TIM3_IRQn 1 */
OSIntExit();
  /* USER CODE END TIM3_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */
	u8 rxdat;
OSIntEnter();//上位机通讯
	if(__HAL_UART_GET_IT(usart.port, UART_IT_RXNE)==SET)	{
		rxdat = (uint8_t)(usart.port->Instance->RDR);
		if(usart.rx_indicate!=NULL)	{
				usart.rx_indicate(&usart,rxdat);
		}
	}
  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */
	if(__HAL_UART_GET_FLAG(usart.port, UART_FLAG_ORE)==SET)	{
		__HAL_UART_CLEAR_OREFLAG(usart.port);
	}
	if(__HAL_UART_GET_FLAG(usart.port, UART_CLEAR_TCF)==SET)	{
		usart.port->gState = HAL_UART_STATE_READY;
		__HAL_UART_CLEAR_FLAG(usart.port,UART_CLEAR_TCF);
	}
OSIntExit();
  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles UART4 global interrupt.
  */
void UART4_IRQHandler(void)
{
  /* USER CODE BEGIN UART4_IRQn 0 */
	u8 rxdat;//串口屏通讯
OSIntEnter();
	if(__HAL_UART_GET_IT(appdis.pDaCai->phuart, UART_IT_RXNE)==SET)	{
		__HAL_UART_CLEAR_IT(appdis.pDaCai->phuart, UART_IT_RXNE);
		rxdat = (uint8_t)(appdis.pDaCai->phuart->Instance->RDR);
		if(appdis.pDaCai->puart_t->rx_indicate!=NULL)	{
				appdis.pDaCai->puart_t->rx_indicate(appdis.pDaCai->puart_t,rxdat);
		}
	}
	if(__HAL_UART_GET_FLAG(appdis.pDaCai->phuart, UART_FLAG_ORE)==SET)	{
		rxdat = (uint8_t)(appdis.pDaCai->phuart->Instance->RDR);
		__HAL_UART_CLEAR_OREFLAG(appdis.pDaCai->phuart);
	}
	if(__HAL_UART_GET_FLAG(appdis.pDaCai->phuart, UART_CLEAR_TCF)==SET)	{
		appdis.pDaCai->phuart->gState = HAL_UART_STATE_READY;
		__HAL_UART_CLEAR_FLAG(appdis.pDaCai->phuart,UART_CLEAR_TCF);
	}
  /* USER CODE END UART4_IRQn 0 */
  HAL_UART_IRQHandler(&huart4);
  /* USER CODE BEGIN UART4_IRQn 1 */
OSIntExit();
  /* USER CODE END UART4_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC channel1 and channel2 underrun error interrupts.
  */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */
	static u32 time_cnt;
OSIntEnter();
	if(__HAL_TIM_GET_FLAG(&htim6, TIM_IT_UPDATE)==SET)	{
		__HAL_TIM_CLEAR_FLAG(&htim6, TIM_IT_UPDATE);
		ad7124_cs_low();		
		time_cnt++;
		if((time_cnt%10)==0)	{//加减速时间间隔1ms
			MotorAccDec(&tMotor[MOTOR_ID1]);//电机加减速			
		}
		if(app_ad.rflag == DEF_False&&AD7124_DATA_READY()==0)	{//ad7124转换结果查询
			OSSemPost(app_ad.sem);//启动ad任务 读取ad转换结果及数据处理
		}
	}
  /* USER CODE END TIM6_DAC_IRQn 0 */
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */
OSIntExit();
  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles TIM7 global interrupt.
  */
void TIM7_IRQHandler(void)
{
  /* USER CODE BEGIN TIM7_IRQn 0 */
OSIntEnter();
	if(__HAL_TIM_GET_FLAG(&htim7, TIM_IT_UPDATE)==SET)	{
		__HAL_TIM_CLEAR_FLAG(&htim7, TIM_IT_UPDATE); 
		SoftTimerCallback();
	}
  /* USER CODE END TIM7_IRQn 0 */
  /* USER CODE BEGIN TIM7_IRQn 1 */
OSIntExit();
  /* USER CODE END TIM7_IRQn 1 */
}

/**
  * @brief This function handles DMA2 channel3 global interrupt.
  */
void DMA2_Channel3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA2_Channel3_IRQn 0 */
OSIntEnter();//串口屏uart4 dma发送中断
	if(__HAL_DMA_GET_FLAG(&hdma_uart4_tx, DMA_FLAG_TC3)==DMA_FLAG_TC3)	{
		DaCaiUART_DMA_Callback(&hdma_uart4_tx);
//		__HAL_DMA_CLEAR_FLAG(&hdma_uart4_tx,DMA_FLAG_TC3);
	}
  /* USER CODE END DMA2_Channel3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_uart4_tx);
  /* USER CODE BEGIN DMA2_Channel3_IRQn 1 */
OSIntExit();
  /* USER CODE END DMA2_Channel3_IRQn 1 */
}

/**
  * @brief This function handles USB OTG FS global interrupt.
  */
void OTG_FS_IRQHandler(void)
{
  /* USER CODE BEGIN OTG_FS_IRQn 0 */
OSIntEnter();
  /* USER CODE END OTG_FS_IRQn 0 */
  HAL_HCD_IRQHandler(&hhcd_USB_OTG_FS);
  /* USER CODE BEGIN OTG_FS_IRQn 1 */
OSIntExit();
  /* USER CODE END OTG_FS_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
