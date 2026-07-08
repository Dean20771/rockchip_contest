/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "dma.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "delay.h"
#include <stdio.h>
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
unsigned int proc_slow;
unsigned char key_flag;
static unsigned char key_val,key_down,key_up,key_old;
unsigned char keytime_flag;
uint16_t keytime_count;

extern float targetA;
extern float targetB;
extern float targetC;
extern float targetD;

// ======================== 机器人参数配置区 ========================
// 请根据您的机器人修改以下参数

#define ROBOT_TYPE  1  // 0=差速驱动, 1=麦克纳姆轮, 2=全向轮

// 轮子参数
#define WHEEL_DIAMETER   0.092f   // 轮子直径 (米) - 请修改为实际值
#define WHEEL_RADIUS     (WHEEL_DIAMETER / 2.0f)
#define WHEEL_BASE       0.210f   // 轮距：左右轮中心距离 (米) - 请修改
#define WHEEL_TRACK      0.170f   // 轴距：前后轮中心距离 (米) - 麦克纳姆轮/全向轮需要

// 编码器参数
#define ENCODER_PPR      1560     // 编码器每转脉冲数 - 请修改
#define GEAR_RATIO       30.0f    // 减速比 - 请修改
#define PULSES_PER_REV   (ENCODER_PPR * GEAR_RATIO)  // 轮子转一圈的总脉冲数

// 计算常数
#define PI               3.14159265358979f
#define WHEEL_PERIMETER  (2.0f * PI * WHEEL_RADIUS)         // 轮子周长
#define METER_PER_PULSE  (WHEEL_PERIMETER / PULSES_PER_REV) // 每个脉冲对应的距离

// 采样周期 (秒) - 与发送频率对应
#define DT               0.1f     // 100ms = 0.1s，如修改HAL_Delay需同步修改

// IMU配置
#define USE_IMU          1        // 1=使用IMU修正角度, 0=仅使用编码器
#define IMU_UART         huart2   // IMU连接的串口，请修改为实际串口（如huart2、huart3等）

// ======================== WT901BCTTL IMU数据 ========================
typedef struct {
    float roll;       // 横滚角 (rad)
    float pitch;      // 俯仰角 (rad)
    float yaw;        // 偏航角 (rad) - 用于里程计
    float gyro_x;     // X轴角速度 (rad/s)
    float gyro_y;     // Y轴角速度 (rad/s)
    float gyro_z;     // Z轴角速度 (rad/s) - 用于里程计
    float accel_x;    // X轴加速度 (m/s²)
    float accel_y;    // Y轴加速度 (m/s²)
    float accel_z;    // Z轴加速度 (m/s²)
    uint8_t updated;  // 数据更新标志
} IMU_Data_t;

IMU_Data_t imu_data = {0};

// WT901BCTTL接收缓冲区
uint8_t imu_rx_buffer[11];  // WT901BCTTL单帧数据11字节
uint8_t imu_rx_state = 0;   // 接收状态机
uint8_t imu_rx_index = 0;   // 接收索引

// ======================== 编码器数据 ========================
typedef struct {
    int32_t left;         // 左轮编码器计数
    int32_t right;        // 右轮编码器计数
    int32_t front_left;   // 左前轮编码器（麦克纳姆轮/全向轮）
    int32_t front_right;  // 右前轮编码器（麦克纳姆轮/全向轮）
    int32_t last_left;    // 上次左轮计数
    int32_t last_right;   // 上次右轮计数
    int32_t last_front_left;
    int32_t last_front_right;
} Encoder_t;

Encoder_t encoder = {0};

// ======================== 里程计数据 ========================
typedef struct {
    float x;   // 累积坐标X (m)
    float y;   // 累积坐标Y (m)
    float th;  // 角度Theta (rad)
    float vx;  // X方向线速度 (m/s)
    float vy;  // Y方向线速度 (m/s)
    float wz;  // Z轴角速度 (rad/s)
} OdomData_t;

OdomData_t odom_data = {0};

