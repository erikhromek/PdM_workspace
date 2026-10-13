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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "tm1637.h"
#include "dht11.h"
#include "delay.h"
#include "app_button.h"
#include "alarm.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/**
 * @brief Program possible states
 * On configure mode, UP and DOWN buttons can be used and reading.
 * On display mode, DHT11 gets polled every 3 seconds and the value is written
 * on the display.
 * Alarm can the toggled in any state.
 *
 */
typedef enum {
	DISPLAY_MODE, CONFIGURE_MODE
} programState_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

static dht11Data_t dht11Data;
static delay_t dht11Delay;
static const tick_t DHT11_DELAY = 3000;
static programState_t programState;
static btnEvent_t btnEvent;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	HAL_Delay(1000);

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	/* USER CODE BEGIN 2 */

	/**
	 * Current program config:
	 *
	 * DHT11: PB6 / D10
	 *
	 * TM1637:
	 * CLK: PA6 / D12
	 * DIO: PA9 / D8
	 *
	 * Buzzer: PA7 / D11
	 *
	 * Buttons:
	 * UP: PB10 / D6
	 * DOWN: PB5 / D4
	 * ALARM: PA10 / D2
	 * CONFIG: PA8 / D7
	 */

	/**
	 * 1) Initialize GPIO for buttons
	 * 2) Initialize and configure TM1637
	 * 3) Initialize and configure DHT11
	 * 4) Configure alarm
	 * 5) Start superloop
	 * 6) check buttons state
	 */

	dht11Init(GPIOB, GPIO_PIN_6);
	tm1637Init(GPIOA, GPIO_PIN_6, GPIOA, GPIO_PIN_9);
	tm1637SetBrightness(7);
	programState = DISPLAY_MODE;

	delayInit(&dht11Delay, DHT11_DELAY);

	appButtonsInit(GPIOB, GPIO_PIN_10, GPIOB, GPIO_PIN_5, GPIOA, GPIO_PIN_10,
	GPIOA, GPIO_PIN_8);
	alarmInit(GPIOA, GPIO_PIN_7);
	delayRead(&dht11Delay); // Trigger the first read

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		switch (programState) {
		case DISPLAY_MODE:
			// If delay of 3 seconds finished, read DHT11
			if (delayRead(&dht11Delay)) {
				bool completed = dht11Read();
				if (completed) {
					dht11LatestRead(&dht11Data);
					tm1637WriteNumber(dht11Data.temperature);
				}
			}
			break;
		case CONFIGURE_MODE:
			/*
			 * Current alarm threshold gets set when entering the state and it
			 * gets updated when UP and DOWN buttons so it is not needed here.
			 */
			break;
		default:
			programState = DISPLAY_MODE;
			break;
		}

		buttonsStateUpdate();
		alarmUpdate(dht11Data.temperature);
		if (buttonPressed()) {
			btnEvent = getEventButton();
			switch (btnEvent) {
			case BTN_ALARM:
				// Alarm button only toggles alarm ON/OFF
				alarmToggle();
				break;
			case BTN_CONFIG:
				// Config button swaps program state
				switch (programState) {
				case DISPLAY_MODE:
					programState = CONFIGURE_MODE;
					tm1637WriteNumber(getAlarmThreshold());
					break;
				case CONFIGURE_MODE:
					programState = DISPLAY_MODE;
					tm1637WriteNumber(dht11Data.temperature);
					break;
				default:
					break;
				}
				break;
			case BTN_UP:
				if (programState == CONFIGURE_MODE) {
					// Increment alarm threshold if configure mode
					incrementAlarmThreshold();
					tm1637WriteNumber(getAlarmThreshold());
				}
				break;
			case BTN_DOWN:
				if (programState == CONFIGURE_MODE) {
					// Decrement alarm threshold if configure mode
					decrementAlarmThreshold();
					tm1637WriteNumber(getAlarmThreshold());
				}
				break;
			default:
				break;
			}
		}

		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : USART_TX_Pin USART_RX_Pin */
	GPIO_InitStruct.Pin = USART_TX_Pin | USART_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : LD2_Pin */
	GPIO_InitStruct.Pin = LD2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
