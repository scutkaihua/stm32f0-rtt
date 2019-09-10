/*========================================================

* file name 		:	spi.c
* Author 			:	kaihua.yang
* Version			��	v1.00
* Date				��	11/30/2015
* Description		��spi ���� 

*********************************************************
*��Ӳ���йز����� Configure or interface����
*********************************************************
========================================================*/


/**************************************
	includes
**************************************/
#include "stm32f0xx_i2c.h"
#include "assert.h"
#include "string.h"
#include "cpu.h"
#include "stdio.h"
#include "driver_config_types.h"

/**************************************
	����
**************************************/
#ifdef __SPIBUS_DEBUG
#define spi_printf gt_printf
#else
#define spi_printf(...)
#endif
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
/**************************************
	�ⲿ����
**************************************/
extern t_spif_map spif_map[];
extern const unsigned char spif_number;

/**************************************
	private 
**************************************/
//<o> cpu ����function
#include "stm32f0xx_spi.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"

typedef const struct{
    SPI_TypeDef* spi;
    U32          rcc;
    U8           rcc_port;  //1 AHB1  2:AHB2
}cpu_spi_map;


static cpu_spi_map spis[]={
    //spi0 
    {0,0,0},
    //spi1
    {SPI1, RCC_APB2Periph_SPI1, 2 },
    //spi2
    {SPI2, RCC_APB1Periph_SPI2, 1 },
//    //spi3
//    {SPI3, RCC_APB1Periph_SPI3, 1 },
};

/*���� index :Ƭ��spi ��*/
static t_spif_map* find(U8 port)
{
	int i = 0;
	for(i=0;i<spif_number;i++)
		if(spif_map[i].port == port)
			return (t_spif_map*)&spif_map[i];
		
		return NULL;
}

/*��ʼ��һ��map������,map:����*/
/*map->port :1-3:SPI1-SPI3*/
static BOOL cpu_spif_init(t_spif_map* map)
{
    SPI_InitTypeDef  SPI_InitStructure;
    cpu_spi_map *cpu = &(spis[map->port]);

    //ʱ��
    if(cpu->rcc_port == 1 )RCC_APB1PeriphClockCmd(cpu->rcc, ENABLE);
    else if( cpu->rcc_port == 2 )RCC_APB2PeriphClockCmd(cpu->rcc,ENABLE);
    else return FALSE;

    /*GPIO����*/
    cpu_gpioaf_cfg(map->mosi.xPort, map->mosi.xPin, GPIO_AF_0);//SPI1 SPI2 ��ʹ�� GPIO_AF_0 ����
    cpu_gpioaf_cfg(map->miso.xPort, map->miso.xPin, GPIO_AF_0);
    cpu_gpioaf_cfg(map->sclk.xPort, map->sclk.xPin, GPIO_AF_0);
    //cpu_gpioaf_cfg(map->nss.xPort, map->nss.xPin, cpu->af);

		cpu_gpio_map_config((&(map->mosi)),0);
		cpu_gpio_map_config((&(map->miso)),0);
		cpu_gpio_map_config((&(map->sclk)),0);
		cpu_gpio_map_config((&(map->nss)),0);

    cpu_gpio_map_set((&map->nss),0,1);

    //spi����
    /*!< spi configuration */
    SPI_Cmd(cpu->spi, DISABLE);
    /* sEE_spi configuration */
    SPI_InitStructure.SPI_BaudRatePrescaler = map->speed;                                   /*�ٶ�*/
    SPI_InitStructure.SPI_Mode = ((map->master_slave == 0)?SPI_Mode_Slave:SPI_Mode_Master); /*����*/
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;                      /*ȫ˫��*/
    SPI_InitStructure.SPI_CPOL = ((map->sckmode ==0)?SPI_CPOL_Low:SPI_CPOL_High);           /*sck��ƽ*/
    SPI_InitStructure.SPI_CPHA = ((map->latchmode == 0) ? SPI_CPHA_1Edge :SPI_CPHA_2Edge);  /*�����ػ���˫����*/
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                                       /*8λ*/
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;    																	/*��λ��ǰ*/
    SPI_InitStructure.SPI_NSS  = SPI_NSS_Soft;           																		/*ʹ�����NSS*/
    SPI_InitStructure.SPI_CRCPolynomial = 7;

    //ע��ע��ע�⣺�����ȳ�ʼ������ʹ��2017-11-13

    /* Apply sEE_spi configuration after enabling it */
    SPI_Init(cpu->spi, &SPI_InitStructure);
		
		SPI_NSSInternalSoftwareConfig(cpu->spi,SPI_NSSInternalSoft_Set);  //����stm32f0 ��Ч
		SPI_RxFIFOThresholdConfig(cpu->spi,SPI_RxFIFOThreshold_QF);       //����stm32f0,������������λΪ8λ     
    
    /* sEE_spi Peripheral Enable */
    SPI_Cmd(cpu->spi, ENABLE);
    return TRUE;
}

/*cpu spi ���Ͷ���һ���ֽ�,ֱ�ӷ��ض�ȡ������*/
U8 static cpu_spif_send(U8 index,U8 send)
{
	t_spif_map* s  = find(index);
    cpu_spi_map *cpu = &(spis[s->port]);
	if(s == NULL)
        return FALSE;
	//�ȴ��������
	while (SPI_I2S_GetFlagStatus(cpu->spi, SPI_I2S_FLAG_TXE) == RESET){}
	//����һ���ֽ�
	//cpu->spi->DR = (U16)send;
		SPI_SendData8(cpu->spi,send);
	//�ȴ�����һ���ֽ�
	while (SPI_I2S_GetFlagStatus(cpu->spi, SPI_I2S_FLAG_RXNE) == RESET){}
	//����һ���ֽ� 
		send = SPI_ReceiveData8(cpu->spi);
    //send = (U8)cpu->spi->DR;
    return send;
}

/*����nss��ƽ*/
BOOL cpu_spif_nss(U8 index,U8 level)
{
	t_spif_map* s  = find(index);
	if(s == NULL)return FALSE;
	cpu_gpio_map_set((&(s->nss)),0,level);
	return TRUE;
}
/**************************************
	public 
**************************************/

/*��ʼ�� index spi   return :[true or false]*/
int ld_spi_init(U8 index)
{
	t_spif_map* s  = find(index);
	if(s == NULL)return FALSE;
	return cpu_spif_init(s);
}

/*����һ���ֽڣ�����ȡһ���ֽ� [readbyte==null,ʱ�����] return:[true or false]�]*/
int ld_spi_transfer(U8 index ,U8 sendbyte,U8 *readbyte)
{
    U8 r = cpu_spif_send(index,sendbyte);
    if(readbyte != NULL) *readbyte = r;
    return TRUE;
}

/*����nss ��ƽ��index: Ƭ��spi ������,level:���õ�ƽ:[0:low,1:high],return:[true or false]*/
int ld_spi_nss(U8 index,U8 level)
{
	return cpu_spif_nss(index,level);
}