// ======================== 里程计计算函数 ========================
void Update_Odometry(void)
{
    // TODO: 从编码器获取当前计数值，请根据实际硬件修改
    // 示例：encoder.left = __HAL_TIM_GET_COUNTER(&htimX);
    // 示例：encoder.right = __HAL_TIM_GET_COUNTER(&htimY);

    // 计算编码器增量
    int32_t delta_left = encoder.left - encoder.last_left;
    int32_t delta_right = encoder.right - encoder.last_right;

    // 保存当前值作为下次的上次值
    encoder.last_left = encoder.left;
    encoder.last_right = encoder.right;

#if ROBOT_TYPE == 0  // 差速驱动机器人
    // 计算左右轮移动距离
    float dist_left = delta_left * METER_PER_PULSE;
    float dist_right = delta_right * METER_PER_PULSE;

    // 计算机器人中心移动距离
    float dist_center = (dist_left + dist_right) / 2.0f;

#if USE_IMU == 1
    // 使用IMU的yaw角作为机器人角度（更精确）
    if(imu_data.updated) {
        odom_data.th = imu_data.yaw;
        odom_data.wz = imu_data.gyro_z;  // 使用IMU的角速度
        imu_data.updated = 0;
    }

    // 使用IMU角度更新位置
    float delta_x = dist_center * cosf(odom_data.th);
    float delta_y = dist_center * sinf(odom_data.th);

    odom_data.x += delta_x;
    odom_data.y += delta_y;

    // 计算线速度
    odom_data.vx = dist_center / DT;
    odom_data.vy = 0.0f;  // 差速驱动没有侧向速度
    // wz已经从IMU获取

#else
    // 仅使用编码器计算（无IMU）
    float delta_theta = (dist_right - dist_left) / WHEEL_BASE;

    // 更新位置（使用中点法，更精确）
    float delta_x = dist_center * cosf(odom_data.th + delta_theta / 2.0f);
    float delta_y = dist_center * sinf(odom_data.th + delta_theta / 2.0f);

    odom_data.x += delta_x;
    odom_data.y += delta_y;
    odom_data.th += delta_theta;

    // 角度归一化到 [-π, π]
    while(odom_data.th > PI) odom_data.th -= 2.0f * PI;
    while(odom_data.th < -PI) odom_data.th += 2.0f * PI;

    // 计算速度
    odom_data.vx = dist_center / DT;
    odom_data.vy = 0.0f;  // 差速驱动没有侧向速度
    odom_data.wz = delta_theta / DT;
#endif

#elif ROBOT_TYPE == 1  // 麦克纳姆轮机器人
    // 获取四个轮子的编码器增量
    int32_t delta_fl = encoder.front_left - encoder.last_front_left;
    int32_t delta_fr = encoder.front_right - encoder.last_front_right;

    encoder.last_front_left = encoder.front_left;
    encoder.last_front_right = encoder.front_right;

    // 计算各轮移动距离
    float dist_fl = delta_fl * METER_PER_PULSE;
    float dist_fr = delta_fr * METER_PER_PULSE;
    float dist_rl = delta_left * METER_PER_PULSE;   // 后左
    float dist_rr = delta_right * METER_PER_PULSE;  // 后右

    // 麦克纳姆轮逆运动学
    float vx_local = (dist_fl + dist_fr + dist_rl + dist_rr) / 4.0f / DT;
    float vy_local = (-dist_fl + dist_fr + dist_rl - dist_rr) / 4.0f / DT;
    float wz_local = (-dist_fl + dist_fr - dist_rl + dist_rr) / 4.0f / (WHEEL_BASE + WHEEL_TRACK) / DT;

    // 转换到全局坐标系
    odom_data.vx = vx_local * cosf(odom_data.th) - vy_local * sinf(odom_data.th);
    odom_data.vy = vx_local * sinf(odom_data.th) + vy_local * cosf(odom_data.th);
    odom_data.wz = wz_local;

    // 更新位置
    odom_data.x += odom_data.vx * DT;
    odom_data.y += odom_data.vy * DT;
    odom_data.th += odom_data.wz * DT;

    // 角度归一化
    while(odom_data.th > PI) odom_data.th -= 2.0f * PI;
    while(odom_data.th < -PI) odom_data.th += 2.0f * PI;

#elif ROBOT_TYPE == 2  // 全向轮机器人（三轮120°布局）
    // 根据实际布局计算，此处为示例
    // 请根据您的全向轮布局修改计算公式

#endif
}

// ======================== 发送里程计数据函数 ========================
// 格式: $x,y,th,vx,vy,wz#
void Send_GyroData(void)
{
    char tx_buffer[80];
    int len;

    // 格式化里程计数据为ROS标准格式
    len = sprintf(tx_buffer, "$%.3f,%.3f,%.3f,%.3f,%.3f,%.3f#",
                  odom_data.x, odom_data.y, odom_data.th,
                  odom_data.vx, odom_data.vy, odom_data.wz);

    // 通过USART1发送数据到ROS
    HAL_UART_Transmit(&huart1, (uint8_t*)tx_buffer, len, 100);
}

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

