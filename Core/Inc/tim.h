/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.h
  * @brief   This file contains all the function prototypes for
  *          the tim.c file
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
#ifndef __TIM_H__
#define __TIM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern TIM_HandleTypeDef htim1;

extern TIM_HandleTypeDef htim2;

extern TIM_HandleTypeDef htim3;

extern TIM_HandleTypeDef htim4;

extern TIM_HandleTypeDef htim7;

extern TIM_HandleTypeDef htim8;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_TIM1_Init(void);
void MX_TIM2_Init(void);
void MX_TIM3_Init(void);
void MX_TIM4_Init(void);
void MX_TIM7_Init(void);
void MX_TIM8_Init(void);

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* USER CODE BEGIN Prototypes */
void motor_setspeed(int16_t speedA,int16_t speedB,int16_t speedC,int16_t speedD);
int get_encodeA(void);
int get_encodeB(void);
int get_encodeC(void);
int get_encodeD(void);

float get_speedA_1s(int8_t ms);
float get_speedB_1s(int8_t ms);
float get_speedC_1s(int8_t ms);
float get_speedD_1s(int8_t ms);

int16_t Velocity_motorA_FeedbackControl(float TargetVelocity, float CurrentVelocity);
int16_t Velocity_motorB_FeedbackControl(float TargetVelocity, float CurrentVelocity);
int16_t Velocity_motorC_FeedbackControl(float TargetVelocity, float CurrentVelocity);
int16_t Velocity_motorD_FeedbackControl(float TargetVelocity, float CurrentVelocity);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __TIM_H__ */

