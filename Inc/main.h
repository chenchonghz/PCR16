/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MAX5401_CS1_Pin GPIO_PIN_13
#define MAX5401_CS1_GPIO_Port GPIOC
#define MAX5401_CS2_Pin GPIO_PIN_14
#define MAX5401_CS2_GPIO_Port GPIOC
#define EquipFanCtrl_Pin GPIO_PIN_0
#define EquipFanCtrl_GPIO_Port GPIOC
#define CoolFanCtrl_Pin GPIO_PIN_1
#define CoolFanCtrl_GPIO_Port GPIOC
#define LimitSwitchRgiht_Pin GPIO_PIN_2
#define LimitSwitchRgiht_GPIO_Port GPIOC
#define LimitSwitchRgiht_EXTI_IRQn EXTI2_IRQn
#define DoorSwitch_Pin GPIO_PIN_3
#define DoorSwitch_GPIO_Port GPIOC
#define TMC260_CS_Pin GPIO_PIN_4
#define TMC260_CS_GPIO_Port GPIOA
#define TMC260_SCK_Pin GPIO_PIN_5
#define TMC260_SCK_GPIO_Port GPIOA
#define TMC260_SDO_Pin GPIO_PIN_6
#define TMC260_SDO_GPIO_Port GPIOA
#define TMC260_SDI_Pin GPIO_PIN_7
#define TMC260_SDI_GPIO_Port GPIOA
#define TMC260_DIR_Pin GPIO_PIN_4
#define TMC260_DIR_GPIO_Port GPIOC
#define TMC260_EN_Pin GPIO_PIN_5
#define TMC260_EN_GPIO_Port GPIOC
#define TMC260_STEP_Pin GPIO_PIN_0
#define TMC260_STEP_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOB
#define LED_GREEN_Pin GPIO_PIN_2
#define LED_GREEN_GPIO_Port GPIOB
#define LED_BLUE_Pin GPIO_PIN_10
#define LED_BLUE_GPIO_Port GPIOB
#define HeatCoverCtrl_Pin GPIO_PIN_11
#define HeatCoverCtrl_GPIO_Port GPIOB
#define SPI2_CS_Pin GPIO_PIN_12
#define SPI2_CS_GPIO_Port GPIOB
#define CoolPWM_Pin GPIO_PIN_6
#define CoolPWM_GPIO_Port GPIOC
#define Fluo_Green_Pin GPIO_PIN_7
#define Fluo_Green_GPIO_Port GPIOC
#define Fluo_Blue_Pin GPIO_PIN_8
#define Fluo_Blue_GPIO_Port GPIOC
#define Fluo_OnOff_Pin GPIO_PIN_9
#define Fluo_OnOff_GPIO_Port GPIOC
#define BEEP_Pin GPIO_PIN_8
#define BEEP_GPIO_Port GPIOA
#define DEBUG_UART1_TX_Pin GPIO_PIN_9
#define DEBUG_UART1_TX_GPIO_Port GPIOA
#define DEBUG_UART1_RX_Pin GPIO_PIN_10
#define DEBUG_UART1_RX_GPIO_Port GPIOA
#define Bridge_DIR_Pin GPIO_PIN_15
#define Bridge_DIR_GPIO_Port GPIOA
#define AD_SCLK_Pin GPIO_PIN_10
#define AD_SCLK_GPIO_Port GPIOC
#define AD_MISO_Pin GPIO_PIN_11
#define AD_MISO_GPIO_Port GPIOC
#define AD_MOSI_Pin GPIO_PIN_12
#define AD_MOSI_GPIO_Port GPIOC
#define AD_SYNC_Pin GPIO_PIN_2
#define AD_SYNC_GPIO_Port GPIOD
#define AD_CS_Pin GPIO_PIN_3
#define AD_CS_GPIO_Port GPIOB
#define LimitSwitchLeft_Pin GPIO_PIN_4
#define LimitSwitchLeft_GPIO_Port GPIOB
#define LimitSwitchLeft_EXTI_IRQn EXTI4_IRQn
#define MAX5401_SCLK_Pin GPIO_PIN_8
#define MAX5401_SCLK_GPIO_Port GPIOB
#define MAX5401_DIN_Pin GPIO_PIN_9
#define MAX5401_DIN_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
