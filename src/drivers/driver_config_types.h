

#ifndef __DRIVER_CONFIG_TYPES_H__
#define __DRIVER_CONFIG_TYPES_H__
#include "types.h"
#include "cpu.h"
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------


//<h>/*IO��������*/
//</h>
typedef const struct
{
    U8 xPort;						   //�˿� A~G
    U8 xPin;				      //pin�� 0~15
    U8 xMode;					    //ģʽ
	  U8 xOType;            //��·
	  U8 xPP;               //������
    U8 xSpeed;            //�ٶ�
    U8 xSta;					    //��ʼ״̬���ߵ͵�ƽ
} t_gpio_map;


//<h>/*uart ��������*/
//</h>
typedef const  struct
{
    t_gpio_map gpio_tx;  //tx �˿�
    t_gpio_map gpio_rx;  //rx �˿�
    char port;       		 //Ƭ�� uart �˿� 0-n :uart(1)--->uart(n+1)
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


/*��ʱ��*/
typedef const struct{
	int timer;
	int pre;
	int peroid;
}t_timer_map;


//<h>/*key��������*/
//</h>
typedef const struct
{
		t_gpio_map    gpio;    //gpio����
    U32  keyvalue;         // ��ֵ
} t_key_map;

#endif


