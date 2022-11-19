/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

extern void MX_USB_DEVICE_Init(void);
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

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
	osThreadDef(GetLcd, StartGetLcdTask, osPriorityHigh, 0, 512);
	GetLcdTaskHandle = osThreadCreate(osThread(GetLcd), NULL);					/*	1. 获取HMI指令	*/
	
	osThreadDef(Mode, StartModeTask, osPriorityAboveNormal, 0, 1024);
	ModeHandle  = osThreadCreate(osThread(Mode), NULL);							/*	2. 麻醉界面	*/
	
	osThreadDef(Settings, StartSettingTask, osPriorityNormal, 0, 256);
	SettingHandle  = osThreadCreate(osThread(Settings), NULL);					/*	3. 设置界面	*/
	
	osThreadDef(FactoryCalib, StartFactoryCalibTask, osPriorityNormal, 0, 256);
	FactoryCalibHandle  = osThreadCreate(osThread(FactoryCalib), NULL);			/*	4. 校正界面	*/
	
	osThreadDef(FactoryAging, StartFactoryAgingTask, osPriorityNormal, 0, 384);
	FactoryAgingHandle  = osThreadCreate(osThread(FactoryAging), NULL);			/*	5. 老化界面	*/
	
	osThreadDef(MotorController, StartMotorTask, osPriorityAboveNormal, 0, 256);
	MotorHandle = osThreadCreate(osThread(MotorController), NULL);				/*	6. 电机控制	*/
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
  /* init code for USB_DEVICE */
//  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
	for(;;){
		static uint8_t cnt = 0;
		check_alarm_event();
		power_on_off_detect();				/*	开关机及充电检测	*/
		osDelay(150);
		if(cnt == 14){
			get_now_page();
			cnt = 0;
		}else
			cnt ++;
	}
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
/*	Task:  获取屏幕指令		*/
void StartGetLcdTask(void const * argument)
{	
	for(;;){
		Analysis_LcdCmd();					/*	分析接收指令	*/
		check_sendBuf_and_sendBuf();		/*	检查发送缓冲区是否有指令等待发送	*/
		osDelay(50);
	}
}

/*	麻醉	*/
void StartModeTask(void const * argument)
{
	for(;;){
		page_mode();
		osDelay(100);
	}
}

/*	设置	*/
void StartSettingTask(void const * argument)
{
	for(;;){
		page_setting();
		osDelay(100);
	}
}

/*	校正	*/
void StartFactoryCalibTask(void const * argument)
{
	for(;;){
		page_calibration();
		osDelay(100);
	}
}
/*	老化	*/
void StartFactoryAgingTask(void const * argument)
{
	for(;;){
		page_aging();
		osDelay(100);
	}
}

/*	电机控制		*/
void StartMotorTask(void const * argument)
{
	for(;;){
		motor_controller();
		osDelay(100);
	}
}
/* USER CODE END Application */
