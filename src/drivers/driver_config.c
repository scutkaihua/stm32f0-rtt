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
#include "driver_config_types.h"
#include "driver.h"

/******************************************************************
			private
*******************************************************************/


/******************************************************************
			configure variables
*******************************************************************/
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

/*****************
   bootloader��С
*****************/
#define USING_BOOT_LOADER 0
const int BOOT_LOADER_SIZE = 0;
#if USING_BOOT_LOADER >0
void ld_iap(void){
	extenr void IAP_Set();
	IAP_Set();
}
#else
void ld_iap(void){}
#endif


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
	
	//��ŷ� �ڱۿ��� ��λ����  ���ⷢ��  �������   led��      ���ʹ��  ���������     sda       scl
	OUT(C,2) IN(B,10) IN(B,15)  OUT(HC,2)  IN(C,14)  OUT(HC,0)  OUT(HC,1)  IN(C,0)         ODH(B,8)  ODH(B,9)  //�ֵ�1
	OUT(C,3) IN(B,1)  IN(A,8)   OUT(HC,6)  IN(C,15)  OUT(HC,4)  OUT(HC,5)  IN(C,1)         ODH(B,6)  ODH(B,7)  //�ֵ�2
	OUT(B,14)IN(A,0)  IN(F,6)   OUT(HC,7)  IN(F, 0)  OUT(HC,8)  OUT(HC,9)  IN(C,5)         ODH(B,4)  ODH(B,5)  //�ֵ�3
	OUT(B,11)IN(C,13) IN(F,7)   OUT(HC,10) IN(B,12)  OUT(HC,12) OUT(HC,13) IN(C,4)         ODH(D,2)  ODH(B,3)  //�ֵ�4
	OUT(C,6) IN(C,9)  IN(A,12)  OUT(HC,11) IN(C,7)   OUT(HC,14) OUT(HC,15) IN(B,13)        ODH(C,11) ODH(C,12)  //�ֵ�5
	
   0,
};
const unsigned char gpio_number = (sizeof(gpio_map)/sizeof(t_gpio_map)) - 1;

/***********************
			EXTI  ���ó���
***********************/
#define exti(pp,pin)  {PORT_##pp,pin,pin,EXTI_Trigger_Rising},
//<o> exti
t_exti_map exti_map[]={
	
//	//�ڱ�      
//	exti(B,10)  //�ֵ�1
//	exti(B, 1)  //�ֵ�2
//	exti(A, 0)  //�ֵ�3
//	exti(C,13)  //�ֵ�4
//	exti(C, 9)  //�ֵ�5
	
	//��λ
	exti(B,15)  //�ֵ�1
	exti(A, 8)  //�ֵ�2
	exti(F, 6)  //�ֵ�3
	exti(F, 7)  //�ֵ�4
	exti(A,12)  //�ֵ�5
	
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

///*************************
//			Key
//*************************/
//#define KEY(p,pin,v)  {IN(p,pin) v},
//const unsigned char keyDownLevel = 1;  //���µ�ƽ
//t_key_map key_map[]=
//{
//	//�ڱۿ���                ��λ����
//  KEY(B,10,KV(KEY_BAI_BI_INDEX,1)) KEY(B,15,KV(KEY_DAO_WEI_INDEX,1)) 
//	KEY(B,1 ,KV(KEY_BAI_BI_INDEX,2)) KEY(A,8 ,KV(KEY_DAO_WEI_INDEX,2)) 
//	KEY(A,0 ,KV(KEY_BAI_BI_INDEX,3)) KEY(F,6 ,KV(KEY_DAO_WEI_INDEX,3)) 
//	KEY(C,13,KV(KEY_BAI_BI_INDEX,4)) KEY(F,7 ,KV(KEY_DAO_WEI_INDEX,4)) 
//	KEY(C,9 ,KV(KEY_BAI_BI_INDEX,5)) KEY(A,12,KV(KEY_DAO_WEI_INDEX,5)) 
//  0
//};
//const unsigned char 		key_number = sizeof(key_map)/sizeof(t_key_map) - 1;       //io ����


///*************************
//			�ײ��ʼ��
//*************************/
void ld_dev_init(void)
{
	//ϵͳʱ��
	ld_system_clock(16);

	//iap
	ld_iap();
	
	//gpio
	ld_gpio_init();
	
	//uart
	ld_uart_init();
	
	//���Ź�
	#if ENABLE_IWDG>0
	ld_iwdg_init();
	#endif
	//�ⲿ�ж�
	ld_exti_init();

	//��ʱ��
	ld_timer3_init();
}

