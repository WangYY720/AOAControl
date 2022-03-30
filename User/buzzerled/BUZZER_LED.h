#ifndef	_BUZZER_LED_H
#define _BUZZER_LED_H

#include "stm32f10x.h"

#define BUZZER_GPIO	GPIOB
#define	BUZZER_PIN	GPIO_Pin_15
#define LED_GPIO		GPIOB
#define LED_PIN			GPIO_Pin_14

#define	BUZZER_ON		GPIO_SetBits(BUZZER_GPIO,BUZZER_PIN)		
#define	BUZZER_OFF	GPIO_ResetBits(BUZZER_GPIO,BUZZER_PIN)	
#define	LED_ON			GPIO_SetBits(LED_GPIO,LED_PIN)		
#define	LED_OFF			GPIO_ResetBits(LED_GPIO,LED_PIN)	

void BUZZER_LED_Init(void);

#endif

