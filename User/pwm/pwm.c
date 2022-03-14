#include "pwm.h"


void TIM_PWM_Init(){  //TIM_PWM初始化 arr重装载值 psc预分频系数
    
		GPIO_InitTypeDef     GPIO_InitStrue;
    TIM_OCInitTypeDef     TIM_OCInitStrue;
    TIM_TimeBaseInitTypeDef     TIM_TimeBaseInitStrue;
    
    RCC_APB1PeriphClockCmd(RCC_PWM_TIMx,ENABLE);					//使能TIMx和相关GPIO时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);	//使能GPIOA时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);	//使能GPIOB时钟
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能AFIO时钟(定时器3通道重映射)
    
		GPIO_InitStrue.GPIO_Pin=PWM_Pin_1;     				// TIM_CH1
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    	// 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;   //设置最大输出速度
    GPIO_Init(PWM_GPIO_1,&GPIO_InitStrue);        //GPIO端口初始化设置
	
	  GPIO_InitStrue.GPIO_Pin=PWM_Pin_2;     				// TIM_CH2
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    	// 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz; 	//设置最大输出速度
    GPIO_Init(PWM_GPIO_2,&GPIO_InitStrue);        //GPIO端口初始化设置
	
	  GPIO_InitStrue.GPIO_Pin=PWM_Pin_3;     				// TIM_CH3
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;   	// 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;   //设置最大输出速度
    GPIO_Init(PWM_GPIO_3,&GPIO_InitStrue);        //GPIO端口初始化设置
		
	  GPIO_InitStrue.GPIO_Pin=PWM_Pin_4;    				// TIM_CH4
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;    	// 复用推挽
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_50MHz;   //设置最大输出速度
    GPIO_Init(PWM_GPIO_4,&GPIO_InitStrue);        //GPIO端口初始化设置
    
//    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE); //映射，重映射只用于64、100、144脚单片机
   //当没有重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PA6，PA7,PB0,PB1
   //当部分重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PB4，PB5,PB0,PB1 (GPIO_PartialRemap_TIM3)
   //当完全重映射时，TIM3的四个通道CH1，CH2，CH3，CH4分别对应PC6，PC7,PC8,PC9 (GPIO_FullRemap_TIM3) 
	      
    TIM_TimeBaseInitStrue.TIM_Period=59999;   		 							 //设置自动重装载值(60000对应20ms，1500-7500对应pwm波)
    TIM_TimeBaseInitStrue.TIM_Prescaler=23;       	 //预分频系数
    TIM_TimeBaseInitStrue.TIM_CounterMode=TIM_CounterMode_Up;   	 //计数器向上溢出
    TIM_TimeBaseInitStrue.TIM_ClockDivision=TIM_CKD_DIV1;       	 //时钟的分频因子，起到了一点点的延时作用，一般设为TIM_CKD_DIV1
    TIM_TimeBaseInit(PWM_TIMx,&TIM_TimeBaseInitStrue);       				//TIMx初始化设置(设置PWM的周期)
    
    TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM1;        							// PWM模式1:CNT < CCR时输出有效电平
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;							// 设置极性-有效电平为：高电平
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;					// 输出使能
		TIM_OC1Init(PWM_TIMx,&TIM_OCInitStrue);        									//TIMx的通道1 PWM 模式设置
		
    TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM1;        							// PWM模式1:CNT < CCR时输出有效电平
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;							// 设置极性-有效电平为：高电平
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;					// 输出使能
		TIM_OC2Init(PWM_TIMx,&TIM_OCInitStrue);        									//TIMx的通道1 PWM 模式设置
		
		TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM1;        							// PWM模式1:CNT < CCR时输出有效电平
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;							// 设置极性-有效电平为：高电平
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;					// 输出使能
		TIM_OC3Init(PWM_TIMx,&TIM_OCInitStrue);        									//TIMx的通道1 PWM 模式设置
		
		TIM_OCInitStrue.TIM_OCMode=TIM_OCMode_PWM1;        							// PWM模式1:CNT < CCR时输出有效电平
    TIM_OCInitStrue.TIM_OCPolarity=TIM_OCPolarity_High;							// 设置极性-有效电平为：高电平
    TIM_OCInitStrue.TIM_OutputState=TIM_OutputState_Enable;					// 输出使能
		TIM_OC4Init(PWM_TIMx,&TIM_OCInitStrue);        									//TIMx的通道1 PWM 模式设置

    TIM_OC1PreloadConfig(PWM_TIMx,TIM_OCPreload_Enable);        		//使能预装载寄存器
		TIM_OC2PreloadConfig(PWM_TIMx,TIM_OCPreload_Enable);
		TIM_OC3PreloadConfig(PWM_TIMx,TIM_OCPreload_Enable);
		TIM_OC4PreloadConfig(PWM_TIMx,TIM_OCPreload_Enable);
    
    TIM_Cmd(PWM_TIMx,ENABLE);        																//使能TIM3   
}




