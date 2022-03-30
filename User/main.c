#include "stm32f10x.h"
#include "bsp_adc.h"
#include "pwm.h"
#include "capture.h"
#include "usart.h"
#include "delay.h"
#include "sys.h"
#include "FreeRTOS.h"
#include "task.h"
#include "oled.h"
#include "buzzer_led.h"

extern __IO uint16_t ADC_ConvertedValue1;
extern __IO uint16_t ADC_ConvertedValue2;
__IO uint16_t ADC_ConvertedValue;
// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal; 
u16 PWM_Value[4];
u32 CaptureValue[4];
u16 t;
char oled[8]={0};

#define Gain	1

#define START_TASK_PRIO		1
#define START_STK_SIZE		128
TaskHandle_t	StartTask_Handle;
void start_task(void *pvParameters);

#define TASK1_TASK_PRIO		3
#define TASK1_STK_SIZE		128
TaskHandle_t	Task1Task_Handle;
void task1_task(void *pvParameters);

#define TASK2_TASK_PRIO		2
#define TASK2_STK_SIZE		128
TaskHandle_t	Task2Task_Handle;
void task2_task(void *pvParameters); 

#define TASK3_TASK_PRIO		3
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
		LED_ON;
		
//		ADC_ConvertedValue = (ADC_ConvertedValue1 > ADC_ConvertedValue2) ? ADC_ConvertedValue2 : ADC_ConvertedValue1;
		ADC_ConvertedValue = ADC_ConvertedValue1;
		
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue1/4096.0*3.3; 

//		TIM_SetCompare1(PWM_TIMx,ADC_ConvertedValueLocal/3.3*6000+1500);
		vTaskDelay(1);
		
	}
}
void task2_task(void *pvparameters)
{
	while(1){
		
		printf("\r\n The current AD value = %f V \r\n",ADC_ConvertedValueLocal);
		for(t=1; t<=4; t++){		
			printf("\r\n Capture value = %d\r\n",CaptureValue[t-1]);
			sprintf(oled,"%d",CaptureValue[t-1]);
//			OLED_ShowString(36, 2*t-2, (u8*)oled, 16);
//			OLED_ShowNum(36,2*t-2,CaptureValue[t-1],4,16);
		}
		printf("\n\r");
		
		vTaskDelay(50);
	}
}
void task3_task(void *pvParameters)
{
	while(1){
		UpdateCapture( CaptureValue );
		
		PWM_Value[0] = CaptureValue[3] + (int)(ADC_ConvertedValueLocal/3.3*6000.0*Gain);
		
//		TIM_SetCompare1(PWM_TIMx,1500+PWM_Value);
		TIM_SetCompare1(PWM_TIMx,PWM_Value[0]);
		TIM_SetCompare2(PWM_TIMx,3300);
		TIM_SetCompare3(PWM_TIMx,4600);
		TIM_SetCompare4(PWM_TIMx,7250);
		
		vTaskDelay(20);
	}
}

int main(void)
{	
	//系统初始化
	SystemInit();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	//LED、蜂鸣器初始化
	BUZZER_LED_Init();
	GPIO_SetBits(GPIOB,GPIO_Pin_4);
	LED_ON;
	
	// 配置串口
	USART1_Init(9600);
	
	//OLED初始化
//	OLED_Init();
//	OLED_Clear();
	
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
