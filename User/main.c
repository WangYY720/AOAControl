#include "stm32f10x.h"
#include "bsp_adc.h"
#include "pwm.h"
#include "capture.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"

extern __IO uint16_t ADC_ConvertedValue1;
extern __IO uint16_t ADC_ConvertedValue2;
__IO uint16_t ADC_ConvertedValue;
// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal; 
u32 CaptureValue;

#define START_TASK_PRIO		1
#define START_STK_SIZE		128
TaskHandle_t	StartTask_Handle;
void start_task(void *pvParameters);

#define TASK1_TASK_PRIO		2
#define TASK1_STK_SIZE		128
TaskHandle_t	Task1Task_Handle;
void task1_task(void *pvParameters);

#define TASK2_TASK_PRIO		3
#define TASK2_STK_SIZE		128
TaskHandle_t	Task2Task_Handle;
void task2_task(void *pvParameters); 

#define TASK3_TASK_PRIO		2
#define TASK3_STK_SIZE		128
TaskHandle_t	Task3Task_Handle;
void task3_task(void *pvParameters);

void start_task(void *pvParameters)
{
	taskENTER_CRITICAL();
	
	xTaskCreate((TaskFunction_t	)task1_task,
							(const char*		)"task1_task",
							(u16						)TASK1_STK_SIZE,
							(void*					)NULL,
							(UBaseType_t		)TASK1_TASK_PRIO,
							(TaskHandle_t		)&Task1Task_Handle);
							
	xTaskCreate((TaskFunction_t	)task2_task,
							(const char*		)"task2_task",
							(u16						)TASK2_STK_SIZE,
							(void*					)NULL,
							(UBaseType_t		)TASK2_TASK_PRIO,
							(TaskHandle_t		)&Task2Task_Handle);
							
	xTaskCreate((TaskFunction_t	)task3_task,
							(const char*		)"task3_task",
							(u16						)TASK3_STK_SIZE,
							(void*					)NULL,
							(UBaseType_t		)TASK3_TASK_PRIO,
							(TaskHandle_t		)&Task3Task_Handle);
							
	vTaskDelete(StartTask_Handle);
	taskEXIT_CRITICAL();
							
}
void task1_task(void *pvparameters)
{
	while(1){
		GPIO_SetBits(GPIOA,GPIO_Pin_8);
		
		ADC_ConvertedValue = (ADC_ConvertedValue1 > ADC_ConvertedValue2) ? ADC_ConvertedValue2 : ADC_ConvertedValue1;
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096.0*3.3; 

//		TIM_SetCompare1(PWM_TIMx,ADC_ConvertedValueLocal/3.3*6000+1500);

	}
}
void task2_task(void *pvparameters)
{
	while(1){
		
//		printf("\r\n The current AD value = %f V \r\n",ADC_ConvertedValueLocal); 
		printf("\r\n Capture value = %d\r\n",CaptureValue);
		vTaskDelay(500);
		
	}
}
void task3_task(void *pvParameters)
{
	while(1){
		UpdateCapture( &CaptureValue );
		TIM_SetCompare1(PWM_TIMx,CaptureValue);
		vTaskDelay(5);
	}
}

int main(void)
{	
	//系统初始化
	SystemInit();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	// 配置串口
	USART1_Init(9600);
	
	// ADC 初始化
	ADCx_Init();
	
	// PWM 初始化
	TIM_PWM_Init();
	
	//输入捕获初始化
	Time_Cap_Init();
	
	xTaskCreate((TaskFunction_t	)start_task,
							(const char*		)"start_task",
							(u16						)START_STK_SIZE,
							(void*					)NULL,
							(UBaseType_t		)START_TASK_PRIO,
							(TaskHandle_t		)&StartTask_Handle);
	vTaskStartScheduler();
}
/*********************************************END OF FILE**********************/
