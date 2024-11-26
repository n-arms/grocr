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
#include "i2c_rx.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "ham.h"
#include "lcd.h"
#include "lsr.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

uint16_t int16_from_bool(bool arr[16]) {
	uint16_t total = 0;
	for (int i = 0; i < 16; i++) {
		total = total * 2 + arr[i];
	}
	return total;
}

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim3;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

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
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */


	I2C_rx_config i2c;
	i2c.clock_gpio = GPIOC;
	i2c.clock_pin = GPIO_PIN_0;
	i2c.data_gpio = GPIOC;
	i2c.data_pin = GPIO_PIN_1;
	bool data[48];
	I2C_rx_driver dr = new_I2C_rx_driver(config, data, 48, 500);

	lcd_t lcd;
	lcd.gpio_etc = GPIOA;
	lcd.gpio_data = GPIOB;
	lcd.pin_select = GPIO_PIN_10;
	lcd.pin_enable = GPIO_PIN_6;
	lcd.pin1 = GPIO_PIN_3;
	lcd.pin2 = GPIO_PIN_5;
	lcd.pin3 = GPIO_PIN_4;
	lcd.pin4 = GPIO_PIN_10;

	lcd_reset(&lcd);
	lcd_str(&lcd, "press button to", "zero");

	uint32_t t0;
	float empty;

	bool btn_prs = false, first = false;

	uint32_t index = 0;
	uint32_t time_data[1000];
	float lc_data[1000];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	for (;;) {
		float weight;

		tick_I2C_rx_driver(&dr);
		if (poll_I2C_driver(&dr)) {
			bool data[48];
			memcpy(data, get_I2C_driver(&dr), 48);
			reset_I2C_driver(&dr);

			uint16_t data1 = ham_dec(int16_from_bool(data));
			uint16_t data2 = ham_dec(int16_from_bool(data + 16));
			uint16_t data3 = ham_dec(int16_from_bool(data + 32));
			
			if (data1 == (uint16_t)-1 || data2 == (uint16_t)-1 ||
					data3 == (uint16_t)-1)
				continue;
			weight = (data1 << 22 | data2 << 11 | data3) * lc_const;
		}

		if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) { /* button is pressed */
			empty = weight;
			t0 = HAL_GetTick();
			index = 0;

			btn_prs = true;
			continue;
		}

		if (!btn_prs)
			continue;
		if (abs(weight - empty) < 20 || weight < empty)
			continue;
		if (!first && abs(weight - lc_data[index - 1]) < 20)
			continue;

		first = true;
		lc_data[index] = weight;
		time_data[index] = HAL_GetTick() - t0;
		++index;

		int lc_int = (int)(weight * 1000) / 1000;
		char lcd_out[16];
		snprintf(lcd_out, 16, "Weight: %.fg", lc_data[index - 1]);
	}


  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  tick_I2C_rx_driver( & driver);
	  if (poll_I2C_driver( & driver)) {
	    memcpy(data, get_I2C_driver( & driver), 48);
	    reset_I2C_driver( & driver);

	    uint16_t data1 = ham_dec(int16_from_bool(data));
	    uint16_t data2 = ham_dec(int16_from_bool(data + 16));
	    uint16_t data3 = ham_dec(int16_from_bool(data + 32));

	    if (data1 == (uint16_t)-1 || data2 == (uint16_t)-1 ||
	      data3 == (uint16_t)-1)
	      reading = -1;
	    else
	      reading = data1 << 22 | data2 << 11 | data3;

	    float weight = reading * lc_const;

	    if (abs(weight - empty) < 20 || weight < empty)
	    	continue;

	    if (!first && abs(weight - lc_data[index - 1]) < 20)
	    	continue;

	    first = true;


	      // ling reg data
	      uint32_t lc_reading = weight - empty;
	      uint32_t time = HAL_GetTick() - time_0;
	      ++index;
	      // realloc lc_data to be 1 longer
	      lc_data = (float * ) realloc(lc_data, (size) * sizeof(float));
	      lc_data[size - 1] = lc_reading;
	      // realloc time_data to be 1 longer
	      time_data = (uint32_t * ) realloc(time_data, (size) * sizeof(uint32_t));
	      time_data[size - 1] = time;

	      int lc_data = (int)((load_cell_reading / lc_const) * 1000);
	      char ln2_buffer[16];
	      sprintf(ln2_buffer, "%s%d%s", "Cur. mass: ", lc_data, "g");
	      lcd_str(&lcd, ln2_buffer, "so help us god.");
	  }
	  bool is_btn_press = !HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);
	  if (btn_was_pressed && ((HAL_GetTick() - btn_press_start_time) > 3000)) {
	    // zero load cell data
	    empty = weight;
	    time_0 = HAL_GetTick();
	    btn_press_start_time = HAL_GetTick();
	    for (int i = 0; i < 5; ++i) {
	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
	    	HAL_Delay(100);
	    	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	    	HAL_Delay(100);
	    }
	    __NOP();
	  }
	  if (is_btn_press) {
	    if (!btn_was_pressed)
	      btn_press_start_time = HAL_GetTick();
	    btn_was_pressed = 1;
	  } else {
	    btn_was_pressed = 0;
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 41999;
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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, D1_Pin|BROKEN_Pin|LD2_Pin|GPIO_PIN_6
                          |D2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, D6_Pin|D3_Pin|D5_Pin|D4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : CLK_Pin DATA__Pin */
  GPIO_InitStruct.Pin = CLK_Pin|DATA__Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : D1_Pin BROKEN_Pin LD2_Pin PA6
                           D2_Pin */
  GPIO_InitStruct.Pin = D1_Pin|BROKEN_Pin|LD2_Pin|GPIO_PIN_6
                          |D2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : D6_Pin D3_Pin D5_Pin D4_Pin */
  GPIO_InitStruct.Pin = D6_Pin|D3_Pin|D5_Pin|D4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
