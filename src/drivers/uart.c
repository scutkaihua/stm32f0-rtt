/*========================================================

* file name 		:	uart.c
* Author 			:	kaihua.yang
* Version			��	v1.00
* Date				��	5/31/2017
* Description		��	uart��

*********************************************************
*��Ӳ���йز����� Configure or interface����
*********************************************************
========================================================*/
#include "driver_config_types.h"
#include "assert.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "cbuffer.h"
#include "cpu.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx.h"
#include "stm32f0xx_misc.h"



/**************************************
	����
***************************************/
#ifdef __EEPROM_DEBUG
#define u_printf gt_printf
#else
#define u_printf(...)
#endif


/**************************************

	Configure  or  interface
***************************************/

extern t_uart_map uart_map[];
extern const unsigned char uart_number;

extern int gt_uart_wait_msleep(int ms);           //���յȴ���ʱ����

#define Uart_Lock(xUart)                          //��xUart ����
#define Uart_Unlock(xUart)                        //��xUart ����
#define UART_MAX             2                    //���������UART,���ݾ���mcu���������޸�

/**************************************
	cpu ����
**************************************/
// <h> cpu ���ڳ���
typedef enum {
    //���տ���
    UART_CMD_ENABLE_RX,
    UART_CMD_DISABLE_RX,

    //���Ϳ���
    UART_CMD_ENABLE_TX,
    UART_CMD_DISABLE_TX,

    //ֱ�ӷ���һ���ַ�
    UART_CMD_SEND_BYTE,

    //��ջ���
    UART_CMD_DUMP,
} UART_CMD;

typedef const struct {
    USART_TypeDef * USART;   //cpu ���ڼĴ���
    U8 IRQn;                 //cpu �����жϺ�
    U32 rcc;                 //cpu ����ʱ��
    U8  APB12;               // 1: APB1  2:APB2
		U8  AF;                  //uart ��af ����,f10xû��AF,��remap�Ż����
} CPU_USART;

static CPU_USART cpu_uart[]= {
		{0     ,          0,                    0,0,0},             //uart0������
    {USART1,USART1_IRQn,RCC_APB2Periph_USART1,2,GPIO_AF_1 },
    {USART2,USART2_IRQn,RCC_APB1Periph_USART2,1,GPIO_AF_1,},
//    {USART3,USART3_IRQn,RCC_APB1Periph_USART3,1,0},
//    {UART4,UART4_IRQn,RCC_APB1Periph_UART4,1,0},
//    {UART5,UART5_IRQn,RCC_APB1Periph_UART5,1,0},
};
static const unsigned char uart_max = UART_MAX;

/*
*	���� �����ʵȴ�����
	xUart  :���ں�
	xBaud  :������
	xData  :����λ  5,6,7,8
	xPari  :У��λ  �� 0��ż 1����2
	xStop  :ֹͣλ 0:0.5bit   1:1bit  2:1.5bit  3:2bit
*/
BOOL cpu_uart_param(U8 xUart,U32 xBaud,U8 xData,U8 xPari,U8 xStop)
{
    USART_InitTypeDef xUartPara;
    CPU_USART*uart;
    if(xUart>uart_max)return FALSE;
    uart = &cpu_uart[xUart];

    /*---------------------------���ڲ�����ʼ��----------------------------*/
    /*ֹͣλ*/
    switch (xStop)
    {
    case 0:
    case 1:
    default:
        xUartPara.USART_StopBits = USART_StopBits_1;
        break;
    case 2:
        xUartPara.USART_StopBits = USART_StopBits_1_5;
        break;
    case 3:
        xUartPara.USART_StopBits = USART_StopBits_2;
        break;
    }
    /*У��λ*/
    switch (xPari)
    {
    case 1:
        xUartPara.USART_Parity = USART_Parity_Even;
        break;
    case 2:
        xUartPara.USART_Parity = USART_Parity_Odd;
        break;
    case 0:
    default:
        xUartPara.USART_Parity = USART_Parity_No;
        break;
    }
    /*����λ*/
    switch (xData)
    {
    case 7://UART_DATA_7D:
        xUartPara.USART_WordLength = USART_WordLength_8b;
        break;
    case 8://UART_DATA_8D:
    default:
        if(xUartPara.USART_Parity == USART_Parity_No)
            xUartPara.USART_WordLength = USART_WordLength_8b;
        else
            xUartPara.USART_WordLength = USART_WordLength_9b;
        break;
    }
    xUartPara.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    xUartPara.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    xUartPara.USART_BaudRate = xBaud;
    USART_Init(uart->USART, &xUartPara);
    USART_Cmd(uart->USART,ENABLE);
	
    return TRUE;

}



