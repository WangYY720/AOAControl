#include "stm32f10x.h"
//#include "bsp_usart.h"
#include "bsp_adc.h"
#include "pwm.h"
#include "usart.h"
#include "delay.h"

extern __IO uint16_t ADC_ConvertedValue;
u16 i=0;

// 局部变量，用于保存转换计算后的电压值 	 
float ADC_ConvertedValueLocal;        

// 软件延时
void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
} 

int main(void)
{	
	SystemInit();
	// 配置串口
	USART1_Init(9600);
	
	// ADC 初始化
	ADCx_Init();
	
	// PWM 初始化
	TIM_PWM_Init();
	
	while (1)
	{
		ADC_ConvertedValueLocal =(float) ADC_ConvertedValue/4096.0*3.3; 
		
		if(i==10000){
			printf("\r\n The current AD value = %f V \r\n",
		       ADC_ConvertedValueLocal); 
			i=0;
		}

		TIM_SetCompare1(PWM_TIMx,ADC_ConvertedValueLocal/3.3*6000+1500);

		i++;
	}
}
/*********************************************END OF FILE**********************/
