
#include "rtthread.h"
#include "mshell.h"
#include "includes.h"
#include "stdio.h"

int board_init(void)
{
	ld_dev_init();
	ld_uart_open(1,115200,8,0,1,50,1024);
	ld_spi_flash_init(1);
	return 1;
}
INIT_EXPORT(board_init, "2.board");

int main(void)
{
	int i = 0;
	mshell('\n');
	while(1)
	{
		{
			char ch=0;
			if(ld_uart_read(1,(U8*)&ch,1)==1)
			{
				mshell(ch);
				ld_gpio_set(1,!(ld_gpio_get(1)));
			}
		}
		rt_thread_mdelay(10);
		i++;
		if(i%25==0)
			ld_gpio_set(1,!(ld_gpio_get(1)));
	}
	
}
