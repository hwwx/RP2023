/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
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
#include "3508_motor.h"
#include "6020_motor.h"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern motor_3508_t motor_3508__structure;
extern motor_6020_t motor_6020__structure;
#if TASK7
extern info_pack_t  TASK_info_pack;
#endif
extern HAL_StatusTypeDef MY_CAN_Sent_Data( uint16_t data_1,uint16_t data_2,uint16_t data_3,uint16_t data_4);
extern CAN_RxFrameTypeDef hcan1RxFrame;

extern CAN_HandleTypeDef hcan1;



uint16_t speed,current,angle;
/* 电机操作句柄 */
static TaskHandle_t KAL_Handle = NULL;
/* 电机监控句柄 */
static TaskHandle_t MONITOR_MOTOR_Handle = NULL;

CAN_RxHeaderTypeDef	CAN_RX_MSG;


static void KAL_Task(void* pvParameters);
static void MONITOR_MOTOR_Task(void* pvParameters);

/* USER CODE END Variables */
osThreadId MonitorTaskHandle;
osThreadId ControlTaskHandle;
osThreadId SystemTaskHandle;
osThreadId SendTaskHandle;



/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartMonitorTask(void const * argument);
extern void StartControlTask(void const * argument);
extern void StartSystemTask(void const * argument);
extern void StartSendTask(void const * argument);

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
  /* definition and creation of MonitorTask */
  osThreadDef(MonitorTask, StartMonitorTask, osPriorityNormal, 0, 256);
  MonitorTaskHandle = osThreadCreate(osThread(MonitorTask), NULL);

  /* definition and creation of ControlTask */
  osThreadDef(ControlTask, StartControlTask, osPriorityNormal, 0, 128);
  ControlTaskHandle = osThreadCreate(osThread(ControlTask), NULL);

  /* definition and creation of SystemTask */
  osThreadDef(SystemTask, StartSystemTask, osPriorityNormal, 0, 128);
  SystemTaskHandle = osThreadCreate(osThread(SystemTask), NULL);

  /* definition and creation of SendTask */
  osThreadDef(SendTask, StartSendTask, osPriorityBelowNormal, 0, 256);
  SendTaskHandle = osThreadCreate(osThread(SendTask), NULL);
	
	
		xTaskCreate((TaskFunction_t )MONITOR_MOTOR_Task,  /* 任务入口函数 */
										(const char*    )"MONITOR_MOTOR_Task",/* 任务名字 */
										(uint16_t       )512,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )2, /* 任务的优先级 */
										(TaskHandle_t*  )&MONITOR_MOTOR_Handle);/* 任务控制块指针 */ 
		xTaskCreate((TaskFunction_t )KAL_Task,  /* 任务入口函数 */
										(const char*    )"KAL_Task",/* 任务名字 */
										(uint16_t       )512,  /* 任务栈大小 */
										(void*          )NULL,/* 任务入口函数参数 */
										(UBaseType_t    )10, /* 任务的优先级 */
										(TaskHandle_t*  )&KAL_Handle);/* 任务控制块指针 */ 
										
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartMonitorTask */
/**
  * @brief  Function implementing the MonitorTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartMonitorTask */
__weak void StartMonitorTask(void const * argument)
{
  /* USER CODE BEGIN StartMonitorTask */
  /* Infinite loop */
  for(;;)
  {


  }
	
  /* USER CODE END StartMonitorTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
static void KAL_Task(void* pvParameters)
{
		while(1)
			{
				

				motor_6020__structure.output_current=3000;
			
				MOTOR_6020_CAN_SENT_DATA(motor_6020__structure.output_current,0,0,0);//发送数据
				
				
				motor_3508__structure.output_current=500;
				
				MOTOR_3508_CAN_SENT_DATA(motor_3508__structure.output_current,0,0,0);//发送数据
				
				/*调试参数*/
				angle=motor_6020__structure.base_info->angle;
				current=motor_6020__structure.base_info->current;
				speed=motor_3508__structure.base_info->speed;
				
				
				#if TASK7
				/*TASK7*/
				MY_CAN_SENT_DATA(&TASK_info_pack);
				#endif
				
				osDelay(2);

			}

	}

	
	
static void MONITOR_MOTOR_Task(void* pvParameters)
{
	 while(1)
		{	
			if(motor_6020__structure.info->offline_cnt>=motor_6020__structure.info->offline_cnt_max)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_RESET);//
					motor_6020__structure.info->status=DEV_OFFLINE;
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_GREEN_Pin|LED_RED_Pin, GPIO_PIN_SET);//等全灭
					motor_6020__structure.info->status=DEV_ONLINE;
					motor_6020__structure.info->offline_cnt++;
					
				}
				
				if(motor_3508__structure.info->offline_cnt>=motor_3508__structure.info->offline_cnt_max)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_BLUE_Pin|LED_ORANGE_Pin, GPIO_PIN_RESET);//
					motor_3508__structure.info->status=DEV_OFFLINE;
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_BLUE_Pin|LED_ORANGE_Pin, GPIO_PIN_SET);//等全灭
					motor_3508__structure.info->status=DEV_ONLINE;
					motor_3508__structure.info->offline_cnt++;
					
				}
					
					

			//vTaskDelayUntil(2);绝对延时
			osDelay(2);//相对延时 
		}
}
	
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
