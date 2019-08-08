



/*使用定时器3,频率100us*/
#include "stm32f0xx_tim.h"
void ld_timer3_init(void)
{
	RCC_ClocksTypeDef RCC_Clocks;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  /* TIM3 clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
  /* Enable the TIM3 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	RCC_GetClocksFreq(&RCC_Clocks);//当前时钟源
	
  /* Time base configuration */
  TIM_TimeBaseStructure.TIM_Period = 50-1;
  TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (RCC_Clocks.HCLK_Frequency  / 500000) - 1;  //timer3 counter =500KHz
  TIM_TimeBaseStructure.TIM_ClockDivision  = TIM_CKD_DIV1;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

  /* TIM3 enable counter */
	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
  TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
  TIM_Cmd(TIM3, ENABLE);
}
