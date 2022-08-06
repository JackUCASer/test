/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "app_main.h"
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
osThreadId GetLcdTaskHandle;
osThreadId ModeHandle;
osThreadId SettingHandle;
osThreadId FactoryCalibHandle;
osThreadId FactoryAgingHandle;
osThreadId MotorHandle;
//osThreadId AlarmHandle;
/* USER CODE END Variables */

osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartGetLcdTask(void const * argument);
void StartMotorTask(void const * argument);
void StartSettingTask(void const * argument);
void StartFactoryCalibTask(void const * argument);
void StartFactoryAgingTask(void const * argument);
void StartModeTask(void const * argument);
//void StartAlarmTask(void const * argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

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

  /* Create the timer(s) */

  /* USER CODE BEGIN RTOS_TIMERS */

  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(GetLcd, StartGetLcdTask, osPriorityHigh, 0, 512);
	GetLcdTaskHandle = osThreadCreate(osThread(GetLcd), NULL);					/*	1. ��ȡHMIָ��	*/
	
	osThreadDef(Mode, StartModeTask, osPriorityAboveNormal, 0, 512);
	ModeHandle  = osThreadCreate(osThread(Mode), NULL);							/*	2. �������	*/
	
	osThreadDef(Settings, StartSettingTask, osPriorityNormal, 0, 256);
	SettingHandle  = osThreadCreate(osThread(Settings), NULL);					/*	3. ���ý���	*/
	
	osThreadDef(FactoryCalib, StartFactoryCalibTask, osPriorityNormal, 0, 256);
	FactoryCalibHandle  = osThreadCreate(osThread(FactoryCalib), NULL);			/*	4. У������	*/
	
	osThreadDef(FactoryAging, StartFactoryAgingTask, osPriorityNormal, 0, 384);
	FactoryAgingHandle  = osThreadCreate(osThread(FactoryAging), NULL);			/*	5. �ϻ�����	*/
	
	osThreadDef(MotorController, StartMotorTask, osPriorityAboveNormal, 0, 256);
	MotorHandle = osThreadCreate(osThread(MotorController), NULL);				/*	6. �������	*/
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
	static uint8_t cnt = 0;
	for(;;){	
		cnt ++;
//		if(cnt > 100){
//			vTaskList((char*)&InfoBuffer);
//			myPrintf("TaskName		TaskState	Priority	LeftStack	TaskNo.\r\n");
//			myPrintf("\r\n%s\r\n",InfoBuffer);
//			vTaskGetRunTimeStats((char*)&InfoBuffer2);
//			myPrintf("TaskName		RunTime		CPU_\r\n");
//			myPrintf("\r\n%s\r\n",InfoBuffer2);
//			cnt = 0;	
//		}
		check_alarm_event();
		power_on_off_detect();				/*	���ػ��������	*/
		osDelay(150);
	}
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*	Task:  ��ȡ��Ļָ��		*/
void StartGetLcdTask(void const * argument)
{	
	for(;;){
		Extract_LcdCmd();
		Analysis_LcdCmd();					/*	��������ָ��	*/
		check_sendBuf_and_sendBuf();
		osDelay(50);
	}
}

/*	����	*/
void StartModeTask(void const * argument)
{
	for(;;){
		page_mode();
		osDelay(100);
	}
}

/*	����	*/
void StartSettingTask(void const * argument)
{
	for(;;){
		page_setting();
		osDelay(100);
	}
}

/*	У��	*/
void StartFactoryCalibTask(void const * argument)
{
	for(;;){
		page_calibration();
		osDelay(100);
	}
}
/*	�ϻ�	*/
void StartFactoryAgingTask(void const * argument)
{
	for(;;){
		page_aging();
		osDelay(100);
	}
}

/*	�������		*/
void StartMotorTask(void const * argument)
{
	for(;;){
		motor_controller();
		osDelay(100);
	}
}


/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
