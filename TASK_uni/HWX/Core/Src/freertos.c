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
#include "dji_pid.h"
#include "remote.h"
#include "my_chassis.h"
#include "car.h"
#include "my_gimbal.h"
#include "bmi.h"
#include "rp_math.h"
#include "my_shoot.h"
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern motor_6020_t motor_6020__structure;

extern pid_t        chassis_pid_speed_structure[4];

extern pid_t        chassis_pid_position_structure;
extern rc_t         rc_structure;
extern chassis_t              chassis_structure;


extern motor_3508_t           motor_3508_LF_structure;
extern motor_3508_t           motor_3508_RF_structure;
extern motor_3508_t           motor_3508_LB_structure;
extern motor_3508_t           motor_3508_RB_structure;


extern car_t                  car_structure;


extern gimbal_t               gimbal_structure;


extern pid_t                  pit_pid_speed_structure;
extern pid_t                  yaw_pid_speed_structure;

extern bmi_t                  bmi_structure;


extern motor_6020_t           motor_6020_YAW_structure;
extern motor_6020_t           motor_6020_PIT_structure;

extern shoot_t                shoot_structure;

extern int16_t                shoot_offline_cnt;

#if TASK7
extern info_pack_t  TASK_info_pack;
#endif

extern short gyroy,gyroz;

extern HAL_StatusTypeDef MY_CAN_Sent_Data( uint16_t data_1,uint16_t data_2,uint16_t data_3,uint16_t data_4);
extern CAN_RxFrameTypeDef hcan1RxFrame;

extern CAN_HandleTypeDef hcan1;

int16_t tem_posi=0;
uint16_t cnt = 0;
uint8_t rc_connect = 0 , rc_offline = 0;


/* ���������� */
static TaskHandle_t KAL_Handle = NULL;
/* �����ؾ�� */
static TaskHandle_t MONITOR_MOTOR_Handle = NULL;
/* PID������ */
static TaskHandle_t PID_Handle = NULL;
/* ��̨������ */
static TaskHandle_t GIM_Handle = NULL;
/* �����Ǽ����� */
static TaskHandle_t BMI_Handle = NULL;
/* ����״̬��� */
static TaskHandle_t CAR_MODE_Handle = NULL;


CAN_RxHeaderTypeDef	CAN_RX_MSG;


static void KAL_Task(void* pvParameters);
static void MONITOR_MOTOR_Task(void* pvParameters);
static void PID_Task(void* pvParameters);
static void GIM_Task(void* pvParameters);
static void BMI_Task(void* pvParameters);
static void CAR_MODE_Task(void* pvParameters);

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
	
	
		xTaskCreate((TaskFunction_t )MONITOR_MOTOR_Task,  /* ������ں��� */
										(const char*    )"MONITOR_MOTOR_Task",/* �������� */
										(uint16_t       )512,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )8, /* ��������ȼ� */
										(TaskHandle_t*  )&MONITOR_MOTOR_Handle);/* ������ƿ�ָ�� */ 

		xTaskCreate((TaskFunction_t )KAL_Task,  /* ������ں��� */
										(const char*    )"KAL_Task",/* �������� */
										(uint16_t       )512,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )7, /* ��������ȼ� */
										(TaskHandle_t*  )&KAL_Handle);/* ������ƿ�ָ�� */ 
										
		xTaskCreate((TaskFunction_t )PID_Task,  /* ������ں��� */
										(const char*    )"PID_Task",/* �������� */
										(uint16_t       )512,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )6, /* ��������ȼ� */
										(TaskHandle_t*  )&PID_Handle);/* ������ƿ�ָ�� */ 
						
	 	xTaskCreate((TaskFunction_t )GIM_Task,  /* ������ں��� */
										(const char*    )"GIM_Task",/* �������� */
										(uint16_t       )1024,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )5, /* ��������ȼ� */
										(TaskHandle_t*  )&GIM_Handle);/* ������ƿ�ָ�� */ 
										
										
		xTaskCreate((TaskFunction_t )BMI_Task,  /* ������ں��� */
										(const char*    )"BMI_Task",/* �������� */
										(uint16_t       )1024,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )3, /* ��������ȼ� */
										(TaskHandle_t*  )&BMI_Handle);/* ������ƿ�ָ�� */ 		

										
		xTaskCreate((TaskFunction_t )CAR_MODE_Task,  /* ������ں��� */
										(const char*    )"CAR_MODE_Task",/* �������� */
										(uint16_t       )1024,  /* ����ջ��С */
										(void*          )NULL,/* ������ں������� */
										(UBaseType_t    )4, /* ��������ȼ� */
										(TaskHandle_t*  )&CAR_MODE_Handle);/* ������ƿ�ָ�� */ 										
										
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


/*ң�������ӳ���״̬*/
static void CAR_MODE_Task(void* pvParameters)
{
	while(1)
	{
		//MODE_CHECK();
		
		osDelay(2);
	}
}


/*��������������*/
static void BMI_Task(void* pvParameters)
{
		while(1)
		{
			/*��ȡ����������*/
			
			
			osDelay(1);
		}
}



/*���͵��ָ��*/
static void KAL_Task(void* pvParameters)
{
		while(1)
			{
				
				//chassis_ctrl_task(&chassis_structure);
				//�ݾ�����
				MODE_CHECK();
				shoot_task();			
				osDelay(1);

			}

	}


