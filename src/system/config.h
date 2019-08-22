

#ifndef __APP_CONFIG_H__
#define __APP_CONFIG_H__

//===========================类型=============================//
typedef enum{
	PROCESS_EVENT_PACKET           =0x40,   //事件:数据包接收
	PROCESS_EVENT_COMM_LEASE,               //事件:租借数据包处理
	PROCESS_EVENT_COMM_CTRL,                //事件:控制命令
	PROCESS_EVENT_COMM_ENTRY,               //事件:进入升级
}PROCESS_EVENT;


//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//<e>是否使用看门狗
#define ENABLE_IWDG               0
//</e>

//<o>485串口号  <1=>uart1 <2=>uart2
#define COM_485                   2

//485发送使能
#define enable_485_tx()   ld_gpio_set(2,1)
#define disable_485_tx()  ld_gpio_set(2,0)
#define enable_485_rx()   disable_485_tx()
#define disable_485_rx()  enable_485_tx()

//<s>使用硬件配置
#define USING_HARDWARE_CONFIG  "25-50-NAS-V5.h"
#include USING_HARDWARE_CONFIG



//<o>允许同时充电的最大仓道数
#define CHANNEL_CHARGE_MAX    2

//<o>仓道编号长度
#define CHANNEL_ID_MAX             10

//<o>解锁时，失败尝试次数         
#define UNLOCK_RETRY_TIMES        2

//<o>租借时，仓道灯闪烁频率ms
#define LEASE_LED_FLASH_TIME      500

//<o>归还充电宝时，读数据尝试次数
#define RETURN_READ_TIMES         2

//<o>仓道灯亮时，最小电量%
#define CHANNEL_LED_LIGHT_UFSOC   50

//<o>循环读充电宝时长(ms)
#define BAO_READ_DATA_MAX_MS      2500

//<o>告警温度(度)
#define BAO_WARN_TEMPERATURE      60
//<o>故障温度(度)
#define BAO_ERROR_TEMPERATURE      65
//<h>充电宝读判断
//<o>失败次数
#define BAO_READ_ERROR_RETYR_TIMES         3
//<o>成功次数
#define BAO_READ_OK_RETYR_TIMES            1

//<o>顶针识别故障计数
#define BAO_DINGZHEN_ERROR_TIMES           2

//<o>红外识别故障计数
#define BAO_IR_ERROR_TIMES                 2
//</h>

//<h>锁仓事件
//<o>检测时间间隔(ms)
#define EJECT_INTERVAL                     10000
//<o>弹仓失败几次后延时一段时间(次数)
#define EJECT_FAIL_TIMES                    6
//<o>弹仓失败几次后延时时间(秒)
#define EJECT_FAIL_WAIT                     7200
//</h>

//<h>充电流程参数
//<h> 补充
//<o>停止补充电量%
#define BUCHONG_STOP_UFSOC_MAX            99
//<h>1小时补充
//<o>停止补充电量%
#define BUCHONG_1HOUR_STOP_UFSOC_MAX      85
//<o>补充次数
#define BUCHONG_1HOUR_TIMES               3
//</h>

//<o>无限补充时长 min
//<i>当电量<=85%
#define BUCHONG_CYCLE_TIMEOUT            180

//</h>

//<h>停止电流
//<o>停止电流大小(ma)
#define STOP_CURRENT_MAX                   100
//<o>持续时间(s)
#define STOP_CURRENT_TIMEOUT                120
//</h>

//</h>
//<e> 是否使用bootloader
#define USING_BOOT_LOADER                  0
//<o> bootloader大小<0x0-0xFFFF:4>
#define BOOT_LOADER_SIZE                   0x3000
//</e>




//判断是否是6代宝
#define is_6_battery(ids)          if((ids[6]&0x0F)==0x06)              
//电磁阀输出
#define dian_ci_fa(channel,level)  ld_gpio_set(channel->map->io_motor,level)

/******************租借处理结果***************************/
#define Lease_fall   					0x00
#define Lease_success   			0x01
#define Lease_differ  				0x02
#define Lease_decrypt_fall    0x03
#define Lease_dianchifa_fall  0x04
/*********************************************************/

/******************归还处理结果***************************/
#define Return_fall   					0x00
#define Return_success   			  0x01
#define Return_unrecognized   	0x02
#define Return_timeout   	      0x03
/*********************************************************/

/******************控制命令结果***************************/
#define Cmd_fall   					  0x00
#define Cmd_success   			  0x01
/*********************************************************/

/******************更新固件0x06**************************/
#define deal_fault	        0x00      
#define updata_model        0x01      
#define normal_model			  0x02      
/*********************************************************/

/******************红外命令*******************************/
#define IR_GETID          	10   //读取ID
#define IR_GETDATA          20   //获取数据
#define IR_OUTENABLE       	30   //允许输出
#define IR_OUTDISABLE      	50   //禁止输出
#define IR_OUTANHOUR      	40   //输出1小时



#endif

