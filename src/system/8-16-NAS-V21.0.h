
#ifndef __8_16_NAS_H__
#define __8_16_NAS_H__

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------

//<s>PCB板编号
#define PCB_VERSION    "(LD-MZ-DRIVER-8-A-V21.0)"

//<o>硬件编号<0-0xFFFF:1>
//<i>带红外的硬件版本 0203
#define HARDWARE_VERSION          0x0203
//<o>软件编号<0-0xFFFF:1>
//<i>软件编号从0001开始叠加
#define SOFTWARE_VERSION          0x0006

//<o>仓道个数
#define CHANNEL_MAX           4


//电磁阀电源使能
#define dian_ci_fa_power(enable)   ld_gpio_set(43,enable)

//5V充电输出
#define set_out5v()      do{ld_gpio_set_io(pch->map->io_mp_detect,TRUE,1);ld_gpio_set(pch->map->io_mp,1);ld_gpio_set_io(pch->map->io_mp_detect,FALSE,0);}while(0)

//硬件io配置
#define driver_config_io_map() \
	/*电磁阀 摆臂开关 到位开关  红外发送  红外接收   led灯      充电使能  充电输出检测     sda       scl*/    \
	OUT(C,9) IN(B,10) IN(B,15)  OUT(C, 8)  IN(A,12)  OUT(A, 7)  OUT(B, 3)  IN(C,0)         ODH(B,8)  ODH(B,9) \
	OUT(C,7) IN(B,1)  IN(A,8)   OUT(C, 3)  IN(A,11)  OUT(A, 6)  OUT(A,15)  IN(C,1)         ODH(B,6)  ODH(B,7) \
	OUT(B,14)IN(A,0)  IN(F,6)   OUT(B, 2)  IN(B, 0)  OUT(A, 5)  OUT(B, 5)  IN(C,5)         ODH(C,12) ODH(D,2) \
	OUT(B,12)IN(C,13) IN(F,7)   OUT(B,11)  IN(B,13)  OUT(A, 4)  OUT(B, 4)  IN(C,4)         ODH(C,10) ODH(C,11) \
	/*电磁阀电源使能*/OUT(C,2)

//硬件exti配置
#define driver_config_exti_map()\
	/*仓道1*/exti(B,15)  \
	/*仓道2*/exti(A, 8)  \
	/*仓道3*/exti(F, 6)  \
	/*仓道4*/exti(F, 7)  

#endif
