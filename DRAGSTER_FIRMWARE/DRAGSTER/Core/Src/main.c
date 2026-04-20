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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "hc05_bt.h"
#include "mma8452q.h"
#include "tc1602a_lcd.h"
#include <stdio.h>
#include <string.h>
#include "bldc_esc.h"
#include "ws2812.h"
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
MMA8452Q_Handle_t accel = { .hi2c=&hi2c1, .i2c_addr=MMA8452Q_I2C_ADDR,
                             .range=MMA8452Q_RANGE_8G, .odr=MMA8452Q_ODR_800HZ };
BT_Handle_t bt;
ESC_Handle_t esc;              /* add this */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
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
	/* Force PA6 LOW before ESC boots — same as holding reset on Nucleo */
//	__HAL_RCC_GPIOA_CLK_ENABLE();
//	GPIOA->MODER  &= ~(3U << (6*2));
//	GPIOA->MODER  |=  (1U << (6*2));
//	GPIOA->BSRR    =  (1U << (6+16));
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
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);  /* ← is this line still there? */
  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 199);
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  /* --- Bare minimum LCD init test --- */
//  #define LCD_RS_LOW()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,  GPIO_PIN_RESET)
//  #define LCD_RS_HIGH()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,  GPIO_PIN_SET)
//  #define LCD_E_LOW()    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,  GPIO_PIN_RESET)
//  #define LCD_E_HIGH()   HAL_GPIO_WritePin(GPIOC, GPIO_PIN_6,  GPIO_PIN_SET)
//
//  #define LCD_D4(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
//  #define LCD_D5(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
//  #define LCD_D6(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
//  #define LCD_D7(x) HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, x ? GPIO_PIN_SET : GPIO_PIN_RESET)
//
//  /* Pulse Enable */
//  #define LCD_PULSE() do { LCD_E_HIGH(); HAL_Delay(2); LCD_E_LOW(); HAL_Delay(2); } while(0)
//
//  /* Send nibble (4 bits) */
//  #define LCD_NIBBLE(n) do {
//      LCD_D4(((n) >> 0) & 1);
//      LCD_D5(((n) >> 1) & 1);
//      LCD_D6(((n) >> 2) & 1);
//      LCD_D7(((n) >> 3) & 1);
//      LCD_PULSE();
//  } while(0)
//
//  /* Send full byte as two nibbles */
//  #define LCD_CMD(b)  do { LCD_RS_LOW();  LCD_NIBBLE((b)>>4); LCD_NIBBLE((b)&0xF); HAL_Delay(2); } while(0)
//  #define LCD_DATA(b) do { LCD_RS_HIGH(); LCD_NIBBLE((b)>>4); LCD_NIBBLE((b)&0xF); HAL_Delay(2); } while(0)
//
//  /* Power on delay */
//  HAL_Delay(100);
//  LCD_E_LOW();
//  LCD_RS_LOW();
//
//  /* Init sequence — 3× send 0x3 then switch to 4-bit */
//  LCD_D4(1); LCD_D5(1); LCD_D6(0); LCD_D7(0);
//  LCD_PULSE(); HAL_Delay(10);
//  LCD_PULSE(); HAL_Delay(5);
//  LCD_PULSE(); HAL_Delay(2);
//
//  /* Switch to 4-bit */
//  LCD_D4(0); LCD_D5(1); LCD_D6(0); LCD_D7(0);
//  LCD_PULSE(); HAL_Delay(2);
//
//  /* Function set: 4-bit, 2 lines, 5x8 */
//  LCD_CMD(0x28);
//  /* Display on, cursor off */
//  LCD_CMD(0x0C);
//  /* Clear */
//  LCD_CMD(0x01); HAL_Delay(5);
//  /* Entry mode */
//  LCD_CMD(0x06);
//
//  /* Print "HELLO" */
//  LCD_DATA('H');
//  LCD_DATA('E');
//  LCD_DATA('L');
//  LCD_DATA('L');
//  LCD_DATA('O');

  /* I2C bus scan — finds all responding devices */
//  char dbg[50];
//  HAL_UART_Transmit(&huart1, (uint8_t*)"I2C SCAN:\r\n", 11, 100);
//  for (uint8_t addr = 0x08; addr < 0x78; addr++)
//  {
//      if (HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 1, 10) == HAL_OK)
//      {
//          snprintf(dbg, sizeof(dbg), "  Found device at 0x%02X\r\n", addr);
//          HAL_UART_Transmit(&huart1, (uint8_t*)dbg, strlen(dbg), 100);
//      }
//  }
//  HAL_UART_Transmit(&huart1, (uint8_t*)"SCAN DONE\r\n", 11, 100);
//
//  /* Using HAL_I2C_Mem_Read — handles repeated start automatically */
//  uint8_t who = 0;
//  HAL_StatusTypeDef r = HAL_I2C_Mem_Read(&hi2c1,
//                                           0x1C << 1,  /* device address */
//                                           0x0D,        /* WHO_AM_I register */
//                                           I2C_MEMADD_SIZE_8BIT,
//                                           &who, 1, 100);
//
//  snprintf(dbg, sizeof(dbg), "MEM_READ=%d WHO=0x%02X\r\n", r, who);
//  HAL_UART_Transmit(&huart1, (uint8_t*)dbg, strlen(dbg), 100);
//
//  /* Temporary accelerometer diagnosis */
//  MMA8452Q_Status_t accel_status = MMA8452Q_Init(&accel);
//
//  snprintf(dbg, sizeof(dbg), "ACCEL INIT: %s\r\n",
//           accel_status == MMA8452Q_OK ? "OK" : "FAILED");
//  HAL_UART_Transmit(&huart1, (uint8_t*)dbg, strlen(dbg), 100);
  WS2812_Init();
  WS2812_SetColor(0, 50, 0);  /* green */
  WS2812_Refresh();
  HAL_Delay(10);

  LCD_Splash();
  MMA8452Q_Init(&accel);
  BT_Init(&bt, &huart2, &huart1, &accel);
  HAL_Delay(1000);
  LCD_Clear();
  LCD_ResetSpeed();
  LCD_ShowStatus("Arming ESC...");
  ESC_Init(&esc, &htim3, TIM_CHANNEL_1);
  LCD_ShowStatus("ESC Ready!  ");
  HAL_Delay(1000);
  LCD_ShowStatus("                ");
  /* Raw ESC test — bypass all drivers */
//  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
//
//  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2999);
//  HAL_Delay(4000);
//
//  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 199);
//  HAL_Delay(2000);
//
//  //__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 2999);
//  HAL_Delay(2000);



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  //__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 999);

	  BT_Task(&bt);

	      if (MMA8452Q_IsDataReady(&accel))
	      {
	          float g = 0.0f;
	          if (MMA8452Q_ReadAccelX(&accel, &g) == MMA8452Q_OK)
	          {
	        	  LCD_IntegrateSpeed(g, 1.0f/800.0f);
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
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
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
