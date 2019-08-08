
#include "types.h"
#include "driver_config_types.h"
#include "stm32f0xx_misc.h"
#include "stm32f0xx.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_flash.h"
#include "stm32f0xx_iwdg.h"
#include "stm32f0xx_gpio.h"
#include "cpu.h"
#include "string.h"

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
/**************************************************************************************************************
		cpu us delay
 **************************************************************************************************************/
// <h> cpu us delay
// </h>
void cpu_us_delay(int us)
{
    while(us-->0)//8M 100-135us  
		{
			//48M --100us
//		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
//		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
//		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
//		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP(); 
//		__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();__NOP();
					//for(i=0;i<7;i++);   //48M  //117us
			
//		//16M 100us
		__NOP();__NOP();__NOP();__NOP();
    }
}

void cpu_ms_delay(int ms)
{
	while(ms >0){
		cpu_us_delay(1000);
		ms--;
	}	
}

/**************************************************************************************************************
		cpu interrupt functions
 **************************************************************************************************************/
// <h> cpu interrupt functions
// </h>
/*使能禁止中断*/
void hw_interrupt_disable(void)
{
    __disable_irq();
}
void hw_interrupt_enable(void)
{
    __enable_irq();
}


/*临界区访问*/
void enter_critical(void)
{
    __disable_irq();
}
void exit_critical(void)
{
    __enable_irq();
}

/**************************************************************************************************************
		cpu watchdog functions
 **************************************************************************************************************/
// <h> cpu watchdog functions
// </h>
/*
*	看门狗 :看门狗时钟 /分频值 /计数值 === f   T=1/f
* 看门狗时钟: 32KHz
*/
void cpu_iwdg_config(U32 pre,U32 xsetr)
{
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);	//启动寄存器读写
    IWDG_SetPrescaler(pre);					//40K时钟32分频
    IWDG_SetReload(xsetr);				 		//计数器数值
    IWDG_ReloadCounter();			 				//重启计数器
}

/* 启动看门狗 */
void cpu_iwdg_open(void) {
    IWDG_Enable();
}

/* 喂狗 */
void cpu_iwdg_reload(void) {
    IWDG_ReloadCounter();
}

/**************************************************************************************************************
		cpu nivc functions
 **************************************************************************************************************/
// <h> cpu nivc functions
// </h>
void cpu_nivc_set(U32 ch,U32 priority,BOOL cmd)
{

    NVIC_InitTypeDef   NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = ch;
    NVIC_InitStructure.NVIC_IRQChannelPriority = priority;
    NVIC_InitStructure.NVIC_IRQChannelCmd = cmd==TRUE?ENABLE:DISABLE;
    NVIC_Init(&NVIC_InitStructure);

}

/* 复位 */
void cpu_nvic_reset(void)
{
	//__set_FAULTMASK(1); //关中断
	NVIC_SystemReset();
	
}

/**************************************************************************************************************
		cpu gpio functions
 **************************************************************************************************************/
// <h> cpu gpio functions
// </h>
static GPIO_TypeDef* const GPIO_TABLE[] = {GPIOA, GPIOB, GPIOC, GPIOD, 0, GPIOF, 0};
static const U32 RCC_GPIO_TABLE[] = {RCC_AHBPeriph_GPIOA,RCC_AHBPeriph_GPIOB,RCC_AHBPeriph_GPIOC,RCC_AHBPeriph_GPIOD,0,RCC_AHBPeriph_GPIOF};

/*释放调试端口*/
void GPIO_ReleaseSWDJ(void)
{
//		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);
//		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);              
//		DBGMCU->CR  &= ~((uint32_t)1<<5);   
//		AFIO->MAPR = (AFIO->MAPR & ~((uint32_t)0x7 << 24)) | (2 << 24);     /*  PA15 PB3 PB4 */

}

/*
*	配置io管脚
*	xPort :0-8  :A-I
* xPin  :0-15 :Pin0-Pin15
* AF    :
*return :TRUE or FALSE
*/
int cpu_gpioaf_cfg(U8 xPort,U8 xPin,U8 AF)
{
	GPIO_TypeDef* GPIOx;
	if(xPort>6)return FALSE;
	GPIOx=	GPIO_TABLE[xPort];
	GPIO_PinAFConfig(GPIOx,xPin, AF);
	return TRUE;
}
/*
*	配置io管脚
*	xPort :0-8  :A-I
* xPin  :0-15 :Pin0-Pin15
* xMode :GPIO模式
* sta   :默认电平  0 or 1
* xSpeed:速度 :0-3 :2,25,50,100 MHz
* return: true or false
*/
int cpu_gpio_cfg(U8 xPort,U8 xPin,U8 xMode,U8 xOType,U8 xPP,U8 sta,U8 xSpeed)
{
    GPIO_InitTypeDef xGpioInit;
    GPIO_TypeDef* GPIOx;
    if(xPort>6)return FALSE;
    GPIOx=	GPIO_TABLE[xPort];

	  if(xPort==5&&xPin==0)RCC->CR &= ~((uint32_t)RCC_CR_HSEON);
	
    xGpioInit.GPIO_Pin = (1<<xPin)&0xFFFF;
    xGpioInit.GPIO_Mode=(GPIOMode_TypeDef)xMode;
    xGpioInit.GPIO_OType = (GPIOOType_TypeDef)xOType;
    xGpioInit.GPIO_Speed = (GPIOSpeed_TypeDef)xSpeed;
	  xGpioInit.GPIO_PuPd = (GPIOPuPd_TypeDef)xPP;
    RCC_AHBPeriphClockCmd(RCC_GPIO_TABLE[xPort], ENABLE);
    /*当作为输出时，初始化输出电平*/
    if ( xMode == GPIO_Mode_OUT )
    {
        if(sta)GPIO_SetBits(GPIOx,xGpioInit.GPIO_Pin);
        else GPIO_ResetBits(GPIOx,xGpioInit.GPIO_Pin);
    }
    GPIO_Init(GPIOx, &xGpioInit);
    return TRUE;
}

/*
*	设置 xPort.xPin 电平 = sta
*/
int cpu_gpio_set(U8 xPort,U8 xPin,U8 sta)
{
    GPIO_TypeDef* GPIOx;
    if(xPort>6)return FALSE;
    GPIOx=	GPIO_TABLE[xPort];
    if(sta)GPIO_SetBits(GPIOx,(1<<xPin)&0xFFFF);
    else GPIO_ResetBits(GPIOx,(1<<xPin)&0xFFFF);
    return TRUE;
}

/*
* 读取 xPort.xPin 电平
* return :电平状态 0 or 1
*/
int cpu_gpio_get(U8 xPort,U8 xPin)
{
    GPIO_TypeDef* GPIOx;
    if(xPort>6)return FALSE;
    GPIOx=	GPIO_TABLE[xPort];
	return (GPIO_ReadInputDataBit(GPIOx,(1<<xPin)&0xFFFF)==Bit_SET)?1:0;
}

