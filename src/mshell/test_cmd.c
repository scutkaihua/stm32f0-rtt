#include "mshell.h"

Export_DIR("/test",test,0);//����Ŀ¼
Export_DIR("/test/info",info,2);//����Ŀ¼

int helloworld(void*arg)
{
	mshell_printf("hello world!");
	return -1;
}
Export(test,hw,helloworld,"say hello to world!");

int arg(int data,void*arg1)
{
	mshell_printf("data=%d��arg1:%s",data,arg1!=NULL?arg1:"null");
	return 1;
}
Export(test,arg,arg,"test arg");

int dinfo(void*arg)
{
	mshell_printf("this is mshell information!");
	return 1;
}
Export(info,info,dinfo,"information.");
