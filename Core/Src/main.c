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
#include "cmsis_os.h"
#include "can.h"
#include "lwip.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "tcpserver.h"
#include "api.h"
#include "Parse.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "fila.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

bool testcfg = 0;
bool clientOnline = 0;
struct netconn com1 = {0};
extern bool Connected;
uint8_t canMessage = 0;
struct Queue* queue;
extern sys_sem_t tcpsem;
extern osSemaphoreId SyncSemaphoreHandle;
extern osMessageQId queue_tcp_sendHandle;
extern osMessageQId queue_can_sendHandle;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t uartRxBuffer[8] = {0}; //buffer recebido pela serial
uint8_t canRxBuffer [8] = {0}; //buffer recebido pela interface CAN
extern struct tcp_pcb *tpcb;
extern struct tcp_server_struct *es;

extern device devices[];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
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
  MX_CAN1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);


  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  tcpclient_init();
  /* USER CODE BEGIN StartDefaultTask */
  osDelay(2000);
  clientOnline = 1;
  HAL_TIM_Base_Start_IT(&htim3);
  /* Infinite loop */
  for(;;)
  {
//	sprintf (smsgc, "index value = %d\n", indx++);
//	// semaphore must be taken before accessing the tcpsend function
//	sys_arch_sem_wait(&tcpsem, 500);
//	// send the data to the server
//	tcpsend(smsgc);

	osDelay(5000);

  }
  /* USER CODE END StartDefaultTask */
}


void buildMessage(uint8_t MessageType, void *data, uint8_t id, char *result)
{
	switch (MessageType)
	{
		case DATA:
		device *sensorData = (device*)data;
		int writenDATA = sprintf(result, "@#%d#", id);
		writenDATA += sprintf(result + writenDATA, "$%d$", MessageType);
		for (int i = 0; i < MAX_SENSORS; ++i)
		{
			if( (devices[id - 2].activeSensorNumber) & (1<<i))
			{
				writenDATA += sprintf(result + writenDATA, "&%d", sensorData->sensorData[i]);
			}
			else
			{
				writenDATA += sprintf(result + writenDATA, "&null");
			}
		}
		strcat(result, "&!");

		break;


		case SYNC:
			device *state = (device*)data;
			uint8_t deviceCount = 0;
			for (int i = 0; i < MAX_DEVICES; i++)
			{
				if (state[i].deviceSync)
				{
					deviceCount++;
				}
			}
			int writenSYNC = sprintf(result, "@#%d#", id);
			writenSYNC += sprintf(result + writenSYNC, "$%d$", MessageType);
			writenSYNC += sprintf(result + writenSYNC, "&%d", deviceCount);
			for (int i = 0; i < MAX_DEVICES; ++i)
			{
				writenSYNC += sprintf(result + writenSYNC, "&%d", state[i].deviceSync);
			}
			strcat(result, "&!");

			break;

	}
}

void setbit(uint8_t *variable, int bitNumber, int value)
{
    if ((bitNumber < 0) || (bitNumber > 7))
    {
        return;
    }

    if (value == 1)
    {
        *variable |= (1 << bitNumber);
    }
    else if (value == 0)
    {
        *variable &= ~(1 << bitNumber);
    }
    else
    {
        return;
    }
}

void reboot_device(uint8_t id)
{
	CanPacket canPacket = {0};
	canPacket.canID = 0x0;
	canPacket.canDataFields.ctrl0 = REBOOT;
	xQueueSendToBack(queue_can_sendHandle, &canPacket ,0);
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch (GPIO_Pin)
	{
		case USER_Btn_Pin:
			HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);

			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			CanPacket canTesteCfg = {0};
			canTesteCfg.canID = 0;
			canTesteCfg.canDataFields.ctrl0 =  CONFIG;
			if (testcfg)
			{
				canTesteCfg.canDataFields.data[0] = 0x38;
				testcfg = 0;
			}
			else
			{
				canTesteCfg.canDataFields.data[0] = 0xff;
				testcfg = 1;
			}

			xQueueSendToBackFromISR(queue_can_sendHandle, &canTesteCfg, &xHigherPriorityTaskWoken);
	}
}


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  if (htim->Instance == TIM2)
  {
    __NOP();
  }

  if (htim->Instance == TIM3)
  {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	CanPacket canPacket = {0};

	canPacket.canID = BROADCAST;
	canPacket.canDataFields.ctrl0 = SYNC;
//	memset(&devices->deviceSync, 0, sizeof(devices->deviceSync));
//	memset(&devices.activeSensorNumber, 0, sizeof(devices.activeSensorNumber));

	xQueueSendToBackFromISR(queue_can_sendHandle, &canPacket, &xHigherPriorityTaskWoken);

	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	__HAL_TIM_CLEAR_IT(&htim4 ,TIM_IT_UPDATE);
	HAL_TIM_Base_Start_IT(&htim4);
  }

  if (htim->Instance == TIM4)
  {
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	char tcpmsg [64] = {0};
	buildMessage(SYNC, &devices, 0x0, tcpmsg);

    HAL_GPIO_TogglePin(LD3_GPIO_Port, LD3_Pin); //  debug

    __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);
	HAL_TIM_Base_Stop_IT(&htim4);
	//Pegar semaforo

	if (Connected)
		xQueueSendToBackFromISR(queue_tcp_sendHandle, &tcpmsg, &xHigherPriorityTaskWoken);



  }
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