// ======================== WT901BCTTL IMU数据解析函数 ========================
// 解析WT901BCTTL数据帧
void Parse_IMU_Data(uint8_t *data)
{
    uint8_t sum = 0;

    // 校验和计算（前10字节相加）
    for(int i = 0; i < 10; i++) {
        sum += data[i];
    }

    // 校验不通过，丢弃数据
    if(sum != data[10]) return;

    // 根据数据类型解析
    switch(data[1])
    {
        case 0x51: // 加速度数据
        {
            int16_t ax = (int16_t)(data[3] << 8 | data[2]);
            int16_t ay = (int16_t)(data[5] << 8 | data[4]);
            int16_t az = (int16_t)(data[7] << 8 | data[6]);

            // 转换为m/s²（量程±16g）
            imu_data.accel_x = ax / 32768.0f * 16.0f * 9.8f;
            imu_data.accel_y = ay / 32768.0f * 16.0f * 9.8f;
            imu_data.accel_z = az / 32768.0f * 16.0f * 9.8f;
            break;
        }

        case 0x52: // 角速度数据
        {
            int16_t gx = (int16_t)(data[3] << 8 | data[2]);
            int16_t gy = (int16_t)(data[5] << 8 | data[4]);
            int16_t gz = (int16_t)(data[7] << 8 | data[6]);

            // 转换为rad/s（量程±2000°/s）
            imu_data.gyro_x = gx / 32768.0f * 2000.0f * PI / 180.0f;
            imu_data.gyro_y = gy / 32768.0f * 2000.0f * PI / 180.0f;
            imu_data.gyro_z = gz / 32768.0f * 2000.0f * PI / 180.0f;
            break;
        }

        case 0x53: // 角度数据（欧拉角）- 最重要！
        {
            int16_t roll = (int16_t)(data[3] << 8 | data[2]);
            int16_t pitch = (int16_t)(data[5] << 8 | data[4]);
            int16_t yaw = (int16_t)(data[7] << 8 | data[6]);

            // 转换为rad（量程±180°）
            imu_data.roll = roll / 32768.0f * PI;
            imu_data.pitch = pitch / 32768.0f * PI;
            imu_data.yaw = yaw / 32768.0f * PI;

            imu_data.updated = 1;  // 标记数据已更新
            break;
        }

        default:
            break;
    }
}

// WT901BCTTL串口接收中断回调函数
// 需要在main函数中启动接收中断：HAL_UART_Receive_IT(&IMU_UART, &imu_rx_buffer[0], 1);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == IMU_UART.Instance)
    {
        switch(imu_rx_state)
        {
            case 0: // 等待帧头0x55
                if(imu_rx_buffer[0] == 0x55) {
                    imu_rx_state = 1;
                    imu_rx_index = 1;
                }
                break;

            case 1: // 接收数据
                imu_rx_buffer[imu_rx_index++] = imu_rx_buffer[0];
                if(imu_rx_index >= 11) {
                    // 一帧接收完成，解析数据
                    Parse_IMU_Data(imu_rx_buffer);
                    imu_rx_state = 0;
                    imu_rx_index = 0;
                }
                break;
        }

        // 继续接收下一个字节
        HAL_UART_Receive_IT(&IMU_UART, &imu_rx_buffer[0], 1);
    }
}

void key_proc(void)
{
    if(key_flag!=0)return;
    key_flag=1;
    key_val = key_scan();
    key_down = key_val &(key_old^key_val);
    key_up = ~key_val&(key_old^key_val);
    key_old = key_val;
    
    if(key_down)
    {
        keytime_flag = 1;//��ʱ��ʼ
    }
    if(keytime_count<500)//�̰�
        {
            if(key_up)//S15̧��
            {
                keytime_flag = keytime_count = 0;//״̬��λ
                targetA += 0.5;
                if(targetA >= 5.2) targetA = 0;
                targetB += 0.5;
                if(targetB >= 5.2) targetB = 0;
                targetC += 0.5;
                if(targetC >= 5.2) targetC = 0;
                targetD += 0.5;
                if(targetD >= 5.2) targetD = 0;
                __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1,100);
            }
        }
        else//����
        {
            if(key_old)//S15����
            {
               targetA = 0;
            }
            if(key_up)//S15̧��
                keytime_flag = keytime_count = 0;//״̬��λ
        }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_RTC_Init();
  MX_TIM1_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_TIM4_Init();
  MX_TIM2_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */
    delay_init(168);

#if USE_IMU == 1
    // 启动WT901BCTTL IMU串口接收中断
    HAL_UART_Receive_IT(&IMU_UART, &imu_rx_buffer[0], 1);
#endif

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    key_proc();

    // 更新里程计数据
    Update_Odometry();

    // 发送里程计数据到ROS
    Send_GyroData();

    // 延时，避免发送过于频繁
    HAL_Delay(100);

//      targetA = 4;
//      targetB = 4;
//      targetC = 4;
//      targetD = 4;
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;  // 修正USB时钟: 336MHz/7=48MHz
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
