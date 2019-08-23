
#include "rtthread.h"
#include "mshell.h"
#include "includes.h"
#include "stdio.h"

int board_init(void)
{
	ld_dev_init();
	ld_uart_open(2,115200,8,0,1,50,150);
	enable_485_rx();
	return 1;
}
INIT_EXPORT(board_init, "2.board");

int main(void)
{
  mshell('\n');
	while(1)
	{
		{
			char ch=0;
			if(ld_uart_read(2,(U8*)&ch,1)==1)
				mshell(ch);
		}
		rt_thread_mdelay(10);
	}
	
}
