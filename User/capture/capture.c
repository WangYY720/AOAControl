#include "capture.h"
#include "stm32f10x.h"

u8 	time_capture_sta[4] = {0};
u16 time_capture_val[4] = {0};
u8 	sign[4];
u32 temp[4];

#define capture_sta_1	time_capture_sta[0]
#define capture_sta_2	time_capture_sta[1]
#define capture_sta_3	time_capture_sta[2]
#define capture_sta_4	time_capture_sta[3]

#define capture_val_1	time_capture_val[0]
#define capture_val_2	time_capture_val[1]
#define capture_val_3	time_capture_val[2]
#define capture_val_4	time_capture_val[3]

void Time_Cap_Init()
{
    //定义结构体
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_ICInitTypeDef TIM_ICInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    //使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);


    //配置TIM5
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period=59999;
    TIM_TimeBaseStructure.TIM_Prescaler=23;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);


    //配置输入捕获
    TIM_ICInitStructure.TIM_Channel=TIM_Channel_1 ;
    TIM_ICInitStructure.TIM_ICFilter=0x00;
    TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//上升沿捕获
    TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM2,&TIM_ICInitStructure);
		TIM_ICInitStructure.TIM_Channel=TIM_Channel_2 ;
    TIM_ICInitStructure.TIM_ICFilter=0x00;
    TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//上升沿捕获
    TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM2,&TIM_ICInitStructure);
		TIM_ICInitStructure.TIM_Channel=TIM_Channel_3 ;
    TIM_ICInitStructure.TIM_ICFilter=0x00;
    TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//上升沿捕获
    TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM2,&TIM_ICInitStructure);
		TIM_ICInitStructure.TIM_Channel=TIM_Channel_4 ;
    TIM_ICInitStructure.TIM_ICFilter=0x00;
    TIM_ICInitStructure.TIM_ICPolarity=TIM_ICPolarity_Rising;//上升沿捕获
    TIM_ICInitStructure.TIM_ICPrescaler=TIM_ICPSC_DIV1;
    TIM_ICInitStructure.TIM_ICSelection=TIM_ICSelection_DirectTI;
    TIM_ICInit(TIM2,&TIM_ICInitStructure);

    //配置GPIO口
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;//下拉输入模式
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA,&GPIO_InitStructure);
    GPIO_ResetBits(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);


    //配置中断分组
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStructure.NVIC_IRQChannel=TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority=0;
    NVIC_Init(&NVIC_InitStructure);

    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);//允许溢出中断
		TIM_ITConfig(TIM2,TIM_IT_CC1,ENABLE);//允许捕获中断
		TIM_ITConfig(TIM2,TIM_IT_CC2,ENABLE);//允许捕获中断
		TIM_ITConfig(TIM2,TIM_IT_CC3,ENABLE);//允许捕获中断
    TIM_ITConfig(TIM2,TIM_IT_CC4,ENABLE);//允许捕获中断
		TIM_Cmd(TIM2,ENABLE);//使能TIM2
		
}

