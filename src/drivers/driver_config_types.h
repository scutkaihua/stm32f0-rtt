

#ifndef __DRIVER_CONFIG_TYPES_H__
#define __DRIVER_CONFIG_TYPES_H__
#include "types.h"
#include "cpu.h"
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------


//<h>/*IO配置类型*/
//</h>
typedef const struct
{
    U8 xPort;						   //端口 A~G
    U8 xPin;				      //pin脚 0~15
    U8 xMode;					    //模式
	  U8 xOType;            //开路
	  U8 xPP;               //上下拉
    U8 xSpeed;            //速度
    U8 xSta;					    //初始状态，高低电平
} t_gpio_map;


//<h>/*uart 配置类型*/
//</h>
typedef const  struct
{
    t_gpio_map gpio_tx;  //tx 端口
    t_gpio_map gpio_rx;  //rx 端口
    char port;       		 //片上 uart 端口 0-n :uart(1)--->uart(n+1)
} t_uart_map;


//<h>/*74h595*/
typedef const struct
{
	 t_gpio_map clk;
	 t_gpio_map cs;
	 t_gpio_map data;
}t_74hc595_map;


/*exti*/
typedef const struct{
	U8 port;
	U8 pin;
	U32 line;       
	U32 trigger;
}t_exti_map;


/*定时器*/
typedef const struct{
	int timer;
	int pre;
	int peroid;
}t_timer_map;


//<h>/*key配置类型*/
//</h>
typedef const struct
{
		t_gpio_map    gpio;    //gpio配置
    U32  keyvalue;         // 键值
} t_key_map;

#endif


