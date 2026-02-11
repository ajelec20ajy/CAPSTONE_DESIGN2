/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
I2C_HandleTypeDef hi2c1;
I2C_HandleTypeDef hi2c2;
DMA_HandleTypeDef hdma_i2c1_rx;
DMA_HandleTypeDef hdma_i2c1_tx;
DMA_HandleTypeDef hdma_i2c2_tx;

SPI_HandleTypeDef hspi3;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart2;

/* Definitions for SENSOR */
osThreadId_t SENSORHandle;
const osThreadAttr_t SENSOR_attributes = {
  .name = "SENSOR",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityRealtime,
};
/* Definitions for NAVIGATE */
osThreadId_t NAVIGATEHandle;
const osThreadAttr_t NAVIGATE_attributes = {
  .name = "NAVIGATE",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for CONTROL */
osThreadId_t CONTROLHandle;
const osThreadAttr_t CONTROL_attributes = {
  .name = "CONTROL",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for LCD */
osThreadId_t LCDHandle;
const osThreadAttr_t LCD_attributes = {
  .name = "LCD",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for g_DataMutex */
osMutexId_t g_DataMutexHandle;
const osMutexAttr_t g_DataMutex_attributes = {
  .name = "g_DataMutex"
};
/* Definitions for lcdI2cSem */
osSemaphoreId_t lcdI2cSemHandle;
const osSemaphoreAttr_t lcdI2cSem_attributes = {
  .name = "lcdI2cSem"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM4_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI3_Init(void);
void sensorTask(void *argument);
void naviTask(void *argument);
void controlTask(void *argument);
void lcdTask(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#include <stdio.h> // lcd에다가 sprintf 써야되서
#include <math.h> // 오도메트리 등에 쓸 cosf등
#include "types.h" // 구조체들
#include "app_pid.h" // PID 제어
#include "app_imu.h" // 오도메트리의 헤딩에다가 쓸 3가지 센서들 구동
#include "bsp_motor_driver.h" // 모터 드라이버 구동
#include "app_encoder.h" // 오도메트리의 주행거리 위한 엔코더
#include "bsp_servo.h" // 조향을 위한 서보
#include "app_pure_pursuit.h" // 주행 알고리즘인 Pure-Pursuit 계산용
#include "app_lcd.h" // lcd 표시용
#include "app_nRF24.h" // 헤더 포함

extern float Straight_Distance; // lcd 출력용

Waypoint_t my_path[5];
#define TARGET_NODE_ID 1
uint16_t current_cds_value;

// motor_pid = {Kp, Ki, prev_error, target_rpm, current_rpm, integral_sum} 등... types.h에 각 구조체와 설명있음.
PID_Config motor_pid_R = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 999}; // 후륜 우측 모터용 PID 파라미터. 주행 목표 rpm에 따라 3가지 중 하나 고르게 해둠.
PID_Config motor_pid_L = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 999}; // // 후륜 좌측 모터용 PID 파라미터. List는 Pure_Pursuit.c 가장 하단에

SharedData_t g_RobotData;   // 로봇 상태 및 미션 정보
PP_Handle_t g_PPHandle;     // Pure Pursuit 관리자

float heading; // 그냥 Live로 헤딩 보기 위함
float heading_deg; // 마찬가지
float steer_cmd;
size_t myFreeHeap = 0;
size_t myMinHeap = 0;
static inline void DWT_Init(void)
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
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
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_I2C2_Init();
  MX_SPI3_Init();
  /* USER CODE BEGIN 2 */
  HAL_Delay(1000);
  g_RobotData.cmd.state = STATE_IDLE;

  // NRF 코드
  APP_NRF24_Init();
  APP_NRF24_WaitForData(5000);
  // 데이터 파싱 및 저장
  if (g_rx_complete_flag[TARGET_NODE_ID]){ // 1번 파이프에서 데이터 수신 성공 시
	  current_cds_value = g_node_data[TARGET_NODE_ID].cds_value;  // 조도 센서 값 가져오기
	  memcpy(my_path, g_node_data[TARGET_NODE_ID].path, sizeof(my_path));
  }
  else{
	  // 수신 실패 시 (타임아웃)
	  memset(my_path, 0, sizeof(my_path));
  	  // 수신 실패 표시 (LED 깜빡임 5회)
  	  for(int i=0; i<5; i++) {
  		  HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  		  HAL_Delay(200);
  	  }
  }

  // 디버깅용 DWT
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CYCCNT = 0;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

  Init_Servo(); // 서보 pwm 시작 및 정면 정렬
  HAL_Delay(1000);

  Init_Sensors(); // ACC, MAG, GYRO INIT


  Init_Encoder(); // 엔코더 초기화:pwm시작

  Init_Motor_Driver(); // 모터 초기 세팅(STBY 등)

  g_RobotData.cmd.state = STATE_RUNNING;

  DWT_Init();
  xTraceEnable(TRC_START);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of g_DataMutex */
  g_DataMutexHandle = osMutexNew(&g_DataMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* creation of lcdI2cSem */
  lcdI2cSemHandle = osSemaphoreNew(1, 1, &lcdI2cSem_attributes);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of SENSOR */
  SENSORHandle = osThreadNew(sensorTask, NULL, &SENSOR_attributes);

  /* creation of NAVIGATE */
  NAVIGATEHandle = osThreadNew(naviTask, NULL, &NAVIGATE_attributes);

  /* creation of CONTROL */
  CONTROLHandle = osThreadNew(controlTask, NULL, &CONTROL_attributes);

  /* creation of LCD */
  LCDHandle = osThreadNew(lcdTask, NULL, &LCD_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1
                              |RCC_PERIPHCLK_I2C2|RCC_PERIPHCLK_TIM1
                              |RCC_PERIPHCLK_TIM2|RCC_PERIPHCLK_TIM34;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_HSI;
  PeriphClkInit.Tim1ClockSelection = RCC_TIM1CLK_HCLK;
  PeriphClkInit.Tim2ClockSelection = RCC_TIM2CLK_HCLK;
  PeriphClkInit.Tim34ClockSelection = RCC_TIM34CLK_HCLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x0010020A;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x0010020A;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_4BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 7;
  hspi3.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi3.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief TIM1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM1_Init(void)
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
  htim1.Init.Prescaler = 4-1;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 1000-1;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
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
  sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.BreakFilter = 0;
  sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
  sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
  sBreakDeadTimeConfig.Break2Filter = 0;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */
  HAL_TIM_MspPostInit(&htim1);

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 20000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
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
  htim3.Init.Period = 0xFFFF;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
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

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief TIM4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM4_Init(void)
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
  htim4.Init.Period = 0xFFFF;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC1Filter = 0;
  sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
  sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
  sConfig.IC2Filter = 0;
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

  /* USER CODE END TIM4_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, BIN1_Pin|BIN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, CE_Pin|LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, DEBUG_LED_4_Pin|STBY_Pin|DEBUG_LED_1_Pin|DEBUG_LED_2_Pin
                          |DEBUG_LED_3_Pin|AIN1_Pin|AIN2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : BIN1_Pin BIN2_Pin */
  GPIO_InitStruct.Pin = BIN1_Pin|BIN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : CE_Pin LED_Pin */
  GPIO_InitStruct.Pin = CE_Pin|LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : DEBUG_LED_4_Pin STBY_Pin DEBUG_LED_1_Pin DEBUG_LED_2_Pin
                           DEBUG_LED_3_Pin AIN1_Pin AIN2_Pin */
  GPIO_InitStruct.Pin = DEBUG_LED_4_Pin|STBY_Pin|DEBUG_LED_1_Pin|DEBUG_LED_2_Pin
                          |DEBUG_LED_3_Pin|AIN1_Pin|AIN2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : CS_Pin */
  GPIO_InitStruct.Pin = CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(CS_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_sensorTask */
/**
  * @brief  Function implementing the SENSOR thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_sensorTask */
void sensorTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  uint32_t tick = osKernelGetTickCount();
  // 좌표 등 저장할 로컬 변수
  float local_x = 0.0f;
  float local_y = 0.0f;
  float current_heading = 0.0f;
  float last_dist = 0.0f;
  float velocity = 0.0f;
  float rpm = 0.0f;
  const uint32_t kPeriod = 10U; // 10ms 주기 (100Hz)

  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_WritePin(DEBUG_LED_1_GPIO_Port, DEBUG_LED_1_Pin, GPIO_PIN_SET); // 주기 디버깅용
	  // 센서 값 읽기 및 오도메트리
	  heading = Get_Fused_Heading();
	  heading_deg = heading * 57.29f; // deg 변환
	  current_heading = heading;
      // 거리 변화량 계산
      float curr_dist = Encoder_GetDist(); // 두 구동 모터 주행 거리의 평균
      float d_dist = curr_dist - last_dist;
      last_dist = curr_dist;
      velocity = d_dist / (kPeriod/1000.0f); // 평균 주행 거리
      rpm = (velocity * 60.0f) / CIRCUMFERENCE_M;; // 평균 rpm
      local_x += d_dist * cosf(current_heading); // 이동한 거리만큼 현재 바라보는 방향으로 분해해서 더함
      local_y += d_dist * sinf(current_heading);

      // 공유 데이터 업데이트 오도메트리 수행 결과(좌표) 업데이트해주고, 현재 rpm도 일단은 기록해둠(표시용)
      osMutexAcquire(g_DataMutexHandle, osWaitForever);
      g_RobotData.pose.x = local_x;
      g_RobotData.pose.y = local_y;
      g_RobotData.pose.theta_rad = current_heading; // 퓨전된 각도 저장
      g_RobotData.pose.v_ms = velocity; //[m/ms]
      g_RobotData.pose.RPM = rpm;
      osMutexRelease(g_DataMutexHandle);

      HAL_GPIO_WritePin(DEBUG_LED_1_GPIO_Port, DEBUG_LED_1_Pin, GPIO_PIN_RESET); // 주기 디버깅용
      // 주기 유지
      tick += kPeriod;
      osDelayUntil(tick);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_naviTask */
/**
* @brief Function implementing the NAVIGATE thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_naviTask */
void naviTask(void *argument)
{
  /* USER CODE BEGIN naviTask */
  ControlCommand_t cmd_msg = {0}; // 로컬 명령 변수
  RobotPose_t current_pose_copy; // g_RobotData 복사해올 변수
  PP_Init(&g_PPHandle, my_path, sizeof(my_path)/sizeof(Waypoint_t)); // Waypoint 관리 초기화
  uint32_t tick = osKernelGetTickCount();
  const uint32_t kPeriod = 50U; // 50ms 주기 (20Hz)
  // 서보가 (6v 기준) 140ms당 60도 돌음. 즉, 0.43도 / ms임. 즉 10ms마다 4.3도 돌아가니깐 50ms 주기가 적당할듯.
  // 초기 상태 설정
  osMutexAcquire(g_DataMutexHandle, osWaitForever);
  g_RobotData.cmd.state = STATE_RUNNING;
  osMutexRelease(g_DataMutexHandle);
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_WritePin(DEBUG_LED_2_GPIO_Port, DEBUG_LED_2_Pin, GPIO_PIN_SET); // 주기 디버깅용

	  // 현재 상태 가져오기
      osMutexAcquire(g_DataMutexHandle, osWaitForever);
      current_pose_copy = g_RobotData.pose;
      MissionState_e current_state = g_RobotData.cmd.state;
      osMutexRelease(g_DataMutexHandle);

      // Pure Pursuit 계산
      if (current_state == STATE_RUNNING) {
          cmd_msg = PP_Compute(&g_PPHandle, &current_pose_copy); // 여기서 cmd에다가 지령값들 업데이트 해줌
          if (cmd_msg.e_stop == 1) { // 종료면 종료 상태로 변경 (이것만 따로 Mutex로 바로 업데이트)
              osMutexAcquire(g_DataMutexHandle, osWaitForever);
              g_RobotData.cmd.state = STATE_FINISHED; // Finished로 종료
              osMutexRelease(g_DataMutexHandle);
          }
      }
      else { // 어떤 이유로 정지나 아이들일때의 보험
          cmd_msg.target_motor_rpm = 0.0f; // 정지
          cmd_msg.target_servo_angle = 0.0f; // 정지
          cmd_msg.e_stop = 1; // 정지
      }

      steer_cmd = cmd_msg.target_servo_angle * 180 / M_PI;

      // 지령 저장
      osMutexAcquire(g_DataMutexHandle, osWaitForever);
      g_RobotData.cmd.target_speed = cmd_msg.target_motor_rpm; // RPM 지령 저장
      g_RobotData.cmd.rpmR = cmd_msg.target_motor_rpm_R;
      g_RobotData.cmd.rpmL = cmd_msg.target_motor_rpm_L;
      g_RobotData.cmd.target_steer = cmd_msg.target_servo_angle; // 서보 지령 저장
      osMutexRelease(g_DataMutexHandle);

      /*
      myFreeHeap = xPortGetFreeHeapSize(); // 걍 메모리볼라고
      myMinHeap = xPortGetMinimumEverFreeHeapSize();
	  */

      HAL_GPIO_WritePin(DEBUG_LED_2_GPIO_Port, DEBUG_LED_2_Pin, GPIO_PIN_RESET); // 주기 디버깅용
      // 주기 유지
      tick += kPeriod;
      osDelayUntil(tick);
  }
  /* USER CODE END naviTask */
}

/* USER CODE BEGIN Header_controlTask */
/**
* @brief Function implementing the CONTROL thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_controlTask */
void controlTask(void *argument)
{
  /* USER CODE BEGIN controlTask */
  uint16_t servo_ccr = 1500;
  uint32_t tick = osKernelGetTickCount();
  const uint32_t kPeriod = 20U; // 20ms 주기 (50Hz)
  RobotCommand_t target_cmd = {0};
  static uint16_t last_cntR = 0; // Delta 계산을 위한
  static uint16_t last_cntL = 0; // Delta 계산을 위한
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_WritePin(DEBUG_LED_3_GPIO_Port, DEBUG_LED_3_Pin, GPIO_PIN_SET); // 주기 디버깅용
	  osMutexAcquire(g_DataMutexHandle, osWaitForever);
	  target_cmd = g_RobotData.cmd;
	  osMutexRelease(g_DataMutexHandle);

	  if(target_cmd.state == STATE_RUNNING){

		  // 데이터 읽기
		  motor_pid_R.target_rpm = target_cmd.rpmR;
		  motor_pid_L.target_rpm = target_cmd.rpmL;


		  // 엔코더 읽기
          uint16_t cur_cntR = (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
          uint16_t cur_cntL = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);

          // Delta
          // 16비트 unsigned 차이를 계산한 뒤 int16_t로 캐스팅하면 타이머가 65535에서 0으로 넘어가거나 반대로 돌아가는 오버플로우 자동 처리
          int16_t diff_count_R = (int16_t)(cur_cntR - last_cntR);
          int16_t diff_count_L = (int16_t)(cur_cntL - last_cntL);
          last_cntR = cur_cntR;// 다음 루프를 위해 현재 값 저장
          last_cntL = cur_cntL;

          //현재 RPM 계산
          motor_pid_R.current_rpm = (float)diff_count_R * RPM_CONVERSION_FACTOR_R;
          motor_pid_L.current_rpm = (float)diff_count_L * RPM_CONVERSION_FACTOR_L;

          // PID 계산
          PID_Compute_R(motor_pid_R.target_rpm, motor_pid_R.current_rpm);
          PID_Compute_L(motor_pid_L.target_rpm, motor_pid_L.current_rpm);
          Motor_Drive_TIM1_R(motor_pid_R.out_pwm);
          Motor_Drive_TIM1_L(motor_pid_L.out_pwm);

	  }

	  else{
		  // 정지 상태 혹은 에러 시 출력 차단
          Motor_Drive_TIM1_R(0);
          Motor_Drive_TIM1_L(0);

          // 정지 중에도 카운터는 계속 돌고 있으므로 last_cnt를 업데이트
          last_cntR = (uint16_t)__HAL_TIM_GET_COUNTER(&htim4);
          last_cntL = (uint16_t)__HAL_TIM_GET_COUNTER(&htim3);
	  }

	  // 서보 조향
      servo_ccr = radian_to_servo_ccr(target_cmd.target_steer); //target_servo_anlge에서 ccr 변환할때, 소수점(ccr)은 버려짐
      CHANGE_SERVO_PWM(servo_ccr);

      HAL_GPIO_WritePin(DEBUG_LED_3_GPIO_Port, DEBUG_LED_3_Pin, GPIO_PIN_RESET); // 주기 디버깅용
	  // 주기 유지
      tick += kPeriod;
      osDelayUntil(tick);
  }
  /* USER CODE END controlTask */
}

/* USER CODE BEGIN Header_lcdTask */
/**
* @brief Function implementing the LCD thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_lcdTask */
void lcdTask(void *argument)
{
  /* USER CODE BEGIN lcdTask */
  uint32_t tick = osKernelGetTickCount();
  const uint32_t kPeriod = 500U; // 500ms 주기 (50Hz)
  lcd_init(); // lcd 초기설정
  char line1[30]; // lcd에 보낼거(윗줄)
  char line2[30]; // lcd에 보낼거(아랫줄)
  /* Infinite loop */
  for(;;)
  {
	  HAL_GPIO_WritePin(DEBUG_LED_4_GPIO_Port, DEBUG_LED_4_Pin, GPIO_PIN_SET); // 주기 디버깅용
	  sprintf(line1, "D:%5.1f R:%5.0f", Straight_Distance, g_RobotData.pose.RPM);
	  sprintf(line2, "Path Idx: %5d", g_PPHandle.current_idx);

	  // 버퍼 비우기
	  lcd_buf_clear();

	  // 화면 데이터 버퍼에 그리기 (실제 전송 X)
	  lcd_put_cursor_buf(0, 0);
	  lcd_send_string_buf(line1);
	  lcd_put_cursor_buf(1, 0);
	  lcd_send_string_buf(line2);

	  // 쌓인 데이터를 DMA (여기서 CPU는 다른 일을 함)
	  lcd_flush_dma();

	  HAL_GPIO_WritePin(DEBUG_LED_4_GPIO_Port, DEBUG_LED_4_Pin, GPIO_PIN_RESET); // 주기 디버깅용

	  // 주기 유지
      tick += kPeriod;
      osDelayUntil(tick);
  }
  /* USER CODE END lcdTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
#ifdef USE_FULL_ASSERT
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
