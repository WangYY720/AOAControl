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

#define JudgeValue 	4000
#define PWM_step		50

extern __IO uint16_t ADC_ConvertedValue1;
extern __IO uint16_t ADC_ConvertedValue2;
// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal[3],ADC_ConvertedValue; 
u8 ADCValueFlag[3];
s16 PWM_Value[5],PWM_Goal[5],temp_main[10];
u32 CaptureValue[5];
u16 t;
char oled[8]={0};

#define START_TASK_PRIO		1
#define START_STK_SIZE		128
TaskHandle_t	StartTask_Handle;
void start_task(void *pvParameters);

#define TASK1_TASK_PRIO		2
#define TASK1_STK_SIZE		128
TaskHandle_t	Task1Task_Handle;
void task1_task(void *pvParameters);

#define TASK2_TASK_PRIO		1
#define TASK2_STK_SIZE		128
TaskHandle_t	Task2Task_Handle;
void task2_task(void *pvParameters); 

#define TASK3_TASK_PRIO		2
#define TASK3_STK_SIZE		128
TaskHandle_t	Task3Task_Handle;
void task3_task(void *pvParameters);

//#define TASK4_TASK_PRIO		1
//#define TASK4_STK_SIZE		128
//TaskHandle_t	Task4Task_Handle;
//void task4_task(void *pvParameters);

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

