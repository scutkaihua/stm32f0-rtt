/*========================================================

* file name 		:	uart.c
* Author 			:	kaihua.yang
* Version			：	v1.00
* Date				：	5/31/2017
* Description		：	uart动

*********************************************************
*与硬件有关部分在 Configure or interface给出
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
	调试
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

extern int gt_uart_wait_msleep(int ms);           //接收等待延时函数

#define Uart_Lock(xUart)                          //对xUart 上锁
#define Uart_Unlock(xUart)                        //对xUart 解锁
#define UART_MAX             2                    //本驱动最大UART,根据具体mcu而定，简单修改

/**************************************
	cpu 函数
**************************************/
// <h> cpu 串口常量
typedef enum {
    //接收开关
    UART_CMD_ENABLE_RX,
    UART_CMD_DISABLE_RX,

    //发送开关
    UART_CMD_ENABLE_TX,
    UART_CMD_DISABLE_TX,

    //直接发送一个字符
    UART_CMD_SEND_BYTE,

    //清空缓冲
    UART_CMD_DUMP,
} UART_CMD;

typedef const struct {
    USART_TypeDef * USART;   //cpu 串口寄存器
    U8 IRQn;                 //cpu 串口中断号
    U32 rcc;                 //cpu 串口时钟
    U8  APB12;               // 1: APB1  2:APB2
		U8  AF;                  //uart 的af 功能,f10x没有AF,当remap才会出现
} CPU_USART;

static CPU_USART cpu_uart[]= {
		{0     ,          0,                    0,0,0},             //uart0不存在
    {USART1,USART1_IRQn,RCC_APB2Periph_USART1,2,GPIO_AF_0 },
    {USART2,USART2_IRQn,RCC_APB1Periph_USART2,1,GPIO_AF_1,},
//    {USART3,USART3_IRQn,RCC_APB1Periph_USART3,1,0},
//    {UART4,UART4_IRQn,RCC_APB1Periph_UART4,1,0},
//    {UART5,UART5_IRQn,RCC_APB1Periph_UART5,1,0},
};
static const unsigned char uart_max = UART_MAX;

/*
*	设置 波特率等待参数
	xUart  :串口号
	xBaud  :波特率
	xData  :数据位  5,6,7,8
	xPari  :校验位  无 0，偶 1，奇2
	xStop  :停止位 0:0.5bit   1:1bit  2:1.5bit  3:2bit
*/
BOOL cpu_uart_param(U8 xUart,U32 xBaud,U8 xData,U8 xPari,U8 xStop)
{
    USART_InitTypeDef xUartPara;
    CPU_USART*uart;
    if(xUart>uart_max)return FALSE;
    uart = &cpu_uart[xUart];

    /*---------------------------串口参数初始化----------------------------*/
    /*停止位*/
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
    /*校验位*/
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
    /*数据位*/
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
	初始化串口
	xUart  :串口号
	xBaud  :波特率
	xData  :数据位  5,6,7,8
	xPari  :校验位  无 0，偶 1，奇2
	xStop  :停止位 0:0.5bit   1:1bit  2:1.5bit  3:2bit
  tx,rx  :管脚配置
	return :
		成功 TRUE
		失败 FALSE
*/
BOOL cpu_uart_init(U8 xUart,U32 xBaud,U8 xData,U8 xPari,U8 xStop,t_gpio_map*tx,t_gpio_map*rx)
{
    NVIC_InitTypeDef xNVIC;
    CPU_USART*uart;
    USART_ClockInitTypeDef USART_ClockInitStructure;

    if(xUart>uart_max)return FALSE;
    uart = &cpu_uart[xUart];


    /*---------------------------初始化时钟-------------------------------*/
    if(uart->APB12 == 1)RCC_APB1PeriphClockCmd(uart->rcc,ENABLE);
    if(uart->APB12 == 2)RCC_APB2PeriphClockCmd(uart->rcc,ENABLE);

    //先关闭uart再配置参数
    USART_Cmd(uart->USART, DISABLE);

    /*---------------------------初始化GPIO AF-----------------------------*/
    cpu_gpioaf_cfg(tx->xPort, tx->xPin,uart->AF);
    cpu_gpio_map_config(tx,0);

    cpu_gpioaf_cfg(rx->xPort, rx->xPin,uart->AF);
    cpu_gpio_map_config(rx,0);


    /*---------------------------中断向量号设置-----------------------------*/
		/*
		 警告 0: 当使用os时，NVIC不能设置 为group_0 preemption = 0; 这样会阻塞或阻止os
		 的SVC中断，导致内核态的os 函数运行被打断，可能引起 函数的运行出错:
		 实际遇到的问题是:
						所有程序运行都没有问题，长时间运行时，出现 任务 Wait_MUT ，导致任务挂掉
						措施 :   减少其它中断的优先级，避免抢占svc (2018-1-31)
		*/
    /* Enable the USART Interrupt */
		//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);
		xNVIC.NVIC_IRQChannel = uart->IRQn;
		//    xNVIC.NVIC_IRQChannelPreemptionPriority = 1;
		//    xNVIC.NVIC_IRQChannelSubPriority = xUart;
		xNVIC.NVIC_IRQChannelPriority = xUart;
		xNVIC.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&xNVIC);
    //禁用收发中断
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

    /*---------------------------串口参数初始化----------------------------*/
    cpu_uart_param(xUart,xBaud,xData,xPari,xStop);

    return TRUE;
}

