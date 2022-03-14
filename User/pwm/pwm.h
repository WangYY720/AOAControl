#define  __PWM_H
#include "stm32f10x.h"

#define 	PWMmax	 		60000
#define 	PWM_TIMx		TIM3
#define 	RCC_PWM_TIMx		RCC_APB1Periph_TIM3


#define 	PWM_GPIO_1	GPIOA
#define 	PWM_Pin_1		GPIO_Pin_6
#define 	PWM_GPIO_2	GPIOA
#define 	PWM_Pin_2		GPIO_Pin_7
#define 	PWM_GPIO_3	GPIOB
#define 	PWM_Pin_3		GPIO_Pin_0
#define 	PWM_GPIO_4	GPIOB
#define 	PWM_Pin_4		GPIO_Pin_1


void TIM_PWM_Init(void);



