#ifndef __DEV_H__
#define __DEV_H__
#include "types.h"
#include "time.h"

/*===================================================
                �ײ��ʼ��
====================================================*/
void ld_dev_init(void);

/*===================================================
                ϵͳʱ��
====================================================*/
void ld_system_clock(unsigned char SK); //SK=8,16,48
void ld_iap(void);                      //iap����
/*===================================================
                ϵͳ����ʱ��ms
====================================================*/
extern void cpu_us_delay(int us);
extern void cpu_ms_delay(int ms);
#define delayus(n)   cpu_us_delay(n)  //us��ʱ
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
int ld_uart_open(U8 xUart,int xBaud,U8 xData,U8 xParity, U8 xStop,int rxbufsize,int txbufsize);
int ld_uart_close(U8 xUart);
int ld_uart_send(U8 xUart,U8*pBuf,int size);
int ld_uart_read(U8 xUart,U8*pBuf,int size);
int ld_uart_dump(U8 xUart,U8 type);//���շ�����:type  :bit0 :rx data    bit1 tx data
int ld_uart_isp(U8 xUart,char*byte,U8 type);	//type: 0 ����    1����



/*===================================================
                �������ݽӿ�
====================================================*/
/*��������ӿ�
* ch    :�ֵ��� 1-n
* io_ir :���Ͷ˿�
* io_re :���ն˿�
*/
void ld_ir_init(U8 ch,U8 io_ir,U8 io_re);
void ld_ir_timer_100us(void);
//===============��׼���ӿ�==========================//
BOOL ld_ir_read_start(U8 ch,BOOL opposite,U8 cmd,U8 wanlen);
int ld_ir_read_isok(U8 ch,U8*dataout,U8 size);
BOOL ld_ir_busy(U8 ch);
BOOL ld_ir_cmd(U8 ch,U8 cmd);
/*===================================================
                iic���ݽӿ�
====================================================*/
BOOL ld_bq27541_read_words(U8 sda,U8 scl,U8*cmd,U8 cmdlen,U16 *dataout);
BOOL ld_bq27541_read_id(U8 sda,U8 scl,U8*dataout);
BOOL ld_bq27541_de_encrypt_charge(U8 sda,U8 scl,U8 cmd);
BOOL ld_bq27541_check_ack(U8 sda,U8 scl);
BOOL ld_bq27541_output_flag(U8 sda,U8 scl,U8*data);




//===============��׼���ӿ�==========================//
void ld_iic_init(U8 ch,U8 sda,U8 scl);
BOOL ld_iic_read_start(U8 ch,BOOL opposite,U8 cmd,U8 wanlen);//(ch:1-n,opposite:TRUE����, cmd ����, ����)
int ld_iic_read_isok(U8 ch,U8*dataout,U8 size);//0: �޲���  1: ���ڶ� 2: ����ȷ
BOOL ld_iic_busy(U8 ch);
BOOL ld_iic_cmd(U8 ch,U8 cmd);


/*===================================================
                flash��д���ݽӿ�
====================================================*/
void ld_flash_write(uint32_t WriteAddress, uint8_t *WriteBuf, int32_t WriteNum, uint32_t WriteAddr_page);
void ld_flash_read(uint32_t ReadAddress, uint8_t *ReadBuf, int32_t ReadNum,uint32_t WriteAddr_page);

/*===================================================
                �ⲿ�ж�
====================================================*/
void ld_exti_init(void);

/*===================================================
                ��ʱ��
====================================================*/

void ld_timer3_init(void);
/*===================================================
                ���Ź�
====================================================*/

void ld_iwdg_init(void);
void ld_iwdg_reload(void);

/*===================================================
                ��籦����
====================================================*/
typedef enum{
	RC_READ_ID   =10,      //:�����ID
	RC_READ_DATA =20,      //:������
	RC_UNLOCK    =30,      //:����05
	RC_LOCK      =50,      //:����06
	RC_UNLOCK_1HOUR =40,   //:����1Сʱ07
	RC_OUTPUT,             //:�������־
}READ_TYPE_CMD;          //��籦����
#endif
