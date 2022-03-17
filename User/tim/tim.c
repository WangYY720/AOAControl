#include "tim.h"

void TIMx_Init(u16 arr,u16 psc){  														//TIM3 初始化 arr重装载值 psc预分频系数
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIMx,ENABLE);				//使能TIMx
    TIMx_NVIC_Init(); 																				//开启TIMx中断向量
	      
    TIM_TimeBaseInitStrue.TIM_Period=arr-1; 									//设置自动重装载值
    TIM_TimeBaseInitStrue.TIM_Prescaler=psc-1; 								//预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up; //计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1; 		//时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(TIMx,&TIM_TimeBaseInitStrue); 						//TIMx初始化设置
    TIM_ITConfig(TIMx, TIM_IT_Update, ENABLE);								//使能TIMx中断    
    TIM_Cmd(TIMx,ENABLE); 																		//使能TIMx
}

void TIMx_NVIC_Init (void){ 																	//开启TIMx中断向量
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIMx_IRQn;	
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x3;	//设置抢占和子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

u8 float2char(float Xfloat)
{
//	u8 Xchar;
	if(Xfloat >= 128)
		return 255;
	else if(Xfloat <= -128)
		return 0;
	else return (int)Xfloat+128;
}

//void TIMx_IRQHandler(void){ 																	//TIMx中断处理函数
//	
//	if (TIM_GetITStatus(TIMx, TIM_IT_Update) != RESET){				//判断是否是TIMx中断
//			TIM_ClearITPendingBit(TIMx, TIM_IT_Update);
//			
//	}
//} 