/*
	��ʼ������
	xUart  :���ں�
	xBaud  :������
	xData  :����λ  5,6,7,8
	xPari  :У��λ  �� 0��ż 1����2
	xStop  :ֹͣλ 0:0.5bit   1:1bit  2:1.5bit  3:2bit
  tx,rx  :�ܽ�����
	return :
		�ɹ� TRUE
		ʧ�� FALSE
*/
BOOL cpu_uart_init(U8 xUart,U32 xBaud,U8 xData,U8 xPari,U8 xStop,t_gpio_map*tx,t_gpio_map*rx)
{
    NVIC_InitTypeDef xNVIC;
    CPU_USART*uart;
    USART_ClockInitTypeDef USART_ClockInitStructure;

    if(xUart>uart_max)return FALSE;
    uart = &cpu_uart[xUart];


    /*---------------------------��ʼ��ʱ��-------------------------------*/
    if(uart->APB12 == 1)RCC_APB1PeriphClockCmd(uart->rcc,ENABLE);
    if(uart->APB12 == 2)RCC_APB2PeriphClockCmd(uart->rcc,ENABLE);

    //�ȹر�uart�����ò���
    USART_Cmd(uart->USART, DISABLE);

    /*---------------------------��ʼ��GPIO AF-----------------------------*/
    cpu_gpioaf_cfg(tx->xPort, tx->xPin,uart->AF);
    cpu_gpio_map_config(tx,0);

    cpu_gpioaf_cfg(rx->xPort, rx->xPin,uart->AF);
    cpu_gpio_map_config(rx,0);


    /*---------------------------�ж�����������-----------------------------*/
		/*
		 ���� 0: ��ʹ��osʱ��NVIC�������� Ϊgroup_0 preemption = 0; ��������������ֹos
		 ��SVC�жϣ������ں�̬��os �������б���ϣ��������� ���������г���:
		 ʵ��������������:
						���г������ж�û�����⣬��ʱ������ʱ������ ���� Wait_MUT ����������ҵ�
						��ʩ :   ���������жϵ����ȼ���������ռsvc (2018-1-31)
		*/
    /* Enable the USART Interrupt */
		//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
		xNVIC.NVIC_IRQChannel = uart->IRQn;
		//    xNVIC.NVIC_IRQChannelPreemptionPriority = 1;
		//    xNVIC.NVIC_IRQChannelSubPriority = xUart;
		xNVIC.NVIC_IRQChannelPriority = xUart;
		xNVIC.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&xNVIC);
    //�����շ��ж�
    USART_ITConfig(uart->USART, USART_IT_RXNE, DISABLE);
    USART_ITConfig(uart->USART, USART_IT_TXE, DISABLE);
    USART_ITConfig(uart->USART, USART_IT_TC, DISABLE);
    /*---------------------------Clock-------------------------------------*/			
    //Clock
    USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;    
    USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;
    USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;
    USART_ClockInit(uart->USART, &USART_ClockInitStructure);

    /*---------------------------���ڲ�����ʼ��----------------------------*/
    cpu_uart_param(xUart,xBaud,xData,xPari,xStop);

    return TRUE;
}

/*���ڿ��Ƶ��շ�*/
BOOL cpu_uart_ctrl(U8 xUart,U8 cmd, void*arg)
{
    CPU_USART*uart;
    if(xUart>uart_max)return FALSE;
    uart = &cpu_uart[xUart];

    switch(cmd)
    {
    case UART_CMD_ENABLE_TX:
				USART_ClearITPendingBit(uart->USART, USART_IT_RXNE);
				USART_ClearITPendingBit(uart->USART, USART_IT_TXE);
				USART_ClearITPendingBit(uart->USART, USART_FLAG_ORE);
				USART_ClearITPendingBit(uart->USART, USART_IT_TC);
        USART_ITConfig(uart->USART, USART_IT_TXE, ENABLE);
        USART_ITConfig(uart->USART, USART_IT_TC, ENABLE);
        break;

    case UART_CMD_DISABLE_TX:
				USART_ITConfig(uart->USART, USART_IT_TXE, DISABLE);
				USART_ITConfig(uart->USART, USART_IT_TC, ENABLE);	
				USART_ClearITPendingBit(uart->USART, USART_IT_RXNE);
				USART_ClearITPendingBit(uart->USART, USART_IT_TXE);
				USART_ClearITPendingBit(uart->USART, USART_FLAG_ORE);
				USART_ClearITPendingBit(uart->USART, USART_IT_TC);
        break;

    case UART_CMD_ENABLE_RX:
        USART_ITConfig(uart->USART, USART_IT_RXNE, ENABLE);
        break;

    case UART_CMD_DISABLE_RX:
        USART_ITConfig(uart->USART, USART_IT_RXNE, DISABLE);
        break;
    
    case UART_CMD_SEND_BYTE:
        USART_SendData(uart->USART, *(U8*)arg);
        /* Loop until the end of transmission */
        while (USART_GetFlagStatus(uart->USART, USART_FLAG_TC) == RESET)
        {}            
        return TRUE;

    default:
        break;
    }

    return TRUE;
}




