#ifndef  __PWM_H
#define  __PWM_H
#include "stm32f10x.h"

#define	TIMx				TIM2
#define TIMx_IRQn		TIM2_IRQn
#define TIMx_IRQHandler 			TIM2_IRQHandler
#define RCC_APB1Periph_TIMx 	RCC_APB1Periph_TIM2

void TIMx_Init(u16 arr,u16 psc);
void TIMx_NVIC_Init (void);


#endif
