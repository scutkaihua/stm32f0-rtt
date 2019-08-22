

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

//===========================����=============================//
typedef enum{
	PROCESS_EVENT_PACKET           =0x40,   //�¼�:���ݰ�����
	PROCESS_EVENT_COMM_LEASE,               //�¼�:������ݰ�����
	PROCESS_EVENT_COMM_CTRL,                //�¼�:��������
	PROCESS_EVENT_COMM_ENTRY,               //�¼�:��������
}PROCESS_EVENT;


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//<e>�Ƿ�ʹ�ÿ��Ź�
#define ENABLE_IWDG               0
//</e>

//<o>485���ں�  <1=>uart1 <2=>uart2
#define COM_485                   2

//485����ʹ��
#define enable_485_tx()   ld_gpio_set(2,1)
#define disable_485_tx()  ld_gpio_set(2,0)
#define enable_485_rx()   disable_485_tx()
#define disable_485_rx()  enable_485_tx()

//<s>ʹ��Ӳ������
#define USING_HARDWARE_CONFIG  "25-50-NAS-V5.h"
#include USING_HARDWARE_CONFIG



//<o>����ͬʱ�������ֵ���
#define CHANNEL_CHARGE_MAX    2

//<o>�ֵ���ų���
#define CHANNEL_ID_MAX             10

//<o>����ʱ��ʧ�ܳ��Դ���         
#define UNLOCK_RETRY_TIMES        2

//<o>���ʱ���ֵ�����˸Ƶ��ms
#define LEASE_LED_FLASH_TIME      500

//<o>�黹��籦ʱ�������ݳ��Դ���
#define RETURN_READ_TIMES         2

//<o>�ֵ�����ʱ����С����%
#define CHANNEL_LED_LIGHT_UFSOC   50

//<o>ѭ������籦ʱ��(ms)
#define BAO_READ_DATA_MAX_MS      2500

//<o>�澯�¶�(��)
#define BAO_WARN_TEMPERATURE      60
//<o>�����¶�(��)
#define BAO_ERROR_TEMPERATURE      65
//<h>��籦���ж�
//<o>ʧ�ܴ���
#define BAO_READ_ERROR_RETYR_TIMES         3
//<o>�ɹ�����
#define BAO_READ_OK_RETYR_TIMES            1

//<o>����ʶ����ϼ���
#define BAO_DINGZHEN_ERROR_TIMES           2

//<o>����ʶ����ϼ���
#define BAO_IR_ERROR_TIMES                 2
//</h>

//<h>�����¼�
//<o>���ʱ����(ms)
#define EJECT_INTERVAL                     10000
//<o>����ʧ�ܼ��κ���ʱһ��ʱ��(����)
#define EJECT_FAIL_TIMES                    6
//<o>����ʧ�ܼ��κ���ʱʱ��(��)
#define EJECT_FAIL_WAIT                     7200
//</h>

//<h>������̲���
//<h> ����
//<o>ֹͣ�������%
#define BUCHONG_STOP_UFSOC_MAX            99
//<h>1Сʱ����
//<o>ֹͣ�������%
#define BUCHONG_1HOUR_STOP_UFSOC_MAX      85
//<o>�������
#define BUCHONG_1HOUR_TIMES               3
//</h>

//<o>���޲���ʱ�� min
//<i>������<=85%
#define BUCHONG_CYCLE_TIMEOUT            180

//</h>

//<h>ֹͣ����
//<o>ֹͣ������С(ma)
#define STOP_CURRENT_MAX                   100
//<o>����ʱ��(s)
#define STOP_CURRENT_TIMEOUT                120
//</h>

//</h>
//<e> �Ƿ�ʹ��bootloader
#define USING_BOOT_LOADER                  0
//<o> bootloader��С<0x0-0xFFFF:4>
#define BOOT_LOADER_SIZE                   0x3000
//</e>




//�ж��Ƿ���6����
#define is_6_battery(ids)          if((ids[6]&0x0F)==0x06)              
//��ŷ����
#define dian_ci_fa(channel,level)  ld_gpio_set(channel->map->io_motor,level)

/******************��账����***************************/
#define Lease_fall   					0x00
#define Lease_success   			0x01
#define Lease_differ  				0x02
#define Lease_decrypt_fall    0x03
#define Lease_dianchifa_fall  0x04
/*********************************************************/

/******************�黹������***************************/
#define Return_fall   					0x00
#define Return_success   			  0x01
#define Return_unrecognized   	0x02
#define Return_timeout   	      0x03
/*********************************************************/

/******************����������***************************/
#define Cmd_fall   					  0x00
#define Cmd_success   			  0x01
/*********************************************************/

/******************���¹̼�0x06**************************/
#define deal_fault	        0x00      
#define updata_model        0x01      
#define normal_model			  0x02      
/*********************************************************/

/******************��������*******************************/
#define IR_GETID          	10   //��ȡID
#define IR_GETDATA          20   //��ȡ����
#define IR_OUTENABLE       	30   //�������
#define IR_OUTDISABLE      	50   //��ֹ���
#define IR_OUTANHOUR      	40   //���1Сʱ



#endif

