#include "stm32f0xx_rcc.h"

/*看门狗初始化*/
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

/*喂狗 */
void ld_iwdg_reload(void)
{
	#define KR_KEY_RELOAD    ((uint16_t)0xAAAA)
	IWDG->KR = KR_KEY_RELOAD;
}