/*************************************
	����
**************************************/


/*�豸ʵ��*/
typedef struct st_uart_device {
    t_uart_map *map;  //�˿�Ĭ������
    CBuffer tx;			  //���ͻ���
    CBuffer rx;	      //���ջ���
} t_uart_device;

t_uart_device uarts[UART_MAX];


/*�����豸*/
static t_uart_device *find(U8 port)
{
	int i = 0;
	for(i=0;i<uart_number ;i++)
	{
		if(uarts[i].map->port ==  port)return &uarts[i];
	}
	return NULL;
}

/*
	�����жϣ���������,���Ͷ������ݵ�byte
  port: �����̿ں�
	byte: ���� �� ����
	type: 0 ����    1����
	return :  TRUE or FALSE
*/

BOOL cpu_uart_isp(U8 port,char*byte,U8 type)
{
    struct st_uart_device * i = find(port);
    if(i == NULL)return FALSE;

    //����
    if(type == 0) {
        if(cbuffer_isfull(&i->rx) == TRUE) {
            cbuffer_clear(&i->rx);
        }

        return cbuffer_write(&i->rx,*byte);
    }

    //����
    else if(type == 1) {
        //��
        return cbuffer_read(&i->tx,byte);
    }
    return FALSE;
}


/*************************************
	public functions
**************************************/
/*��ʼ��*/
int ld_uart_init()
{
	int i = 0,j = sizeof(uarts)/sizeof(t_uart_device);
	memset(uarts,0,sizeof(uarts));
	for(i=0;i<j && i < uart_number;i++)
	{
		uarts[i].map = &uart_map[i];
	}
	return NULL;
}

/*
	�򿪴���
	
	xUart  :1-n  Uart1-Uartn
	xBaud  :������
	xData  :����λ��5 6 7 8 
	xParity:�� 0��ż 1����2
	xStop  :ֹͣλ 0:0.5bit   1:1bit  2:1.5bit  3:2bit
	rxbufsize :���ջ���  >0 ���жϷ�ʽ����
	txbufsize :���ͻ���  >0 ���жϷ�ʽ����

	return :  TRUE �ɹ�  FALSE ʧ��
*/
int ld_uart_open(U8 xUart,int xBaud,U8 xData,U8 xParity, U8 xStop,int rxbufsize,int txbufsize)
{
	t_uart_device*uart = find(xUart);
	if(uart==NULL)return FALSE;
	
	//����
	Uart_Lock(xUart);
	
	//��ʼ��Ӳ��
	if(cpu_uart_init(uart->map->port,(U32)xBaud, xData, xParity, xStop,&uart->map->gpio_tx,&uart->map->gpio_rx)==FALSE)
		goto UART_INIT_ERROR;
	
	
	//��ʼ�������ڴ�
    if(rxbufsize>0)
	if(cbuffer_init(&uart->rx,rxbufsize) == FALSE)
	{
			u_printf("uart init rx cbuffer error.\r\n");
			goto UART_INIT_ERROR;
	}
    if(txbufsize>0)
	if(cbuffer_init(&uart->tx,txbufsize) == FALSE)
	{
			u_printf("uart init tx cbuffer error.\r\n");
			cbuffer_free(&uart->rx);
			goto UART_INIT_ERROR;
	}

	//ʹ��uart.port
	if(txbufsize > 0)
    {
        cpu_uart_ctrl(uart->map->port,UART_CMD_ENABLE_TX,NULL);
        cpu_uart_ctrl(uart->map->port,UART_CMD_ENABLE_RX,NULL);
    }
	
	//����
	Uart_Unlock(xUart);
	return TRUE;
	
	
	//���󷵻�
	UART_INIT_ERROR:
		Uart_Unlock(xUart);
		return FALSE;
}

