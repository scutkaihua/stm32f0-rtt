#ifndef __DEV_H__
#define __DEV_H__
#include "types.h"
#include "time.h"

/*===================================================
                底层初始化
====================================================*/
void ld_dev_init(void);

/*===================================================
                系统时钟
====================================================*/
void ld_system_clock(unsigned char SK); //SK=8,16,48
void ld_iap(void);                      //iap设置
/*===================================================
                系统运行时间ms
====================================================*/
extern void cpu_us_delay(int us);
extern void cpu_ms_delay(int ms);
#define delayus(n)   cpu_us_delay(n)  //us延时
#define delayms(n)   cpu_ms_delay(n)
void cpu_nvic_reset(void);
time_t time(time_t*t);
/*===================================================
                gpio
====================================================*/
extern void ld_gpio_init(void);
extern void ld_gpio_set(U32 index,U8 value);
extern U8 ld_gpio_get(U32 index);
extern U8 ld_gpio_refresh(void);

extern void ld_hc595_init(void);
extern void ld_hc595_reload(void);
extern void ld_hc595_reload(void);
/*===================================================
                uart
====================================================*/
int ld_uart_init(void);
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
int ld_uart_open(U8 xUart,int xBaud,U8 xData,U8 xParity, U8 xStop,int rxbufsize,int txbufsize);
int ld_uart_close(U8 xUart);
int ld_uart_send(U8 xUart,U8*pBuf,int size);
int ld_uart_read(U8 xUart,U8*pBuf,int size);
int ld_uart_dump(U8 xUart,U8 type);//清收发缓冲:type  :bit0 :rx data    bit1 tx data
int ld_uart_isp(U8 xUart,char*byte,U8 type);	//type: 0 接收    1发送



/*===================================================
                红外数据接口
====================================================*/
/*红外操作接口
* ch    :仓道号 1-n
* io_ir :发送端口
* io_re :接收端口
*/
void ld_ir_init(U8 ch,U8 io_ir,U8 io_re);
void ld_ir_timer_100us(void);
//===============标准化接口==========================//
BOOL ld_ir_read_start(U8 ch,BOOL opposite,U8 cmd,U8 wanlen);
int ld_ir_read_isok(U8 ch,U8*dataout,U8 size);
BOOL ld_ir_busy(U8 ch);
BOOL ld_ir_cmd(U8 ch,U8 cmd);
/*===================================================
                iic数据接口
====================================================*/
BOOL ld_bq27541_read_words(U8 sda,U8 scl,U8*cmd,U8 cmdlen,U16 *dataout);
BOOL ld_bq27541_read_id(U8 sda,U8 scl,U8*dataout);
BOOL ld_bq27541_de_encrypt_charge(U8 sda,U8 scl,U8 cmd);
BOOL ld_bq27541_check_ack(U8 sda,U8 scl);
BOOL ld_bq27541_output_flag(U8 sda,U8 scl,U8*data);




//===============标准化接口==========================//
void ld_iic_init(U8 ch,U8 sda,U8 scl);
BOOL ld_iic_read_start(U8 ch,BOOL opposite,U8 cmd,U8 wanlen);//(ch:1-n,opposite:TRUE反向, cmd 命令, 长度)
int ld_iic_read_isok(U8 ch,U8*dataout,U8 size);//0: 无操作  1: 正在读 2: 读正确
BOOL ld_iic_busy(U8 ch);
BOOL ld_iic_cmd(U8 ch,U8 cmd);


/*===================================================
                flash读写数据接口
====================================================*/
void ld_flash_write(uint32_t WriteAddress, uint8_t *WriteBuf, int32_t WriteNum, uint32_t WriteAddr_page);
void ld_flash_read(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum,uint32_t WriteAddr_page);

/*===================================================
                外部中断
====================================================*/
void ld_exti_init(void);

/*===================================================
                定时器
====================================================*/

void ld_timer3_init(void);
/*===================================================
                看门狗
====================================================*/

void ld_iwdg_init(void);
void ld_iwdg_reload(void);

/*===================================================
                充电宝命令
====================================================*/
typedef enum{
	RC_READ_ID   =10,      //:读充电ID
	RC_READ_DATA =20,      //:读数据
	RC_UNLOCK    =30,      //:解锁05
	RC_LOCK      =50,      //:上锁06
	RC_UNLOCK_1HOUR =40,   //:解锁1小时07
	RC_OUTPUT,             //:读输出标志
}READ_TYPE_CMD;          //充电宝命令
#endif
