/*========================================================
* file name 	:	4G.c
* Author 			:	kaihua.yang
* Version			��	v1.00
* Date				��	9/11/2019
* Description	��	4Gģ������
========================================================*/
#include "driver_config_types.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "cpu.h"
/*************************************
							����
**************************************/
/*************************************
							���غ���
**************************************/
/*�������ս��*/
int _xg_wait_result(char*result,int timeout,char*in)
{

}

/*************************************
							����״̬��
**************************************/
static int line = 0;
#define start()           switch(line){case 0:
#define state(name)       break;name:line=__LINE__;return;case __LINE__:
#define defaultx()        break;default:{line=0;}break;}
void xg_fsm(void)
{
	start()
	{
	  
	}
	state(
	

}
/*************************************
							ȫ��
**************************************/
int ld_xg_init(void)
{}
	
	
int ld_xg_start(void)
{

}

int ld_xg_close(void)
{

}
