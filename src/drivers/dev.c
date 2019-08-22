

#include "includes.h"
#include "stm32f0xx_rcc.h"
/*===================================================
                私有
====================================================*/
#define APPLICATION_ADDRESS     ((U32)(BOOT_LOADER_SIZE +0x08000000))

void IAP_Set()
{
   uint32_t i = 0;
      
  for(i = 0; i < 48; i++)
  {
    *((uint32_t*)(0x20000000 + (i << 2)))=*(volatile U32*)(APPLICATION_ADDRESS + (i<<2));
	}
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE); /* Enable the SYSCFG peripheral clock*/ 
  SYSCFG_MemoryRemapConfig(SYSCFG_MemoryRemap_SRAM);     /* Remap SRAM at 0x00000000 */
}	


void ld_iwdg_init(void)
{
  /* Enable write access to IWDG_PR and IWDG_RLR registers */
  IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
  /* IWDG counter clock: LSI/32 */
  IWDG_SetPrescaler(IWDG_Prescaler_64);//6.4MS
  IWDG_SetReload(3750);//3000       3125*64/40=5000ms
  /* Reload IWDG counter */
  IWDG_ReloadCounter();
  /* Enable IWDG (the LSI oscillator will be enabled by hardware) */
  IWDG_Enable();
}	
#define KR_KEY_RELOAD    ((uint16_t)0xAAAA)
void ld_iwdg_reload(void)
{
	IWDG->KR = KR_KEY_RELOAD;
}


void MSetSysClock(unsigned char SK)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
  /* Set HSION bit */
  RCC->CR |= (uint32_t)0x00000001;

  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
  RCC->CFGR &= (uint32_t)0xF8FFB80C;
  
  /* Reset HSEON, CSSON and PLLON bits */
  RCC->CR &= (uint32_t)0xFEF6FFFF;

  /* Reset HSEBYP bit */
  RCC->CR &= (uint32_t)0xFFFBFFFF;

  /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
  RCC->CFGR &= (uint32_t)0xFFC0FFFF;

  /* Reset PREDIV1[3:0] bits */
  RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

  /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
  RCC->CFGR3 &= (uint32_t)0xFFFFFEAC;

  /* Reset HSI14 bit */
  RCC->CR2 &= (uint32_t)0xFFFFFFFE;

  /* Disable all interrupts */
  RCC->CIR = 0x00000000;
  /* At this stage the HSI is already enabled */

  /* Enable Prefetch Buffer and set Flash Latency */
  FLASH->ACR = FLASH_ACR_PRFTBE | FLASH_ACR_LATENCY;
 
  /* HCLK = SYSCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
      
  /* PCLK = HCLK */
  RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE_DIV1;

  /* PLL configuration = (HSI/2) * 12 = ~48 MHz */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
	if(SK==8)
	{	
		SystemCoreClock    = 8000000;
		RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL2);//8
	}
	if(SK==16)
	{
	 SystemCoreClock    = 16000000;
	 RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL4);//16
	}
	else if(SK==48)
	{
	 SystemCoreClock    = 48000000;
	 RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLSRC_HSI_Div2 | RCC_CFGR_PLLXTPRE_PREDIV1 | RCC_CFGR_PLLMULL12);//48
	}
  /* Enable PLL */
  RCC->CR |= RCC_CR_PLLON;

  /* Wait till PLL is ready */
  while((RCC->CR & RCC_CR_PLLRDY) == 0)
  {
  }

  /* Select PLL as system clock source */
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;    

  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)RCC_CFGR_SWS_PLL)
  {
  }
 
}

/*===================================================
                底层初始化
====================================================*/
void ld_dev_init(void)
{
	MSetSysClock(16);
	//bootloader下重定向向量表
	#if USING_BOOT_LOADER >0
	IAP_Set();
	#endif
	
	//gpio
	ld_gpio_init();
	
	//uart
	ld_uart_init();
	
	//看门狗
	#if ENABLE_IWDG>0
	ld_iwdg_init();
	#endif
	//外部中断
	ld_exti_init();

	//定时器
	ld_timer3_init();
}