/*
	�رմ���
	xUart  :1-n  Uart1-Uartn
	return  :TRUE  :�ɹ�   FALSE:ʧ��
*/
int ld_uart_close(U8 xUart)
{
	CPU_USART*uart = &cpu_uart[xUart];
	t_uart_device* i = find(xUart);
	if(i == NULL)return FALSE;
	Uart_Lock(xUart);
	USART_Cmd(uart->USART, DISABLE);
	cpu_uart_ctrl(i->map->port,UART_CMD_DISABLE_TX,NULL);
	cpu_uart_ctrl(i->map->port,UART_CMD_DISABLE_RX,NULL);
	cbuffer_free(&i->tx);
	cbuffer_free(&i->rx);
	Uart_Unlock(xUart);
	return TRUE;
}


/*
	��������
  xUart  :1-n  Uart1-Uartn
	��� txbufsize >0 ʹ���ж�
	���� ʹ�� �ȴ� (���ڴ��ڴ�ӡ)

	return :���͵��ֽ���
*/
int ld_uart_send(U8 xUart,U8*pBuf,int size)
{
	  int has = 0;
    char*pin= (char*)pBuf;
    t_uart_device* i = find(xUart);
    if(i == NULL  || size <= 0)return 0;

		Uart_Lock(xUart);
	
    if(i->tx.maxlen == 0){
        //��ѯ��ʽ
        while(size-- >0)
            cpu_uart_ctrl(i->map->port,UART_CMD_SEND_BYTE,(void*)pBuf++);
        
    }else{
	
        //�жϷ�ʽ
        while(cbuffer_write(&i->tx,*pin) != FALSE && size>0) {
            pin++;
            size--;
            has++;
            if(size==0)break;
        }
        //enable tx interrupt
        cpu_uart_ctrl(i->map->port,UART_CMD_ENABLE_TX,NULL);
    }
		
		Uart_Unlock(xUart);

    return has;
}

/*
	��������:ֻ֧���жϽ���
	xUart  :1-n  Uart1-Uartn
	pBuf  :[out]���յ�����
	size  :��ȡ����
	return :ʵ�ʽ��յĳ��� 
*/
int ld_uart_read(U8 xUart,U8*pBuf,int size)
{

    char *pout = (char*)pBuf;
    t_uart_device* i = find(xUart);
    int len =0;  //ʵ�ʳ���
    if(i == NULL)return 0;

    /*���ջ���Ϊ��*/
    if(i->rx.pbuf == NULL)return 0;
		
		/*����*/
		Uart_Lock(xUart);
	
    do
    {
        while((size>0)&& (cbuffer_read(&i->rx,&pout[len])!=FALSE) )
        {
            size--;
            len++;
        }
        if(size<=0)break;
        //�ֽڳ�ʱ,���յ����ݲ���Ч
        break;
    } while (1);
		
		/*����*/
		Uart_Unlock(xUart);
		
    return len;
}

/*
	������������
  xUart  :1-n  Uart1-Uartn
	type  :bit0 :rx data    bit1 tx data
	return :FALSE  or TRUE
*/
int ld_uart_dump(U8 xUart,U8 type)
{
	t_uart_device* i = find(xUart);
	if( i== NULL ) return FALSE;
	
	Uart_Lock(xUart);
	
	if(type & 0x01)cbuffer_clear(&i->rx);
	if(type & 0x02)cbuffer_clear(&i->tx);
	
	Uart_Unlock(xUart);
	
	return TRUE;
}

/*
	

	�������  ��������,�ж���ʽ
  xUart  :1-n  Uart1-Uartn
	�����жϣ���������,���Ͷ������ݵ�byte
	byte: ���� �� ����
	type: 0 ����    1����
	return :TRUE or FALSE
*/

int ld_uart_isp(U8 xUart,char*byte,U8 type)
{
	return cpu_uart_isp(xUart,byte,type);
}

/*
* ��ȡ uart �շ�����
* return :TRUE or FALSE
*/
int ld_uart_cache(U8 xUart,char**rx,char**tx)
{
	t_uart_device* i = find(xUart);
	if( i== NULL ) return FALSE;
	*rx = (char*)&(i->rx);
	*tx = (char*)&(i->tx);
	return TRUE;
}

/*
* �����Ƿ�Ϊ��
*/
int ld_uart_is_tx_empty(U8 xUart)
{
	t_uart_device* i = find(xUart);
	if( i== NULL ) return FALSE;
	if(i->tx.pbuf==NULL)return TRUE;
	if(i->tx.rptr==i->tx.wptr)return TRUE;
	return FALSE;
}
	