//线程1：更新ADC和输入捕获数值线程
void task1_task(void *pvparameters)
{
	while(1){
		
		//更新TIM3 4个通道的输入捕获
		UpdateCapture( CaptureValue );
		
		//更新ADC1和2的输入值
		ADC_ConvertedValueLocal[1] =(float) ADC_ConvertedValue1/4096.0*3.3;
		ADC_ConvertedValueLocal[2] =(float) ADC_ConvertedValue2/4096.0*3.3;		
		//ADC1
		if(ADC_ConvertedValueLocal[1] < 1.65){
			ADCValueFlag[1] = 0;
		}else{
			ADC_ConvertedValueLocal[1] = 3.3 - ADC_ConvertedValueLocal[1];
			ADCValueFlag[1] = 1;
		}
		//ADC2
		if(ADC_ConvertedValueLocal[2] < 1.65){
			ADCValueFlag[2] = 1;
		}else{
			ADC_ConvertedValueLocal[2] = 3.3 - ADC_ConvertedValueLocal[2];
			ADCValueFlag[2] = 0;
		}
		//ADC1和ADC2取较小值
		ADC_ConvertedValueLocal[0] = ADC_ConvertedValueLocal[1] < ADC_ConvertedValueLocal[2] ? ADC_ConvertedValueLocal[1] : ADC_ConvertedValueLocal[2];
		ADCValueFlag[0] = ADC_ConvertedValueLocal[1] < ADC_ConvertedValueLocal[2] ? ADCValueFlag[1] : ADCValueFlag[2];

		//更新速率为50Hz
		vTaskDelay(20);
		
	}
}
//线程2：更新OLED和串口线程 LED和蜂鸣器 
void task2_task(void *pvparameters)
{
		while(1){
		
//		printf("\r\n The current ADC1 value = %f V \r\n",ADC_ConvertedValueLocal[1]);
		sprintf(oled,"%.1f",ADC_ConvertedValue1/4096.0*360.0);
		OLED_ShowString(56, 0, (u8*)oled, 16);
		
//		printf("\r\n The current ADC2 value = %f V \r\n",ADC_ConvertedValueLocal[2]);
		sprintf(oled,"%.1f",ADC_ConvertedValue2/4096.0*360.0);
		OLED_ShowString(56, 2, (u8*)oled, 16);
		
		if(CaptureValue[3] < JudgeValue)
			OLED_ShowString(56, 4, (u8*)"SYS ON", 16);
		else
			OLED_ShowString(56, 4, (u8*)"SYS OFF", 16);
		
		for(t=1; t<=4; t++){		
//			printf("\r\n Capture value = %d\r\n",CaptureValue[t-1]);
			sprintf(oled,"%d",CaptureValue[t]);
			OLED_ShowString(0, 2*t-2, (u8*)oled, 16);
//			OLED_ShowNum(36,2*t-2,CaptureValue[t-1],4,16);
		}
//		printf("\n\r");
		
		if(CaptureValue[3] < JudgeValue)
			temp_main[0] = 1;
		else temp_main[0] = 0;
		
		if(temp_main[1] != temp_main[0]){
			if(CaptureValue[3] < JudgeValue)
				LED_ON;
			else
				LED_OFF;
			
			BUZZER_ON;
			vTaskDelay(1000);
			BUZZER_OFF;
		}
		temp_main[1] = temp_main[0];
		
		vTaskDelay(150);
		
		OLED_Clear();
	}
}
//线程3：控制舵机线程
void task3_task(void *pvParameters)
{
	while(1){
		
		//控制系统开启
		if(CaptureValue[3] < JudgeValue){
			//如果ADC数值有效
			if(ADC_ConvertedValueLocal[0]<3.3/9.0){
				
				temp_main[3] = 6000 * ADC_ConvertedValueLocal[0] / 3.3 * 2;
				
				//襟翼控制
				if(ADCValueFlag[1] && ADCValueFlag[2]){
					PWM_Goal[1] = 1500 - temp_main[3];
					PWM_Goal[2] = 1500 - temp_main[3];
				}
		
				//鸭翼控制
				if(ADCValueFlag[1] && ADCValueFlag[2]){
					PWM_Goal[3] = temp_main[3];
					PWM_Goal[4] = temp_main[3];
				}
//				else{
//					PWM_Goal[3] = - PWM_Value[1];
//					PWM_Goal[4] = - PWM_Value[1];
//				}
				
			}else{
				PWM_Goal[1] = 1500;
				PWM_Goal[2] = 1500;
				PWM_Goal[3] = 0;
				PWM_Goal[4] = 0;
			}
		}

		//控制系统关闭
		else{
			//襟翼收回
			PWM_Goal[1] = 1500;
			PWM_Goal[2] = 1500;
			//鸭翼按遥控器信号控制
			PWM_Goal[3] = 0;
			PWM_Goal[4] = 0;
		}
		
		//CH1
		if(PWM_Value[1] < PWM_Goal[1]){
			if((PWM_Goal[1]-PWM_Value[1]) > PWM_step)
				PWM_Value[1] += PWM_step;
			else 
				PWM_Value[1] = PWM_Goal[1];
		}else if(PWM_Value[1] > PWM_Goal[1]){
			if((PWM_Value[1]-PWM_Goal[1]) > PWM_step)
				PWM_Value[1] -= PWM_step;
			else 
				PWM_Value[1] = PWM_Goal[1];
		}
		//CH2
		if(PWM_Value[2] < PWM_Goal[2]){
			if((PWM_Goal[2]-PWM_Value[2]) > PWM_step)
				PWM_Value[2] += PWM_step;
			else 
				PWM_Value[2] = PWM_Goal[2];
		}else if(PWM_Value[2] > PWM_Goal[2]){
			if((PWM_Value[1]-PWM_Goal[2]) > PWM_step)
				PWM_Value[2] -= PWM_step;
			else 
				PWM_Value[2] = PWM_Goal[2];
		}
		//CH3
		if(PWM_Value[3] < PWM_Goal[3]){
			if((PWM_Goal[3]-PWM_Value[3]) > PWM_step)
				PWM_Value[3] += PWM_step;
			else 
				PWM_Value[3] = PWM_Goal[3];
		}else if(PWM_Value[3] > PWM_Goal[3]){
			if((PWM_Value[3]-PWM_Goal[3]) > PWM_step)
				PWM_Value[3] -= PWM_step;
			else 
				PWM_Value[3] = PWM_Goal[3];
		}
		//CH4
		if(PWM_Value[4] < PWM_Goal[4]){
			if((PWM_Goal[4]-PWM_Value[4]) > PWM_step)
				PWM_Value[4] += PWM_step;
			else 
				PWM_Value[4] = PWM_Goal[4];
		}else if(PWM_Value[4] > PWM_Goal[4]){
			if((PWM_Value[4]-PWM_Goal[4]) > PWM_step)
				PWM_Value[4] -= PWM_step;
			else 
				PWM_Value[4] = PWM_Goal[4];
		}
		
		//襟翼
		TIM_SetCompare1(PWM_TIMx,PWM_Value[1]);
		TIM_SetCompare2(PWM_TIMx,PWM_Value[2]);
		//鸭翼
		TIM_SetCompare3(PWM_TIMx,CaptureValue[1] + PWM_Value[3]);
		TIM_SetCompare4(PWM_TIMx,CaptureValue[2] + PWM_Value[4]);
		
		//速率20Hz
		vTaskDelay(20);
	}
}
////线程4：LED和蜂鸣器 
//void task4_task(void *pvparameters)
//{
//	while(1){
//		
//		if(CaptureValue[3] < JudgeValue)
//			temp_main[0] = 1;
//		else temp_main[0] = 0;
//		
//		if(temp_main[1] != temp_main[0]){
//			BUZZER_ON;
//			vTaskDelay(1000);
//			BUZZER_OFF;
//			if(CaptureValue[3] < JudgeValue)
//				LED_ON;
//			else
//				LED_OFF;
//		}
//		temp_main[1] = temp_main[0];
//		
//		vTaskDelay(200);
//	}
//}

int main(void)
{	
	//系统初始化
	SystemInit();
	delay_init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	
	//LED、蜂鸣器初始化
	BUZZER_LED_Init();
	
	// 配置串口
	USART1_Init(9600);
	LED_ON;
	BUZZER_ON;
	Delay_ms(300);
	BUZZER_OFF;
	Delay_ms(200);
	BUZZER_ON;
	Delay_ms(300);
	BUZZER_OFF;
	
	//OLED初始化
	OLED_Init();
	OLED_Clear();
	
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