static void GIM_Task(void* pvParameters)
{
	while(1)
	{
		
			//��ʱ����
			my_BMI_Get_EulerAngle(&bmi_structure.pit_angle, &bmi_structure.yaw_angle,NULL ,NULL);
			bmi_structure.yaw_gro = Low_Pass_Fliter(gyroz,bmi_structure.yaw_gro,0.2);
			bmi_structure.pit_gro = Low_Pass_Fliter(gyroy,bmi_structure.pit_gro,0.2);
		
		
		
		 //gimbal_ctrl_task( & gimbal_structure);
		
		 //MOTOR_6020_CAN_SENT_DATA(motor_6020_YAW_structure.output_current,\
														  motor_6020_PIT_structure.output_current,\
														  0,0);//ע��˳�� 
		osDelay(1);
	}
}
	
	
 /*���߼��*/
static void MONITOR_MOTOR_Task(void* pvParameters)
{
	 while(1)
		{		
		/*������߼��*/
			
			  /*LF 2  /1*/
				if(motor_3508_LF_structure.info->offline_cnt>=motor_3508_LF_structure.info->offline_cnt_max)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_BLUE_Pin, GPIO_PIN_RESET);
					motor_3508_LF_structure.info->status=_DEV_OFFLINE;
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_BLUE_Pin, GPIO_PIN_SET);
					motor_3508_LF_structure.info->status=_DEV_ONLINE;
					motor_3508_LF_structure.info->offline_cnt++;
					
				}
				/*RF 1  /2*/
				if(motor_3508_RF_structure.info->offline_cnt>=motor_3508_RF_structure.info->offline_cnt_max)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_BLUE_Pin, GPIO_PIN_RESET);
					motor_3508_RF_structure.info->status=_DEV_OFFLINE;
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_BLUE_Pin, GPIO_PIN_SET);
					motor_3508_RF_structure.info->status=_DEV_ONLINE;
					motor_3508_RF_structure.info->offline_cnt++;
					
				}
				/*LB 3 */
				if(motor_3508_LB_structure.info->offline_cnt>=motor_3508_LB_structure.info->offline_cnt_max)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_BLUE_Pin, GPIO_PIN_RESET);
					motor_3508_LB_structure.info->status=_DEV_OFFLINE;
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_BLUE_Pin, GPIO_PIN_SET);
					motor_3508_LB_structure.info->status=_DEV_ONLINE;
					motor_3508_LB_structure.info->offline_cnt++;
					
				}
				/*RB 4 */
				if(motor_3508_RB_structure.info->offline_cnt>=motor_3508_RB_structure.info->offline_cnt_max)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_BLUE_Pin, GPIO_PIN_RESET);
					motor_3508_RB_structure.info->status=_DEV_OFFLINE;
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_BLUE_Pin, GPIO_PIN_SET);
					motor_3508_RB_structure.info->status=_DEV_ONLINE;
					motor_3508_RB_structure.info->offline_cnt++;
					
				}
				
				
			 /*ң�������߼��*/ // �ڻص�����������
				if(rc_structure.info->offline_cnt >= REMOTE_OFFLINE_CNT_MAX)
				{
					
					HAL_GPIO_WritePin(GPIOC,LED_ORANGE_Pin, GPIO_PIN_RESET);
					rc_structure.base_info->s1.value = 0;
					rc_structure.info->status=_DEV_OFFLINE;
					rc_offline =1 ;
					
				}else
				{
					HAL_GPIO_WritePin(GPIOC, LED_ORANGE_Pin, GPIO_PIN_SET);
					rc_structure.info->offline_cnt++;
					shoot_offline_cnt = 0;
					rc_connect = 1;				
				}
				
				if(rc_connect == 1 && rc_offline == 1)//ң������������
				{
					rc_offline = 0;
				  gimbal_init ( &gimbal_structure);
					chassis_init( &chassis_structure);
				}
								
			osDelay(2);//�����ʱ 
		}
}


/*����PID*/
static void PID_Task(void* pvParameters)
{
	
	while(1)
	{
		if(motor_3508_LF_structure.info->status==_DEV_ONLINE)//����״̬�²�����
					{
							motor_3508_LF_structure.output_current = pid_calc(&chassis_pid_speed_structure[0],motor_3508_LF_structure.base_info->speed,motor_3508_LF_structure.base_info->target_prm);//set

					}else
					{
						chassis_pid_position_structure.iout = 0;//��������
						chassis_pid_position_structure.iout = 0;
					}
		if(motor_3508_RF_structure.info->status==_DEV_ONLINE)//����״̬�²�����
					{
							motor_3508_RF_structure.output_current = pid_calc(&chassis_pid_speed_structure[1],motor_3508_RF_structure.base_info->speed,motor_3508_RF_structure.base_info->target_prm);//set

					}else
					{
						chassis_pid_position_structure.iout = 0;//��������
						chassis_pid_position_structure.iout = 0;
					}
		if(motor_3508_LB_structure.info->status==_DEV_ONLINE)//����״̬�²�����
					{
							motor_3508_LB_structure.output_current = pid_calc(&chassis_pid_speed_structure[2],motor_3508_LB_structure.base_info->speed,motor_3508_LB_structure.base_info->target_prm);//set

					}else
					{
						chassis_pid_position_structure.iout = 0;//��������
						chassis_pid_position_structure.iout = 0;
					}
		if(motor_3508_RB_structure.info->status==_DEV_ONLINE)//����״̬�²�����
					{
							motor_3508_RB_structure.output_current = pid_calc(&chassis_pid_speed_structure[3],motor_3508_RB_structure.base_info->speed,motor_3508_RB_structure.base_info->target_prm);//set

					}else
					{
						chassis_pid_position_structure.iout = 0;//��������
						chassis_pid_position_structure.iout = 0;
					}
		
					
					
					
					
		
		osDelay(1);
	}
}
	
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/