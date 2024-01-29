/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
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
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[ 256 ];
osStaticThreadDef_t defaultTaskControlBlock;
osThreadId SendTCP_MSG_Handle;
uint32_t ProcessCanMsg_tBuffer[ 512 ];
osStaticThreadDef_t ProcessCanMsg_tControlBlock;
osThreadId ReceiveCAN_MSG_Handle;
uint32_t ReceiveCAN_MSG_Buffer[ 256 ];
osStaticThreadDef_t ReceiveCAN_MSG_ControlBlock;
osThreadId SendCAN_MSG_Handle;
uint32_t SendCAN_MSG_Buffer[ 256 ];
osStaticThreadDef_t SendCAN_MSG_ControlBlock;
osMessageQId queue_can_receiveHandle;
uint8_t queue_can_receiveBuffer[ 20 * sizeof( CanPacket ) ];
osStaticMessageQDef_t queue_can_receiveControlBlock;
osMessageQId queue_can_sendHandle;
uint8_t queue_can_sendBuffer[ 20 * sizeof( CanPacket ) ];
osStaticMessageQDef_t queue_can_sendControlBlock;
osMessageQId queue_tcp_sendHandle;
uint8_t queue_tcp_sendBuffer[ 20 * sizeof( TcpMessage ) ];
osStaticMessageQDef_t queu_tcp_sendControlBlock;
osSemaphoreId SyncSemaphoreHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void SendTCP_MSG(void const * argument);
void ReceiveCAN_MSG(void const * argument);
void SendCAN_MSG(void const * argument);

extern void MX_LWIP_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* Create the semaphores(s) */
  /* definition and creation of SyncSemaphore */
  osSemaphoreDef(SyncSemaphore);
  SyncSemaphoreHandle = osSemaphoreCreate(osSemaphore(SyncSemaphore), 1);

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* definition and creation of queue_can_receive */
  osMessageQStaticDef(queue_can_receive, 20, CanPacket, queue_can_receiveBuffer, &queue_can_receiveControlBlock);
  queue_can_receiveHandle = osMessageCreate(osMessageQ(queue_can_receive), NULL);

  /* definition and creation of queue_can_send */
  osMessageQStaticDef(queue_can_send, 20, CanPacket, queue_can_sendBuffer, &queue_can_sendControlBlock);
  queue_can_sendHandle = osMessageCreate(osMessageQ(queue_can_send), NULL);

  /* definition and creation of queue_tcp_send */
  osMessageQStaticDef(queue_tcp_send, 20, TcpMessage, queue_tcp_sendBuffer, &queu_tcp_sendControlBlock);
  queue_tcp_sendHandle = osMessageCreate(osMessageQ(queue_tcp_send), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256, defaultTaskBuffer, &defaultTaskControlBlock);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of SendTCP_MSG_ */
  osThreadStaticDef(SendTCP_MSG_, SendTCP_MSG, osPriorityAboveNormal, 0, 512, ProcessCanMsg_tBuffer, &ProcessCanMsg_tControlBlock);
  SendTCP_MSG_Handle = osThreadCreate(osThread(SendTCP_MSG_), NULL);

  /* definition and creation of ReceiveCAN_MSG_ */
  osThreadStaticDef(ReceiveCAN_MSG_, ReceiveCAN_MSG, osPriorityNormal, 0, 256, ReceiveCAN_MSG_Buffer, &ReceiveCAN_MSG_ControlBlock);
  ReceiveCAN_MSG_Handle = osThreadCreate(osThread(ReceiveCAN_MSG_), NULL);

  /* definition and creation of SendCAN_MSG_ */
  osThreadStaticDef(SendCAN_MSG_, SendCAN_MSG, osPriorityBelowNormal, 0, 256, SendCAN_MSG_Buffer, &SendCAN_MSG_ControlBlock);
  SendCAN_MSG_Handle = osThreadCreate(osThread(SendCAN_MSG_), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
__weak void StartDefaultTask(void const * argument)
{
  /* init code for LWIP */
  MX_LWIP_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_SendTCP_MSG */
/**
* @brief Function implementing the SendTCP_MSG_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SendTCP_MSG */
__weak void SendTCP_MSG(void const * argument)
{
  /* USER CODE BEGIN SendTCP_MSG */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END SendTCP_MSG */
}

/* USER CODE BEGIN Header_ReceiveCAN_MSG */
/**
* @brief Function implementing the ReceiveCAN_MSG_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReceiveCAN_MSG */
__weak void ReceiveCAN_MSG(void const * argument)
{
  /* USER CODE BEGIN ReceiveCAN_MSG */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END ReceiveCAN_MSG */
}

/* USER CODE BEGIN Header_SendCAN_MSG */
/**
* @brief Function implementing the SendCAN_MSG_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_SendCAN_MSG */
__weak void SendCAN_MSG(void const * argument)
{
  /* USER CODE BEGIN SendCAN_MSG */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END SendCAN_MSG */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

