#include "mshell.h"

Export_DIR("/test",test,0);//¶¨ÒåÄ¿Â¼




long helloworld(void*arg)
{
	mshell_printf("hello world!");
	return 1;
}
Export(test,hw,helloworld,"say hello to world!");

long arg(void*arg)
{
	mshell_printf("arg:%s",arg!=NULL?arg:"null");
	return 1;
}
Export(test,arg,arg,"test arg");