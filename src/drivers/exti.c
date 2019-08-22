

#include "driver_config_types.h"
#include "stm32f0xx_gpio.h"
#include "string.h"
#include "cpu.h"
#include "stm32f0xx.h" 
//配置
extern t_exti_map exti_map[];
extern const unsigned char exti_number;

//static int irq_ch(int line)
//{
//	if(line<=1)return EXTI0_1_IRQn;
//	if(line<=3)return EXTI2_3_IRQn;
//	if(line<=15)return EXTI4_15_IRQn;
//	return 0;
//}

//初始化
void ld_exti_init(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	int i= 0;	  
	
	//开时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	//中断向量表
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;//irq_ch(map->line);
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	for(;i<exti_number;i++)
	{
		t_exti_map *map = &exti_map[i];
		//初始化io
	  cpu_gpio_cfg(map->port,map->pin,GPIO_Mode_IN,GPIO_OType_OD,GPIO_PuPd_NOPULL,0,GPIO_Speed_2MHz);

		
		//使能外部中断
		SYSCFG_EXTILineConfig(map->port, map->pin);
		EXTI_InitStructure.EXTI_Line = 1<<(map->line);
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
		EXTI_InitStructure.EXTI_Trigger = (EXTITrigger_TypeDef)map->trigger;  
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;
		EXTI_Init(&EXTI_InitStructure);
	}
}

