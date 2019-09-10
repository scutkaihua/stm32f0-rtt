

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
//</h>

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


//<h>/*片上spi配置类型  spi flash 专用*/
//</h>
typedef const  struct
{
    t_gpio_map mosi;  		//mosi 端口
    t_gpio_map miso;  		//miso 端口
		t_gpio_map sclk;  	//sclk 端口
		t_gpio_map nss;   	//nss  端口
    char port;       			//片上spi
		U32 speed;       	//速度分频
	  U8 sckmode;      		// 0 :low  1:high
	  U8 latchmode;    		// 触发边沿 0 : 单边  1：双边
	  U8 master_slave;		//主从 :0 从 1：主
} t_spif_map;


//<h> /*spi flash 配置类型*/
//</h>
typedef const struct{
	
	U8  spi;          /*使用的spi设备名*/
	U32 PageSize;      /*页大小*/
	U32 PagesPerSector;/*扇区页数*/
	U32 PagesPerBlock; /*块区页数*/
	U32 Blocks;        /*总块数*/
}t_spi_flash_map;
#endif


