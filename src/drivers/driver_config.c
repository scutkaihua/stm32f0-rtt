/*========================================================

* file name 		:	drv_config.c
* Author 			:	kaihua.yang
* Version			��	v1.00
* Date				��	11/30/2015
* Description		��	Ӳ�����������ļ�

*********************************************************
*��Ӳ���йز����� Configure or interface����
*********************************************************
========================================================*/
/******************************************************************
			includes
*******************************************************************/
#include "config.h"
#include "driver_config_types.h"
#include "types.h"
#include "config.h"
//#include "gtDev.h"

/******************************************************************
			private
*******************************************************************/


/******************************************************************
			configure variables
*******************************************************************/
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
/*****************
   �ⲿʱ������
*****************/
//<o> ʱ��--�ⲿ����


/*****************
   IDWG ��������
*****************/
//<o> iwdg
//const unsigned int iwatchdog_xpres =  IWATCHDOG_XPRES;
//const unsigned int iwatchdog_xsetr =  IWATCHDOG_XSETR;

#include "stm32f0xx.h"

/***********************
			GPIO  ���ó���
***********************/
#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3
#define PORT_E 4
#define PORT_F 5
#define PORT_HC  0xFF  
#define OUT(pp,pin)  {PORT_##pp,pin,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_NOPULL,GPIO_Speed_Level_3,0},
#define OUTH(pp,pin) {PORT_##pp,pin,GPIO_Mode_OUT,GPIO_OType_PP,GPIO_PuPd_NOPULL,GPIO_Speed_Level_3,1},
#define ODH(pp,pin)  {PORT_##pp,pin,GPIO_Mode_OUT,GPIO_OType_OD,GPIO_PuPd_NOPULL,GPIO_Speed_Level_3,1},
#define IN(pp,pin)   {PORT_##pp,pin,GPIO_Mode_IN,GPIO_OType_OD,GPIO_PuPd_NOPULL,GPIO_Speed_Level_3,0},
#define AF(pp,pin)   {PORT_##pp,pin,GPIO_Mode_AF,GPIO_OType_PP,GPIO_PuPd_UP,GPIO_Speed_Level_3,0},

//<o> gpio
t_gpio_map gpio_map[]=
{
	//ϵͳLED��    485����ʹ��                                                                    
	  OUT(F,5)     OUT(A,1)
		driver_config_io_map()
   0,
};
const unsigned char gpio_number = (sizeof(gpio_map)/sizeof(t_gpio_map)) - 1;

/***********************
			EXTI  ���ó���
***********************/
#define exti(pp,pin)  {PORT_##pp,pin,pin,EXTI_Trigger_Rising},
//<o> exti
t_exti_map exti_map[]={
	driver_config_exti_map()
	0
};
const unsigned char exti_number = (sizeof(exti_map) /sizeof(t_exti_map)) - 1;
/*************************
			Uart
*************************/
//<o> uart
t_uart_map uart_map[] = {
  
	{AF(A,9) AF(A,10) 1},  //uart1 
	{AF(A,2) AF(A,3) 2},   //uart2 
	{0}	
};
const unsigned char uart_number = sizeof(uart_map) /(sizeof(t_uart_map)) - 1;
