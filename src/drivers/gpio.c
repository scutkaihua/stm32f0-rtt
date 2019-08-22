#include "types.h"
#include "driver_config_types.h"



//static 
volatile U16 hc595data=0;//数据缓冲
/*===================================================
                配置文件
====================================================*/
extern t_gpio_map gpio_map[];
extern const unsigned char gpio_number;

extern void ld_hc595_init(void);
void ld_hc595_send(uint32_t data);
extern void ld_hc595_reload(void);

/*===================================================
                全局函数
====================================================*/
//初始化配置的iic接口
void ld_gpio_init(void)
{
  int i=0;
	for(;i<gpio_number;i++)
	{
		 cpu_gpio_map_config(gpio_map,i);
	}
	ld_hc595_init();
}

//设置电平
void ld_gpio_set(U32 index,U8 value)
{
	index--;
	if(index>=gpio_number)return;
	//HC595
	if(gpio_map[index].xPort==0xFF)
	{
		if(value==0){
			hc595data&=~(1<<gpio_map[index].xPin);
		}else{
			hc595data|= (1<<gpio_map[index].xPin);
		}
		//ld_hc595_send(hc595data);
	}
	else
   cpu_gpio_map_set(gpio_map,index,value);
	
}

/*读取电平
* index :io索引(1-n)
* return: 1:high  0:low
*/
U8 ld_gpio_get(U32 index)
{
	index--;
	if(index>=gpio_number)return 0;
	//HC595
	if(gpio_map[index].xPort==0xFF)
	{
			if(hc595data&(1<<gpio_map[index].xPin))return 1;
			else return 0;
	}
	else
   return cpu_gpio_map_get(gpio_map,index);
}

//设置输出/输入
void ld_gpio_set_io(U32 index,BOOL out,U8 value)
{
	index--;
	if(index>=gpio_number)return;	
	if(out){
				cpu_gpio_cfg(gpio_map[index].xPort,gpio_map[index].xPin,
				0x01,0x00,0x01,
				value,gpio_map[index].xSpeed);
	}else{
				cpu_gpio_cfg(gpio_map[index].xPort,gpio_map[index].xPin,
				0x00,0x01,0x00,
				value,gpio_map[index].xSpeed);	
	}
}


/*只对595有效*/
U8 ld_gpio_refresh(void)
{
	static U16 d = 0;
	if(d!=hc595data)
	{
		ld_hc595_send(hc595data);
		d=hc595data;
		return TRUE;
	}
	return FALSE;
}
