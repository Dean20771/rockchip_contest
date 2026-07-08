/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
/* Includes ------------------------------------------------------------------*/
#include "tim.h"

/* USER CODE BEGIN 0 */
volatile int encodeA_count = 0;
volatile int encodeB_count = 0;
volatile int encodeC_count = 0;
volatile int encodeD_count = 0;

int64_t EncodeA_now=0,EncodeA_old=0;
int64_t EncodeB_now=0,EncodeB_old=0;
int64_t EncodeC_now=0,EncodeC_old=0;
int64_t EncodeD_now=0,EncodeD_old=0;

float varA;
float varB;
float varC;
float varD;

float speedA;
float speedB;
float speedC;
float speedD;

/* USER CODE END 0 */

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;
TIM_HandleTypeDef htim7;
TIM_HandleTypeDef htim8;

/* TIM1 init function */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 168-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 300;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  __HAL_TIM_DISABLE_OCxPRELOAD(&htim1, TIM_CHANNEL_1);
  sConfigOC.Pulse = 0;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_2);  
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}
/* TIM2 init function */
void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 65535;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */
   HAL_TIM_Encoder_Start_IT(&htim2,TIM_CHANNEL_ALL);
  __HAL_TIM_CLEAR_FLAG(&htim2,TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&htim2,TIM_IT_UPDATE);
  /* USER CODE END TIM2_Init 2 */

}
/* TIM3 init function */
void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 5;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 7;
  if (HAL_TIM_Encoder_Init(&htim3, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */
   HAL_TIM_Encoder_Start_IT(&htim3,TIM_CHANNEL_ALL);
  __HAL_TIM_CLEAR_FLAG(&htim3,TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&htim3,TIM_IT_UPDATE);
  /* USER CODE END TIM3_Init 2 */

}
/* TIM4 init function */
void MX_TIM4_Init(void)
{

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 0;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 65535;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 7;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 7;
  if (HAL_TIM_Encoder_Init(&htim4, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */
   HAL_TIM_Encoder_Start_IT(&htim4,TIM_CHANNEL_ALL);
  __HAL_TIM_CLEAR_FLAG(&htim4,TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&htim4,TIM_IT_UPDATE);
  /* USER CODE END TIM4_Init 2 */

}
/* TIM7 init function */
void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 84-1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 1000-1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */
   HAL_TIM_Base_Start_IT(&htim7);
  /* USER CODE END TIM7_Init 2 */

}
/* TIM8 init function */
void MX_TIM8_Init(void)
{

  /* USER CODE BEGIN TIM8_Init 0 */

  /* USER CODE END TIM8_Init 0 */

  TIM_Encoder_InitTypeDef sConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM8_Init 1 */

  /* USER CODE END TIM8_Init 1 */
  htim8.Instance = TIM8;
  htim8.Init.Prescaler = 0;
  htim8.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim8.Init.Period = 65535;
  htim8.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim8.Init.RepetitionCounter = 0;
  htim8.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
  if (HAL_TIM_Encoder_Init(&htim8, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim8, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM8_Init 2 */
   HAL_TIM_Encoder_Start_IT(&htim8,TIM_CHANNEL_ALL);
  __HAL_TIM_CLEAR_FLAG(&htim8,TIM_IT_UPDATE);
  __HAL_TIM_ENABLE_IT(&htim8,TIM_IT_UPDATE);
  /* USER CODE END TIM8_Init 2 */

}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspInit 0 */

  /* USER CODE END TIM1_MspInit 0 */
    /* TIM1 clock enable */
    __HAL_RCC_TIM1_CLK_ENABLE();
  /* USER CODE BEGIN TIM1_MspInit 1 */

  /* USER CODE END TIM1_MspInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspInit 0 */

  /* USER CODE END TIM7_MspInit 0 */
    /* TIM7 clock enable */
    __HAL_RCC_TIM7_CLK_ENABLE();

    /* TIM7 interrupt Init */
    HAL_NVIC_SetPriority(TIM7_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM7_IRQn);
  /* USER CODE BEGIN TIM7_MspInit 1 */

  /* USER CODE END TIM7_MspInit 1 */
  }
}

void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef* tim_encoderHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(tim_encoderHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspInit 0 */

  /* USER CODE END TIM2_MspInit 0 */
    /* TIM2 clock enable */
    __HAL_RCC_TIM2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM2 GPIO Configuration
    PA1     ------> TIM2_CH2
    PA5     ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = motorC_EncoderCH2_Pin|motorC_EncoderCH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM2 interrupt Init */
    HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspInit 1 */

  /* USER CODE END TIM2_MspInit 1 */
  }
  else if(tim_encoderHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspInit 0 */

  /* USER CODE END TIM3_MspInit 0 */
    /* TIM3 clock enable */
    __HAL_RCC_TIM3_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**TIM3 GPIO Configuration
    PA6     ------> TIM3_CH1
    PA7     ------> TIM3_CH2
    */
    GPIO_InitStruct.Pin = motorA_EncoderCH1_Pin|motorA_EncoderCH2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* TIM3 interrupt Init */
    HAL_NVIC_SetPriority(TIM3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspInit 1 */

  /* USER CODE END TIM3_MspInit 1 */
  }
  else if(tim_encoderHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspInit 0 */

  /* USER CODE END TIM4_MspInit 0 */
    /* TIM4 clock enable */
    __HAL_RCC_TIM4_CLK_ENABLE();

    __HAL_RCC_GPIOD_CLK_ENABLE();
    /**TIM4 GPIO Configuration
    PD12     ------> TIM4_CH1
    PD13     ------> TIM4_CH2
    */
    GPIO_InitStruct.Pin = motorB_EncoderCH2_Pin|motorB_EncoderCH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* TIM4 interrupt Init */
    HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspInit 1 */

  /* USER CODE END TIM4_MspInit 1 */
  }
  else if(tim_encoderHandle->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspInit 0 */

  /* USER CODE END TIM8_MspInit 0 */
    /* TIM8 clock enable */
    __HAL_RCC_TIM8_CLK_ENABLE();

    __HAL_RCC_GPIOC_CLK_ENABLE();
    /**TIM8 GPIO Configuration
    PC6     ------> TIM8_CH1
    PC7     ------> TIM8_CH2
    */
    GPIO_InitStruct.Pin = motorD_EncoderCH2_Pin|motorD_EncoderCH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    /* TIM8 interrupt Init */
    HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
  /* USER CODE BEGIN TIM8_MspInit 1 */

  /* USER CODE END TIM8_MspInit 1 */
  }
}
void HAL_TIM_MspPostInit(TIM_HandleTypeDef* timHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(timHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspPostInit 0 */

  /* USER CODE END TIM1_MspPostInit 0 */

    __HAL_RCC_GPIOE_CLK_ENABLE();
    /**TIM1 GPIO Configuration
    PE9     ------> TIM1_CH1
    PE11     ------> TIM1_CH2
    PE13     ------> TIM1_CH3
    PE14     ------> TIM1_CH4
    */
    GPIO_InitStruct.Pin = motorA_PWM_Pin|motorB_PWM_Pin|motorC_PWM_Pin|motorD_PWM_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /* USER CODE BEGIN TIM1_MspPostInit 1 */

  /* USER CODE END TIM1_MspPostInit 1 */
  }

}

void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
{

  if(tim_baseHandle->Instance==TIM1)
  {
  /* USER CODE BEGIN TIM1_MspDeInit 0 */

  /* USER CODE END TIM1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM1_CLK_DISABLE();
  /* USER CODE BEGIN TIM1_MspDeInit 1 */

  /* USER CODE END TIM1_MspDeInit 1 */
  }
  else if(tim_baseHandle->Instance==TIM7)
  {
  /* USER CODE BEGIN TIM7_MspDeInit 0 */

  /* USER CODE END TIM7_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM7_CLK_DISABLE();

    /* TIM7 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM7_IRQn);
  /* USER CODE BEGIN TIM7_MspDeInit 1 */

  /* USER CODE END TIM7_MspDeInit 1 */
  }
}

void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef* tim_encoderHandle)
{

  if(tim_encoderHandle->Instance==TIM2)
  {
  /* USER CODE BEGIN TIM2_MspDeInit 0 */

  /* USER CODE END TIM2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM2_CLK_DISABLE();

    /**TIM2 GPIO Configuration
    PA1     ------> TIM2_CH2
    PA5     ------> TIM2_CH1
    */
    HAL_GPIO_DeInit(GPIOA, motorC_EncoderCH2_Pin|motorC_EncoderCH1_Pin);

    /* TIM2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM2_IRQn);
  /* USER CODE BEGIN TIM2_MspDeInit 1 */

  /* USER CODE END TIM2_MspDeInit 1 */
  }
  else if(tim_encoderHandle->Instance==TIM3)
  {
  /* USER CODE BEGIN TIM3_MspDeInit 0 */

  /* USER CODE END TIM3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM3_CLK_DISABLE();

    /**TIM3 GPIO Configuration
    PA6     ------> TIM3_CH1
    PA7     ------> TIM3_CH2
    */
    HAL_GPIO_DeInit(GPIOA, motorA_EncoderCH1_Pin|motorA_EncoderCH2_Pin);

    /* TIM3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM3_IRQn);
  /* USER CODE BEGIN TIM3_MspDeInit 1 */

  /* USER CODE END TIM3_MspDeInit 1 */
  }
  else if(tim_encoderHandle->Instance==TIM4)
  {
  /* USER CODE BEGIN TIM4_MspDeInit 0 */

  /* USER CODE END TIM4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM4_CLK_DISABLE();

    /**TIM4 GPIO Configuration
    PD12     ------> TIM4_CH1
    PD13     ------> TIM4_CH2
    */
    HAL_GPIO_DeInit(GPIOD, motorB_EncoderCH2_Pin|motorB_EncoderCH1_Pin);

    /* TIM4 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM4_IRQn);
  /* USER CODE BEGIN TIM4_MspDeInit 1 */

  /* USER CODE END TIM4_MspDeInit 1 */
  }
  else if(tim_encoderHandle->Instance==TIM8)
  {
  /* USER CODE BEGIN TIM8_MspDeInit 0 */

  /* USER CODE END TIM8_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_TIM8_CLK_DISABLE();

    /**TIM8 GPIO Configuration
    PC6     ------> TIM8_CH1
    PC7     ------> TIM8_CH2
    */
    HAL_GPIO_DeInit(GPIOC, motorD_EncoderCH2_Pin|motorD_EncoderCH1_Pin);

    /* TIM8 interrupt Deinit */
    HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
  /* USER CODE BEGIN TIM8_MspDeInit 1 */

  /* USER CODE END TIM8_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
void motor_setspeed(int16_t speedA,int16_t speedB,int16_t speedC,int16_t speedD)
{
    if (speedA > 0)							//���������ת���ٶ��?
	{
		HAL_GPIO_WritePin(motorA_IN1_GPIO_Port, motorA_IN1_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorA_IN1_GPIO_Port, motorA_IN2_Pin, GPIO_PIN_RESET);	//PE8�õ͵�ƽ�����÷���Ϊ��ת
	}
	else if	(speedA < 0)								//���򣬼����÷�ת���ٶ�ֵ
	{
		HAL_GPIO_WritePin(motorA_IN1_GPIO_Port, motorA_IN2_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorA_IN1_GPIO_Port, motorA_IN1_Pin, GPIO_PIN_RESET);  //PE8�õ͵�ƽ�����÷���Ϊ��ת
        speedA = -speedA;
	}
    else                                              //�ٶ�Ϊ�㣬�رյ��?
    {
        HAL_GPIO_WritePin(motorA_IN1_GPIO_Port, motorA_IN2_Pin, GPIO_PIN_RESET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorA_IN1_GPIO_Port, motorA_IN1_Pin, GPIO_PIN_RESET);
    }
    
    if (speedB > 0)							//���������ת���ٶ��?
	{
		HAL_GPIO_WritePin(motorB_IN1_GPIO_Port, motorB_IN1_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorB_IN1_GPIO_Port, motorB_IN2_Pin, GPIO_PIN_RESET);	//PE8�õ͵�ƽ�����÷���Ϊ��ת
	}
	else if	(speedB < 0)									//���򣬼����÷�ת���ٶ�ֵ
	{
		HAL_GPIO_WritePin(motorB_IN1_GPIO_Port, motorB_IN2_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorB_IN1_GPIO_Port, motorB_IN1_Pin, GPIO_PIN_RESET);  //PE8�õ͵�ƽ�����÷���Ϊ��ת
        speedB = -speedB;
	}
    else                                                //�ٶ�Ϊ�㣬�رյ��?
    {
        HAL_GPIO_WritePin(motorB_IN1_GPIO_Port, motorB_IN2_Pin, GPIO_PIN_RESET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorB_IN1_GPIO_Port, motorB_IN1_Pin, GPIO_PIN_RESET);
    }
    
    if (speedC > 0)							//���������ת���ٶ��?
	{
		HAL_GPIO_WritePin(motorC_IN1_GPIO_Port, motorC_IN1_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorC_IN1_GPIO_Port, motorC_IN2_Pin, GPIO_PIN_RESET);	//PE8�õ͵�ƽ�����÷���Ϊ��ת
	}
	else if	(speedC < 0)									//���򣬼����÷�ת���ٶ�ֵ
	{
		HAL_GPIO_WritePin(motorC_IN1_GPIO_Port, motorC_IN2_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorC_IN1_GPIO_Port, motorC_IN1_Pin, GPIO_PIN_RESET);  //PE8�õ͵�ƽ�����÷���Ϊ��ת
        speedC = -speedC;
	}  
    else                                                 //�ٶ�Ϊ�㣬�رյ��?
    {
        HAL_GPIO_WritePin(motorC_IN1_GPIO_Port, motorC_IN2_Pin, GPIO_PIN_RESET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorC_IN1_GPIO_Port, motorC_IN1_Pin, GPIO_PIN_RESET);
    }
    
    if (speedD > 0)							//���������ת���ٶ��?
	{
		HAL_GPIO_WritePin(motorD_IN1_GPIO_Port, motorD_IN1_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorD_IN1_GPIO_Port, motorD_IN2_Pin, GPIO_PIN_RESET);	//PE8�õ͵�ƽ�����÷���Ϊ��ת
	}
	else if	(speedD < 0)								//���򣬼����÷�ת���ٶ�ֵ
	{
		HAL_GPIO_WritePin(motorD_IN1_GPIO_Port, motorD_IN2_Pin, GPIO_PIN_SET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorD_IN1_GPIO_Port, motorD_IN1_Pin, GPIO_PIN_RESET);  //PE8�õ͵�ƽ�����÷���Ϊ��ת
        speedD = -speedD;
    }
    else                                                 //�ٶ�Ϊ�㣬�رյ��?
    {
        HAL_GPIO_WritePin(motorD_IN1_GPIO_Port, motorD_IN2_Pin, GPIO_PIN_RESET);	//PE7�øߵ�ƽ
		HAL_GPIO_WritePin(motorD_IN1_GPIO_Port, motorD_IN1_Pin, GPIO_PIN_RESET);
    }
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,speedA);				//PWM����Ϊ�ٶ�ֵ
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2,speedB);				//PWM����Ϊ�ٶ�ֵ
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3,speedC);				//PWM����Ϊ�ٶ�ֵ
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4,speedD);				//PWM

}

int get_encodeA(void)
{
    return ( int32_t )__HAL_TIM_GET_COUNTER(&htim3) + encodeA_count * 65536;       /* ��ǰ����ֵ+֮ǰ�ۼƱ�������ֵ=�ܵı�����ֵ */
}

int get_encodeB(void)
{
    return ( int32_t )__HAL_TIM_GET_COUNTER(&htim4) + encodeB_count * 65536;       /* ��ǰ����ֵ+֮ǰ�ۼƱ�������ֵ=�ܵı�����ֵ */
}

int get_encodeC(void)
{
    return ( int32_t )__HAL_TIM_GET_COUNTER(&htim2) + encodeC_count * 65536;       /* ��ǰ����ֵ+֮ǰ�ۼƱ�������ֵ=�ܵı�����ֵ */
}
int get_encodeD(void)
{
    return ( int32_t )__HAL_TIM_GET_COUNTER(&htim8) + encodeD_count * 65536;       /* ��ǰ����ֵ+֮ǰ�ۼƱ�������ֵ=�ܵı�����ֵ */
}

float get_speedA_1s(int8_t ms)
{
      EncodeA_now = get_encodeA();                             /* ��ȡ������ֵ�����ڼ����ٶ� */
      varA = EncodeA_now - EncodeA_old;
      EncodeA_old = EncodeA_now;
    
      return (varA*(1000/ms)/4/13/30);
}

float get_speedB_1s(int8_t ms)
{
      EncodeB_now = get_encodeB();                             /* ��ȡ������ֵ�����ڼ����ٶ� */
      varB = EncodeB_now - EncodeB_old;
      EncodeB_old = EncodeB_now;
    
      return (varB*(1000/ms)/4/13/30);
}

float get_speedC_1s(int8_t ms)
{
      EncodeC_now = get_encodeC();                             /* ��ȡ������ֵ�����ڼ����ٶ� */
      varC = EncodeC_now - EncodeC_old;
      EncodeC_old = EncodeC_now;
    
      return (varC*(1000/ms)/4/13/30);
}

float get_speedD_1s(int8_t ms)
{
      EncodeD_now = get_encodeD();                             /* ��ȡ������ֵ�����ڼ����ٶ� */
      varD = EncodeD_now - EncodeD_old;
      EncodeD_old = EncodeD_now;
    
      return (varD*(1000/ms)/4/13/30);
}



int16_t Velocity_motorA_FeedbackControl(float TargetVelocity_A, float CurrentVelocity_A)
{
    float Bias_A; //当前误差变量
    static float Last_Bias_A = 0; //上次误差
    static float Last_Last_Bias_A = 0; //上上次误差(用于微分项)
    static int16_t ControlVelocity_A;
    Bias_A=TargetVelocity_A-CurrentVelocity_A; //计算速度偏差
    // 增量式PID控制：P项(比例) + I项(积分) + D项(微分)
    ControlVelocity_A+=Velcity_Kp*(Bias_A-Last_Bias_A)+Velcity_Ki*Bias_A+Velcity_Kd*(Bias_A-2*Last_Bias_A+Last_Last_Bias_A);
    Last_Last_Bias_A = Last_Bias_A;
    Last_Bias_A=Bias_A;
    if(ControlVelocity_A>=1000)  ControlVelocity_A=1000;
    if(ControlVelocity_A<=-1000)  ControlVelocity_A=-1000;
    return ControlVelocity_A; //返回速度控制值
}

int16_t Velocity_motorB_FeedbackControl(float TargetVelocity_B, float CurrentVelocity_B)
{
    float Bias_B; //当前误差变量
    static float Last_Bias_B = 0; //上次误差
    static float Last_Last_Bias_B = 0; //上上次误差(用于微分项)
    static int16_t ControlVelocity_B;
    Bias_B=TargetVelocity_B-CurrentVelocity_B; //计算速度偏差
    // 增量式PID控制：P项(比例) + I项(积分) + D项(微分)
    ControlVelocity_B+=Velcity_Kp*(Bias_B-Last_Bias_B)+Velcity_Ki*Bias_B+Velcity_Kd*(Bias_B-2*Last_Bias_B+Last_Last_Bias_B);
    Last_Last_Bias_B = Last_Bias_B;
    Last_Bias_B=Bias_B;
    if(ControlVelocity_B>=1000)  ControlVelocity_B=1000;
    if(ControlVelocity_B<=-1000)  ControlVelocity_B=-1000;
    return ControlVelocity_B; //返回速度控制值
}

int16_t Velocity_motorC_FeedbackControl(float TargetVelocity_C, float CurrentVelocity_C)
{
    float Bias_C; //当前误差变量
    static float Last_Bias_C = 0; //上次误差
    static float Last_Last_Bias_C = 0; //上上次误差(用于微分项)
    static int16_t ControlVelocity_C;
    Bias_C=TargetVelocity_C-CurrentVelocity_C; //计算速度偏差
    // 增量式PID控制：P项(比例) + I项(积分) + D项(微分)
    ControlVelocity_C+=Velcity_Kp*(Bias_C-Last_Bias_C)+Velcity_Ki*Bias_C+Velcity_Kd*(Bias_C-2*Last_Bias_C+Last_Last_Bias_C);
    Last_Last_Bias_C = Last_Bias_C;
    Last_Bias_C=Bias_C;
    if(ControlVelocity_C>=1000)  ControlVelocity_C=1000;
    if(ControlVelocity_C<=-1000)  ControlVelocity_C=-1000;
    return ControlVelocity_C; //返回速度控制值
}

int16_t Velocity_motorD_FeedbackControl(float TargetVelocity_D, float CurrentVelocity_D)
{
    float Bias_D; //当前误差变量
    static float Last_Bias_D = 0; //上次误差
    static float Last_Last_Bias_D = 0; //上上次误差(用于微分项)
    static int16_t ControlVelocity_D;
    Bias_D=TargetVelocity_D-CurrentVelocity_D; //计算速度偏差
    // 增量式PID控制：P项(比例) + I项(积分) + D项(微分)
    ControlVelocity_D+=Velcity_Kp*(Bias_D-Last_Bias_D)+Velcity_Ki*Bias_D+Velcity_Kd*(Bias_D-2*Last_Bias_D+Last_Last_Bias_D);
    Last_Last_Bias_D = Last_Bias_D;
    Last_Bias_D=Bias_D;
    if(ControlVelocity_D>=1000)  ControlVelocity_D=1000;
    if(ControlVelocity_D<=-1000)  ControlVelocity_D=-1000;
    return ControlVelocity_D; //返回速度控制值
}
/* USER CODE END 1 */
