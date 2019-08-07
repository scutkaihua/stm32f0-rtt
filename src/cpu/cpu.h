


#ifndef _CPU_H__
#define __CPU_H__

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
/**************************************************************************************************************
		cc functions
 **************************************************************************************************************/
// <h> cc functions
// </h>
#define inline  __inline

#include "types.h"
/**************************************************************************************************************
		cpu debug function
 **************************************************************************************************************/
#ifdef CPU_DEBUG_ENABLE
#define cpu_printf
#else
#define cpu_printf(...)
#endif

/**************************************************************************************************************
		cpu us delay
 **************************************************************************************************************/
// <h> cpu us delay
// </h>
void cpu_us_delay(int us);  //裸跑us
void cpu_ms_delay(int ms);  //裸跑ms

/**************************************************************************************************************
		cpu interrupt functions
 **************************************************************************************************************/
// <h> cpu interrupt functions
// </h>
/*使能禁止中断*/
void hw_interrupt_disable(void);
void hw_interrupt_enable(void);

/*临界区访问*/
void enter_critical(void);
void exit_critical(void);


/**************************************************************************************************************
		cpu watchdog functions
 **************************************************************************************************************/
// <h> cpu watchdog functions
// </h>
/*
*	看门狗 :看门狗时钟 /分频值 /计数值 === f   T=1/f
* 看门狗时钟: 32KHz
*/
void cpu_iwdg_config(U32 pre,U32 xsetr);

/* 启动看门狗 */
void cpu_iwdg_open(void);

/* 喂狗 */
void cpu_iwdg_reload(void);

/* 选项字节，自动启动看门狗*/
void cpu_iwdg_autoStart(void);

/**************************************************************************************************************
		cpu nivc functions
 **************************************************************************************************************/
// <h> cpu nivc functions
// </h>
void cpu_nivc_set(U32 ch,U32 priority,BOOL cmd);


/**************************************************************************************************************
		cpu gpio functions
 **************************************************************************************************************/
// <h> cpu gpio functions
// </h>
/*
*	配置io管脚
*	xPort :0-8  :A-I
* xPin  :0-15 :Pin0-Pin15
* xMode :GPIO模式
* sta   :默认电平  0 or 1
* xSpeed:速度 :0-3 :2,25,50,100 MHz
* return: true or false
*/

void GPIO_ReleaseSWDJ(void);/*释放调试端口*/
int cpu_gpioaf_cfg(U8 xPort,U8 xPin,U8 AF);
int cpu_gpio_cfg(U8 xPort,U8 xPin,U8 xMode,U8 xOType,U8 sta,U8 xSpeed);
int cpu_gpio_set(U8 xPort,U8 xPin,U8 sta);
int cpu_gpio_get(U8 xPort,U8 xPin);


/*通用cpu gpio 设置读写*/
#define cpu_gpio_map_config(set,X)       cpu_gpio_cfg(set[(X)].xPort,set[(X)].xPin,set[(X)].xMode,set[(X)].xOType,set[(X)].xSta,set[(X)].xSpeed)
#define cpu_gpio_map_get(set,X)				   cpu_gpio_get(set[(X)].xPort,set[(X)].xPin)
#define cpu_gpio_map_set(set,X,value)		 cpu_gpio_set(set[(X)].xPort,set[(X)].xPin,value)


/*定义一个端口的配置*/
#define cpu_gpio_map_Item(name)  	PORT_##name,PIN_##name,MODE_##name,OType_##name,SPEED_##name,STATE_##name
#define cpu_gpio_map(name)    		{cpu_gpio_map_Item(name)},
#define cpu_gpio_map_null()				{255,255,255,255,255,255},
#define cpu_Key_Map(name)         {cpu_gpio_map_Item(name),KEYVALUE_##name},
#define cpu_Adc_Map(name)         {PORT_##name,PIN_##name, 3,1,0,1,0,CHANNEL_##name},


#endif


