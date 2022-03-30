#include "stm32f10x.h"
#include "buzzer_led.h"

void BUZZER_LED_Init(void)
{
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOB时钟

    GPIO_InitStructure.GPIO_Pin = BUZZER_PIN | LED_PIN; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//推挽输出
    GPIO_Init(BUZZER_GPIO, &GPIO_InitStructure); 
}
