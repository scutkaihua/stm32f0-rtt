#include "mshell.h"

Export_DIR("/test",test,0);//¶¨ÒåÄ¿Â¼




long helloworld(void*arg)
{
	mshell_printf("hello world!");
	return 1;
}
Export(test,hw,helloworld,"say hello to world!");

long arg(void*arg1,int*data)
{
	mshell_printf("arg1:%s,data=%d",arg1!=NULL?arg1:"null",*data);
	return 1;
}
Export(test,arg,arg,"test arg");

long reset(void*arg)
{
	board_reset();
	return 1;
}
Export(test,rst,reset,"reset mcu.");