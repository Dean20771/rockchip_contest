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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
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
#define motorD_IN1_Pin GPIO_PIN_2
#define motorD_IN1_GPIO_Port GPIOE
#define motorD_IN2_Pin GPIO_PIN_3
#define motorD_IN2_GPIO_Port GPIOE
#define motorC_IN1_Pin GPIO_PIN_4
#define motorC_IN1_GPIO_Port GPIOE
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
#define motorC_EncoderCH2_Pin GPIO_PIN_1
#define motorC_EncoderCH2_GPIO_Port GPIOA
#define motorC_EncoderCH1_Pin GPIO_PIN_5
#define motorC_EncoderCH1_GPIO_Port GPIOA
#define motorA_EncoderCH1_Pin GPIO_PIN_6
#define motorA_EncoderCH1_GPIO_Port GPIOA
#define motorA_EncoderCH2_Pin GPIO_PIN_7
#define motorA_EncoderCH2_GPIO_Port GPIOA
#define motorA_IN1_Pin GPIO_PIN_7
#define motorA_IN1_GPIO_Port GPIOE
#define motorA_IN2_Pin GPIO_PIN_8
#define motorA_IN2_GPIO_Port GPIOE
#define motorA_PWM_Pin GPIO_PIN_9
#define motorA_PWM_GPIO_Port GPIOE
#define motorB_IN1_Pin GPIO_PIN_10
#define motorB_IN1_GPIO_Port GPIOE
#define motorB_PWM_Pin GPIO_PIN_11
#define motorB_PWM_GPIO_Port GPIOE
#define motorB_IN2_Pin GPIO_PIN_12
#define motorB_IN2_GPIO_Port GPIOE
#define motorC_PWM_Pin GPIO_PIN_13
#define motorC_PWM_GPIO_Port GPIOE
#define motorD_PWM_Pin GPIO_PIN_14
#define motorD_PWM_GPIO_Port GPIOE
#define motorC_IN2_Pin GPIO_PIN_15
#define motorC_IN2_GPIO_Port GPIOE
#define motorB_EncoderCH2_Pin GPIO_PIN_12
#define motorB_EncoderCH2_GPIO_Port GPIOD
#define motorB_EncoderCH1_Pin GPIO_PIN_13
#define motorB_EncoderCH1_GPIO_Port GPIOD
#define motorD_EncoderCH2_Pin GPIO_PIN_6
#define motorD_EncoderCH2_GPIO_Port GPIOC
#define motorD_EncoderCH1_Pin GPIO_PIN_7
#define motorD_EncoderCH1_GPIO_Port GPIOC
#define Usart1_TX_Pin GPIO_PIN_9
#define Usart1_TX_GPIO_Port GPIOA
#define Usart1_RX_Pin GPIO_PIN_10
#define Usart1_RX_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */
// PID参数 - 方案一：增大Ki消除稳态误差
#define Velcity_Kp     500   // 保持不变
#define Velcity_Ki     0    // 增大积分系数，快速消除稳态误差
#define Velcity_Kd     50     // 保持不变
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
