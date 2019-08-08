
#include "rtthread.h"
#include "mshell.h"
#include "driver.h"

int main(void)
{
	ld_dev_init();
	ld_uart_open(2,115200,8,0,1,0,256);
	while(1)
	{
		rt_thread_mdelay(100);
	}
	
}
