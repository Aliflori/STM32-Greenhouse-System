/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32f1xx_hal.h"

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
#define Fan_Pin GPIO_PIN_3
#define Fan_GPIO_Port GPIOA
#define CS_Pin GPIO_PIN_12
#define CS_GPIO_Port GPIOB
#define Pump_Pin GPIO_PIN_13
#define Pump_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
#define Door_Pin GPIO_PIN_2
#define Door_Port GPIOA

#define portD4  GPIOB
#define portD5  GPIOB
#define portD6  GPIOB
#define portD7  GPIOB
#define portRS  GPIOB
#define portE   GPIOB

#define pinD4   GPIO_PIN_0
#define pinD5   GPIO_PIN_1
#define pinD6   GPIO_PIN_2
#define pinD7   GPIO_PIN_3
#define pinRS   GPIO_PIN_4
#define pinE    GPIO_PIN_5

#define Heater_CH   TIM_CHANNEL_3
#define Curtain_CH  TIM_CHANNEL_4

#define E2PROM_writeAddress 0x0000

#define FanOn   30.0
#define FanOff  25.0

#define PumpOff 75.0
#define PumpOn 35.0

#define tune_offset 0.2
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
