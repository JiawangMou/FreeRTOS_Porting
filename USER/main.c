#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"

/*下面主函数是移植FreeRTOS系统实现控制LED控制和键盘扫描，并且串口输出*/

#define START_TASK_PRIO 1 //任务优先 级
#define START_STK_SIZE 128 //任务堆栈大小
TaskHandle_t Start_Task_Handler; //任务句柄
void start_task(void *pvParameters); //任务函数

#define LED_TASK_PRIO 3 //任务优先 级
#define LED_STK_SIZE 128 //任务堆栈大小
TaskHandle_t Led_Task_Handler; //任务句柄
void led_task(void *pvParameters); //任务函数

#define KEYBOARD_TASK_PRIO 2 //任务优先级
#define KEYBOARD_STK_SIZE 128 //任务堆栈大小
TaskHandle_t Keyboard_Task_Handler; //任务句柄
void keyboard_task(void *pvParameters); //任务函数
 
#define UART_TASK_PRIO 2 //任务优先级
#define UART_STK_SIZE 128 //任务堆栈大小
TaskHandle_t Uart_Task_Handler; //任务句柄
void uart_task(void *pvParameters); //任务函数

int main(void)
{
    HAL_Init();                     //初始化HAL库   
    Stm32_Clock_Init(360,25,2,8);   //设置时钟,180Mhz
    delay_init(180);                //初始化延时函数
    LED_Init();                     //初始化LED  
	uart_init(115200); 				//初始化串口
		//创建开始任务
	xTaskCreate((TaskFunction_t )start_task, 			//任务函数
				(const char*    )"start_task", 			//任务名称
				(uint16_t 		)START_STK_SIZE,  		//任务堆栈大小
				(void* 			)NULL, 					//传递给任务函数的参数
				(UBaseType_t 	)START_TASK_PRIO, 		//任务优先级
				(TaskHandle_t* 	)&Start_Task_Handler); 	//任务句柄
								
	vTaskStartScheduler(); //开启任务调度
}

void start_task(void *pvParameters)//任务函数
{
	taskENTER_CRITICAL(); //进入临界区
	xTaskCreate((TaskFunction_t )led_task, 				//任务函数
				(const char*    )"led_task", 			//任务名称
				(uint16_t 		)LED_STK_SIZE,  		//任务堆栈大小
				(void* 			)NULL, 					//传递给任务函数的参数
				(UBaseType_t 	)LED_TASK_PRIO, 		//任务优先级
				(TaskHandle_t* 	)&Led_Task_Handler); 	//任务句柄
				
	xTaskCreate((TaskFunction_t )keyboard_task, 			//任务函数
				(const char*    )"keyboard_task", 			//任务名称
				(uint16_t 		)KEYBOARD_STK_SIZE,  		//任务堆栈大小
				(void* 			)NULL, 						//传递给任务函数的参数
				(UBaseType_t 	)KEYBOARD_TASK_PRIO, 		//任务优先级
				(TaskHandle_t* 	)&Keyboard_Task_Handler); 	//任务句柄
				
	xTaskCreate((TaskFunction_t )uart_task, 			//任务函数
				(const char*    )"uart_task", 			//任务名称
				(uint16_t 		)UART_STK_SIZE,  		//任务堆栈大小
				(void* 			)NULL, 					//传递给任务函数的参数
				(UBaseType_t 	)UART_TASK_PRIO, 		//任务优先级
				(TaskHandle_t* 	)&Uart_Task_Handler); 	//任务句柄		
	vTaskDelete(Start_Task_Handler); 					//删除开始任务
	taskEXIT_CRITICAL(); //退出临界区
}

 
void led_task(void *pvParameters)
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;
	xLastWakeTime = xTaskGetTickCount ();
	while(1)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
	}
	
}
 
void uart_task(void *pvParameters)
{
	TickType_t Time;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 300;
	xLastWakeTime = xTaskGetTickCount ();
	while(1)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		Time = xTaskGetTickCount();
		printf("系统时钟 = %d\r\n", (uint32_t)Time);

	}
}

void keyboard_task(void *pvParameters)
{
	const TickType_t xDelay = 500 / portTICK_PERIOD_MS;
	while(1)
	{
		printf("键盘任务执行\r\n");
		vTaskDelay( xDelay);
	}
}


