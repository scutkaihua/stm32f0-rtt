

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
//</h>

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


//<h>/*Ƭ��spi��������  spi flash ר��*/
//</h>
typedef const  struct
{
    t_gpio_map mosi;  		//mosi �˿�
    t_gpio_map miso;  		//miso �˿�
		t_gpio_map sclk;  	//sclk �˿�
		t_gpio_map nss;   	//nss  �˿�
    char port;       			//Ƭ��spi
		U32 speed;       	//�ٶȷ�Ƶ
	  U8 sckmode;      		// 0 :low  1:high
	  U8 latchmode;    		// �������� 0 : ����  1��˫��
	  U8 master_slave;		//���� :0 �� 1����
} t_spif_map;


//<h> /*spi flash ��������*/
//</h>
typedef const struct{
	
	U8  spi;          /*ʹ�õ�spi�豸��*/
	U32 PageSize;      /*ҳ��С*/
	U32 PagesPerSector;/*����ҳ��*/
	U32 PagesPerBlock; /*����ҳ��*/
	U32 Blocks;        /*�ܿ���*/
}t_spi_flash_map;
#endif