/*串口控制的收发*/
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
	类型
**************************************/


/*设备实现*/
typedef struct st_uart_device {
    t_uart_map *map;  //端口默认配置
    CBuffer tx;			  //发送缓冲
    CBuffer rx;	      //接收缓冲
} t_uart_device;

t_uart_device uarts[UART_MAX];


/*查找设备*/
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
	串口中断，保存数据,发送读出数据到byte
  port: 串口商口号
	byte: 读入 或 读出
	type: 0 接收    1发送
	return :  TRUE or FALSE
*/

BOOL cpu_uart_isp(U8 port,char*byte,U8 type)
{
    struct st_uart_device * i = find(port);
    if(i == NULL)return FALSE;

    //接收
    if(type == 0) {
        if(cbuffer_isfull(&i->rx) == TRUE) {
            cbuffer_clear(&i->rx);
        }

        return cbuffer_write(&i->rx,*byte);
    }

    //发送
    else if(type == 1) {
        //读
        return cbuffer_read(&i->tx,byte);
    }
    return FALSE;
}


/*************************************
	public functions
**************************************/
/*初始化*/
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
	打开串口
	
	xUart  :1-n  Uart1-Uartn
	xBaud  :波特率
	xData  :数据位，5 6 7 8 
	xParity:无 0，偶 1，奇2
	xStop  :停止位 0:0.5bit   1:1bit  2:1.5bit  3:2bit
	rxbufsize :接收缓存  >0 以中断方式接收
	txbufsize :发送缓存  >0 以中断方式发送

	return :  TRUE 成功  FALSE 失败
*/
int ld_uart_open(U8 xUart,int xBaud,U8 xData,U8 xParity, U8 xStop,int rxbufsize,int txbufsize)
{
	t_uart_device*uart = find(xUart);
	if(uart==NULL)return FALSE;
	
	//上锁
	Uart_Lock(xUart);
	
	//初始化硬件
	if(cpu_uart_init(uart->map->port,(U32)xBaud, xData, xParity, xStop,&uart->map->gpio_tx,&uart->map->gpio_rx)==FALSE)
		goto UART_INIT_ERROR;
	
	
	//初始化接收内存
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

	//使能uart.port
	if(txbufsize > 0)
    {
        cpu_uart_ctrl(uart->map->port,UART_CMD_ENABLE_TX,NULL);
        cpu_uart_ctrl(uart->map->port,UART_CMD_ENABLE_RX,NULL);
    }
	
	//解锁
	Uart_Unlock(xUart);
	return TRUE;
	
	
	//错误返回
	UART_INIT_ERROR:
		Uart_Unlock(xUart);
		return FALSE;
}

/*
	关闭串口
	xUart  :1-n  Uart1-Uartn
	return  :TRUE  :成功   FALSE:失败
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
	发送数据
  xUart  :1-n  Uart1-Uartn
	如果 txbufsize >0 使用中断
	否则 使用 等待 (用于串口打印)

	return :发送的字节数
*/
int ld_uart_send(U8 xUart,U8*pBuf,int size)
{
	  int has = 0;
    char*pin= (char*)pBuf;
    t_uart_device* i = find(xUart);
    if(i == NULL  || size <= 0)return 0;

		Uart_Lock(xUart);
	
    if(i->tx.maxlen == 0){
        //查询方式
        while(size-- >0)
            cpu_uart_ctrl(i->map->port,UART_CMD_SEND_BYTE,(void*)pBuf++);
        
    }else{
	
        //中断方式
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
	接收数据:只支持中断接收
	xUart  :1-n  Uart1-Uartn
	pBuf  :[out]接收的数据
	size  :读取长度
	return :实际接收的长度 
*/
int ld_uart_read(U8 xUart,U8*pBuf,int size)
{

    char *pout = (char*)pBuf;
    t_uart_device* i = find(xUart);
    int len =0;  //实际长度
    if(i == NULL)return 0;

    /*接收缓冲为空*/
    if(i->rx.pbuf == NULL)return 0;
		
		/*上锁*/
		Uart_Lock(xUart);
	
    do
    {
        while((size>0)&&cbuffer_read(&i->rx,&pout[len])!=FALSE)
        {
            size--;
            len++;
        }
        if(size<=0)break;
        //字节超时,接收到数据才有效
        if(len>0)break;
    } while (1);
		
		/*解锁*/
		Uart_Unlock(xUart);
		
    return len;
}

/*
	放弃所有数据
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
	

	处理接收  发送数据,中断形式
  xUart  :1-n  Uart1-Uartn
	串口中断，保存数据,发送读出数据到byte
	byte: 读入 或 读出
	type: 0 接收    1发送
	return :TRUE or FALSE
*/

int ld_uart_isp(U8 xUart,char*byte,U8 type)
{
	return cpu_uart_isp(xUart,byte,type);
}

/*
* 获取 uart 收发缓冲
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

	
