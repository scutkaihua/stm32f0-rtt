
#include "rtthread.h"
#include "mshell.h"
#include "driver.h"
#include "stdio.h"

int board_init(void)
{
	ld_dev_init();
	ld_uart_open(2,115200,8,0,1,0,100);
	mshell_init();
	mshell_printf("hello\n");
	return 1;
}
INIT_EXPORT(board_init, "2.board");

int main(void)
{

	while(1)
	{
		printf("hello\r\n");
		rt_thread_mdelay(1000);
	}
	
}
