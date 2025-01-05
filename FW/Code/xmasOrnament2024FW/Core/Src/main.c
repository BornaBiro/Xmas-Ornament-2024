/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "WS2812.h"
#include "Buzzer.h"
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

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim14;
DMA_HandleTypeDef hdma_tim3_ch1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM3_Init(void);
static void MX_TIM14_Init(void);
/* USER CODE BEGIN PFP */
void tone(TIM_HandleTypeDef *_htim, uint32_t _freq, uint32_t _durMs);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM3_Init();
  MX_TIM14_Init();
  /* USER CODE BEGIN 2 */

  HAL_GPIO_WritePin(PERIPH_GPIO_Port, PERIPH_Pin, GPIO_PIN_RESET);
  HAL_Delay(100ULL);

  buzzer_begin(&htim14);

  if (!ws2812_begin(&htim3, &hdma_tim3_ch1, TIM_CHANNEL_1, 12, 2, 9, 6, 3))
  {
	  while(1);
  }
  for (int i = 0; i < 12; i++)
  {
	  uint32_t step = 65535 / 12;
	  ws2812_setPixelColor(i, 0);
	  ws2812_setPixelColor(i, ws2812_hsv(step * i, 255, 32));

  }
  ws2812_show();

  static const uint16_t melody1Notes[] = {968, 1620, 1447, 1287, 968, 1620, 1445, 1287, 1081, 1706, 1620, 1445, 1219, 1915, 1709, 1447, 1618, 968, 1620, 1445, 1288, 968, 1620, 1445, 1287, 1081, 1709, 1618, 1445, 1919, 2188, 1919, 1709, 1447, 1287, 1620, 1919, 1287, 1445, 1620, 1709, 1618, 1447, 1620, 1445, 1915, 1620, 1915, 1287, 1445, 1618, 1709, 1620, 1915, 1709, 1447, 1287};
  static const uint16_t melody1Durations[] = {216, 218, 217, 219, 1088, 217, 217, 219, 1088, 217, 218, 217, 871, 435, 217, 218, 871, 217, 217, 218, 217, 1089, 217, 218, 217, 1089, 217, 217, 218, 871, 217, 218, 217, 218, 871, 1960, 218, 217, 218, 871, 1089, 871, 435, 217, 436, 434, 1960, 218, 218, 217, 871, 1089, 653, 435, 217, 218, 957};
  int melodyMaxIndex = sizeof(melody1Notes) / sizeof(uint16_t);

  HAL_Delay(8000ULL);
  for (int i = 0; i < 12; i++)
  {
	  ws2812_setPixelColor(i, 0);
  }
  HAL_GPIO_WritePin(PERIPH_GPIO_Port, PERIPH_Pin, GPIO_PIN_SET);

  HAL_SuspendTick();
  HAL_TIM_PWM_DeInit(&htim14);
  HAL_TIM_PWM_DeInit(&htim3);

  HAL_PWR_DisableWakeUpPin(PWR_WAKEUP_PIN1);
  HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN1_LOW);
  __HAL_PWR_CLEAR_FLAG(PWR_FLAG_WUF);

  HAL_PWREx_EnterSHUTDOWNMode();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	  //htim1.Instance->CCR1 = 10;
	  //HAL_Delay(100ULL);
	  //htim1.Instance->CCR1 = 0;
	  //HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	  //HAL_Delay(1000ULL);
	  for (int i = 0; i < melodyMaxIndex; i++)
	  {
		  //tone(&htim14, melody1Notes[i], melody1Durations[i]);
		  buzzer_toneDuration(melody1Notes[i], melody1Durations[i], TIM_CHANNEL_1);
	  }
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

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_0);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV2;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
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

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 2;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 9;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief TIM14 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM14_Init(void)
{

  /* USER CODE BEGIN TIM14_Init 0 */

  /* USER CODE END TIM14_Init 0 */

  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM14_Init 1 */

  /* USER CODE END TIM14_Init 1 */
  htim14.Instance = TIM14;
  htim14.Init.Prescaler = 1200;
  htim14.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim14.Init.Period = 100;
  htim14.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim14.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim14) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim14, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM14_Init 2 */

  /* USER CODE END TIM14_Init 2 */
  HAL_TIM_MspPostInit(&htim14);

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);

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
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PERIPH_GPIO_Port, PERIPH_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : PERIPH_Pin */
  GPIO_InitStruct.Pin = PERIPH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PERIPH_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/*
void tone(TIM_HandleTypeDef *_htim, uint32_t _freq, uint32_t _durMs)
{
	// Calculate the prescaler.
	uint32_t _prescaler = (HAL_RCC_GetSysClockFreq() / (50 * _freq)) - 1;

	// Check if the prescaler is not overflowed.
	if (_prescaler > 65535) _prescaler = 65535;

	// Now calculate the period.
	uint32_t _period = ((HAL_RCC_GetSysClockFreq() / (_prescaler)) / _freq) - 1;

	// Set duty cycle to 50%.
	uint32_t _compare = _period / 2;

	// Set the duty to 0%.
	_htim->Instance->CCR1 = 0;

	// Stop the timer.
	HAL_TIM_PWM_Stop(_htim, TIM_CHANNEL_1);

	// Set new prescaler value.
	__HAL_TIM_SET_PRESCALER(_htim, _prescaler);

	// Set the new auto preload value.
	__HAL_TIM_SET_AUTORELOAD(_htim, _period);

	// Set new compare value.
	_htim->Instance->CCR1 = _compare;

	// Start the PWM.
	HAL_TIM_PWM_Start(_htim, TIM_CHANNEL_1);

	// Wait defined amount of time.
	HAL_Delay(_durMs);

	// Stop the PWM.
	HAL_TIM_PWM_Stop(_htim, TIM_CHANNEL_1);
}
*/
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