void TIM2_IRQHandler(void)//中断处理函数  （尤其要注意中断服务函数名字的写法）
{
		//CH1
    if((capture_sta_1 & 0x80) == 0)//标志位最高位为0，说明还没有成功捕获到高电平，此时捕获中断和溢出中断可被响应
    {
        if(TIM_GetITStatus(TIM2,TIM_IT_CC1)!=RESET)//发生捕获中断
        {
            if((capture_sta_1 & 0x40) == 0)//标志位的次高位为0，说明还没有捕获到一次上升沿，所以这此发生的捕获中断，是捕捉到了一次上升沿
            {
                capture_sta_1 = 0;
                capture_val_1 = 0;
                capture_val_1 = TIM_GetCapture1(TIM2);
                capture_sta_1 |= 0x40;//把标志位的次高位置1，标记为已经捕获到了一次上升沿，这里必须要用或运算，因为time5_capture_sta这个变量的第六位保存着计数器溢出的次数，我们在改变这个变量的最高位和次高位的时候，不能改动后面的第六位数据
                TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling);//设置为下降沿捕获
            }
            else if(capture_sta_1 & 0x40)//标志位次高位为1，说明上次已经捕获到了一次上升沿，那么这次发生的捕获中断就是捕获到的下降沿
            {
                capture_sta_1 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平
                if(TIM_GetCapture1(TIM2) > capture_val_1){
									sign[0] = 0;
									capture_val_1 = TIM_GetCapture1(TIM2) - capture_val_1;//获取当前计数器中的数值
								}else{
									sign[0] = 1;
									capture_val_1 = capture_val_1 - TIM_GetCapture1(TIM2);//获取当前计数器中的数值
								}
								//************************************数据处理**************************************************
								temp[0] = capture_sta_1 & 0x3f;//取出溢出的次数
								temp[0] *= 59999;//溢出的时间总和
								if(sign[0]){
									temp[0] -= capture_val_1;
								}else{
									temp[0] += capture_val_1;
								}
								capture_sta_1 = 0;//清除标志位的最高位，以便能够响应接下来的捕获中断和溢出中断，开启下一次的捕获
								//**********************************************************************************************								
                TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising);//设置为上升沿捕获，为下一次捕获做准备
            }
        }
        if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//发生了溢出中断
        {
            if(capture_sta_1 & 0x40)//判断是否在溢出之前已经捕获到了高电平，其实是肯定已经捕捉到了一次高电平
            {
                if((capture_sta_1 & 0x3f)==0x3f)//高电平时间太长了，已经溢出了2的6次方-1=63次
                {
                    capture_sta_1 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平，此时为强制结束高电平检测
										capture_val_1 = 0xffff;//强制结束后，将计数器的值设为最大，其实程序运行到这里，它已经是最大了
                }
                else
                {
                    capture_sta_1++;//如果溢出次数没有超出标志位的低六位所能表示的范围，那么就让标志位直接加1，以此来记录溢出的次数，溢出次数一直加下去，啥时候超出了标志位的低六位所能表示的范围，那么就强制退出检测，直接认为此次检测高电平已经结束，然后直接开启下一次检测
                }
            }
        }
    }
    //清除中断标志位
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC1);

		//CH2
    if((capture_sta_2 & 0x80) == 0)//标志位最高位为0，说明还没有成功捕获到高电平，此时捕获中断和溢出中断可被响应
    {
        if(TIM_GetITStatus(TIM2,TIM_IT_CC2)!=RESET)//发生捕获中断
        {
            if((capture_sta_2 & 0x40) == 0)//标志位的次高位为0，说明还没有捕获到一次上升沿，所以这此发生的捕获中断，是捕捉到了一次上升沿
            {
                capture_sta_2 = 0;
                capture_val_2 = 0;
                capture_val_2 = TIM_GetCapture2(TIM2);
                capture_sta_2 |= 0x40;//把标志位的次高位置1，标记为已经捕获到了一次上升沿，这里必须要用或运算，因为time5_capture_sta这个变量的第六位保存着计数器溢出的次数，我们在改变这个变量的最高位和次高位的时候，不能改动后面的第六位数据
                TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Falling);//设置为下降沿捕获
            }
            else if(capture_sta_2 & 0x40)//标志位次高位为1，说明上次已经捕获到了一次上升沿，那么这次发生的捕获中断就是捕获到的下降沿
            {
                capture_sta_2 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平
                if(TIM_GetCapture2(TIM2) > capture_val_2){
									sign[1] = 0;
									capture_val_2 = TIM_GetCapture2(TIM2) - capture_val_2;//获取当前计数器中的数值
								}else{
									sign[1] = 1;
									capture_val_2 = capture_val_2 - TIM_GetCapture2(TIM2);//获取当前计数器中的数值
								}
								//************************************数据处理**************************************************
								temp[1] = capture_sta_2 & 0x3f;//取出溢出的次数
								temp[1] *= 59999;//溢出的时间总和
								if(sign[1]){
									temp[1] -= capture_val_2;
								}else{
									temp[1] += capture_val_2;
								}
								capture_sta_2 = 0;//清除标志位的最高位，以便能够响应接下来的捕获中断和溢出中断，开启下一次的捕获
								//**********************************************************************************************								
                TIM_OC2PolarityConfig(TIM2,TIM_ICPolarity_Rising);//设置为上升沿捕获，为下一次捕获做准备
            }
        }
        if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//发生了溢出中断
        {
            if(capture_sta_2 & 0x40)//判断是否在溢出之前已经捕获到了高电平，其实是肯定已经捕捉到了一次高电平
            {
                if((capture_sta_2 & 0x3f)==0x3f)//高电平时间太长了，已经溢出了2的6次方-1=63次
                {
                    capture_sta_2 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平，此时为强制结束高电平检测
										capture_val_2 = 0xffff;//强制结束后，将计数器的值设为最大，其实程序运行到这里，它已经是最大了
                }
                else
                {
                    capture_sta_2++;//如果溢出次数没有超出标志位的低六位所能表示的范围，那么就让标志位直接加1，以此来记录溢出的次数，溢出次数一直加下去，啥时候超出了标志位的低六位所能表示的范围，那么就强制退出检测，直接认为此次检测高电平已经结束，然后直接开启下一次检测
                }
            }
        }
    }
    //清除中断标志位
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC2);

		//CH3
    if((capture_sta_3 & 0x80) == 0)//标志位最高位为0，说明还没有成功捕获到高电平，此时捕获中断和溢出中断可被响应
    {
        if(TIM_GetITStatus(TIM2,TIM_IT_CC3)!=RESET)//发生捕获中断
        {
            if((capture_sta_3 & 0x40) == 0)//标志位的次高位为0，说明还没有捕获到一次上升沿，所以这此发生的捕获中断，是捕捉到了一次上升沿
            {
                capture_sta_3 = 0;
                capture_val_3 = 0;
                capture_val_3 = TIM_GetCapture3(TIM2);
                capture_sta_3 |= 0x40;//把标志位的次高位置1，标记为已经捕获到了一次上升沿，这里必须要用或运算，因为time5_capture_sta这个变量的第六位保存着计数器溢出的次数，我们在改变这个变量的最高位和次高位的时候，不能改动后面的第六位数据
                TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Falling);//设置为下降沿捕获
            }
            else if(capture_sta_3 & 0x40)//标志位次高位为1，说明上次已经捕获到了一次上升沿，那么这次发生的捕获中断就是捕获到的下降沿
            {
                capture_sta_3 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平
                if(TIM_GetCapture3(TIM2) > capture_val_3){
									sign[2] = 0;
									capture_val_3 = TIM_GetCapture3(TIM2) - capture_val_3;//获取当前计数器中的数值
								}else{
									sign[2] = 1;
									capture_val_3 = capture_val_3 - TIM_GetCapture3(TIM2);//获取当前计数器中的数值
								}
								//************************************数据处理**************************************************
								temp[2] = capture_sta_3 & 0x3f;//取出溢出的次数
								temp[2] *= 59999;//溢出的时间总和
								if(sign[2]){
									temp[2] -= capture_val_3;
								}else{
									temp[2] += capture_val_3;
								}
								capture_sta_3 = 0;//清除标志位的最高位，以便能够响应接下来的捕获中断和溢出中断，开启下一次的捕获
								//**********************************************************************************************
                TIM_OC3PolarityConfig(TIM2,TIM_ICPolarity_Rising);//设置为上升沿捕获，为下一次捕获做准备
            }
        }
        if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//发生了溢出中断
        {
            if(capture_sta_3 & 0x40)//判断是否在溢出之前已经捕获到了高电平，其实是肯定已经捕捉到了一次高电平
            {
                if((capture_sta_3 & 0x3f)==0x3f)//高电平时间太长了，已经溢出了2的6次方-1=63次
                {
                    capture_sta_3 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平，此时为强制结束高电平检测
										capture_val_3 = 0xffff;//强制结束后，将计数器的值设为最大，其实程序运行到这里，它已经是最大了
                }
                else
                {
                    capture_sta_3++;//如果溢出次数没有超出标志位的低六位所能表示的范围，那么就让标志位直接加1，以此来记录溢出的次数，溢出次数一直加下去，啥时候超出了标志位的低六位所能表示的范围，那么就强制退出检测，直接认为此次检测高电平已经结束，然后直接开启下一次检测
                }
            }
        }
    }
    //清除中断标志位
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC3);
		
		//CH4
    if((capture_sta_4 & 0x80) == 0)//标志位最高位为0，说明还没有成功捕获到高电平，此时捕获中断和溢出中断可被响应
    {
        if(TIM_GetITStatus(TIM2,TIM_IT_CC4)!=RESET)//发生捕获中断
        {
            if((capture_sta_4 & 0x40) == 0)//标志位的次高位为0，说明还没有捕获到一次上升沿，所以这此发生的捕获中断，是捕捉到了一次上升沿
            {
                capture_sta_4 = 0;
                capture_val_4 = TIM_GetCapture4(TIM2);
                capture_sta_4 |= 0x40;//把标志位的次高位置1，标记为已经捕获到了一次上升沿，这里必须要用或运算，因为time5_capture_sta这个变量的第六位保存着计数器溢出的次数，我们在改变这个变量的最高位和次高位的时候，不能改动后面的第六位数据
                TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Falling);//设置为下降沿捕获
            }
            else if(capture_sta_4 & 0x40)//标志位次高位为1，说明上次已经捕获到了一次上升沿，那么这次发生的捕获中断就是捕获到的下降沿
            {
                capture_sta_4 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平
								if(TIM_GetCapture4(TIM2) > capture_val_4){
									sign[3] = 0;
									capture_val_4 = TIM_GetCapture4(TIM2) - capture_val_4;//获取当前计数器中的数值
								}else{
									sign[3] = 1;
									capture_val_4 = capture_val_4 - TIM_GetCapture4(TIM2);//获取当前计数器中的数值
								}
								//************************************数据处理**************************************************
								temp[3] = capture_sta_4 & 0x3f;//取出溢出的次数
								temp[3] *= 59999;//溢出的时间总和
								if(sign[3]){
									temp[3] -= capture_val_4;
								}else{
									temp[3] += capture_val_4;
								}
								capture_sta_4 = 0;//清除标志位的最高位，以便能够响应接下来的捕获中断和溢出中断，开启下一次的捕获
								//**********************************************************************************************
                TIM_OC4PolarityConfig(TIM2,TIM_ICPolarity_Rising);//设置为上升沿捕获，为下一次捕获做准备
            }
        }
        if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)//发生了溢出中断
        {
            if(capture_sta_4 & 0x40)//判断是否在溢出之前已经捕获到了高电平，其实是肯定已经捕捉到了一次高电平
            {
                if((capture_sta_4 & 0x3f)==0x3f)//高电平时间太长了，已经溢出了2的6次方-1=63次
                {
                    capture_sta_4 |= 0x80;//把标志位的最高位置1，标记已经成功捕获到了一次高电平，此时为强制结束高电平检测
										capture_val_4 = 0xffff;//强制结束后，将计数器的值设为最大，其实程序运行到这里，它已经是最大了
                }
                else
                {
                    capture_sta_4++;//如果溢出次数没有超出标志位的低六位所能表示的范围，那么就让标志位直接加1，以此来记录溢出的次数，溢出次数一直加下去，啥时候超出了标志位的低六位所能表示的范围，那么就强制退出检测，直接认为此次检测高电平已经结束，然后直接开启下一次检测
                }
            }
        }
    }
    //清除中断标志位
    TIM_ClearITPendingBit(TIM2,TIM_IT_CC4);
		
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}
void UpdateCapture(u32 data[])
{
		
	//CH1
	if(temp[0]>=1499 && temp[0]<=7500)
		data[1] = temp[0] + 1;
	
	//CH2
	if(temp[1]>=1499 && temp[1]<=7500)
		data[2] = temp[1] + 1;
	
	//CH3
	if(temp[2]>=1499 && temp[2]<=7500)
		data[3] = temp[2] + 1;
	
	//CH4
	if(temp[3]>=1499 && temp[3]<=7500)
		data[4] = temp[3] + 1;
		
}

