#ifndef __74HC595_H__ 
#define __74HC595_H__ 

#include "stm32f0xx.h" 	

 
#define HC595_CLK_PIN           GPIO_Pin_5 
#define HC595_CLK_GPIO          GPIOA 
#define HC595_CLK_GPIO_CLK      RCC_APB2Periph_GPIOA 
#define HC595_CLK_H()           GPIOA->BSRR = GPIO_Pin_5//GPIO_SetBits(GPIOA,GPIO_Pin_5) 
#define HC595_CLK_L()           GPIOA->BRR = GPIO_Pin_5//GPIO_ResetBits(GPIOA,GPIO_Pin_5)
 
#define HC595_CS_PIN            GPIO_Pin_4 
#define HC595_CS_GPIO           GPIOA 
#define HC595_CS_GPIO_CLK       RCC_APB2Periph_GPIOA 
#define HC595_CS_H()            GPIOA->BSRR = GPIO_Pin_4//GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define HC595_CS_L()            GPIOA->BRR = GPIO_Pin_4//GPIO_ResetBits(GPIOA,GPIO_Pin_4) 
 
#define HC595_DATA_PIN          GPIO_Pin_6 
#define HC595_DATA_GPIO         GPIOA 
#define HC595_DATA_GPIO_CLK     RCC_APB2Periph_GPIOA 
#define HC595_DATA_H()          GPIOA->BSRR = GPIO_Pin_6//GPIO_SetBits(GPIOA,GPIO_Pin_6) 
#define HC595_DATA_L()          GPIOA->BRR = GPIO_Pin_6//;GPIO_ResetBits(GPIOA,GPIO_Pin_6)
 
void HC595Send(uint32_t data); 
void HC595Init(void); 
void HC595Load(void); 
void LedRowOn(uint8_t Row7_0, uint8_t Row15_8, uint8_t Row16_23,uint8_t Row31_24); 
void LedRowOut(uint32_t Data); 
 
#endif




